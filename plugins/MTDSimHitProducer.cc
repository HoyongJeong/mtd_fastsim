////////////////////////////////////////////////////////////////////////////////
///
///   MTDSimHitProducer.cc
///
///   MTD fastsim try...
///
/// - Hoyong Jeong (hoyong5419@korea.ac.kr, Korea University)
///
////////////////////////////////////////////////////////////////////////////////



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "MTDSimHitProducer.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "TrackingTools/TrajectoryParametrization/interface/GlobalTrajectoryParameters.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"

// MTD SubDetector ID
#include "DataFormats/ForwardDetId/interface/MTDDetId.h"
#include "DataFormats/ForwardDetId/interface/BTLDetId.h"
#include "DataFormats/ForwardDetId/interface/ETLDetId.h"

#include <iostream>



///-----------------------------------------------------------------------------
/// Define constructor
///-----------------------------------------------------------------------------
MTDSimHitProducer::MTDSimHitProducer(const edm::ParameterSet& iConfig)
	: mtdGeoToken_    (esConsumes<MTDGeometry  , MTDDigiGeometryRecord>()   )
	, magFieldToken_  (esConsumes<MagneticField, IdealMagneticFieldRecord>())
	, propagatorToken_(esConsumes<Propagator   , TrackingComponentsRecord>(
		edm::ESInputTag("", iConfig . getParameter<std::string>("propagatorName"))))
	, simTrackToken_ (consumes<edm::SimTrackContainer> (iConfig . getParameter<edm::InputTag>("simTrackLabel" )))
	, simVertexToken_(consumes<edm::SimVertexContainer>(iConfig . getParameter<edm::InputTag>("simVertexLabel")))
{
	//----------------------------------------------------------
	// Debugging message
	//----------------------------------------------------------
	std::cout << "[MTDSimHitProducer::MTDSimHitProducer] Constructed" << std::endl;


	//----------------------------------------------------------
	// Define labels of data product that this module produces
	//----------------------------------------------------------
	produces<std::vector<PSimHit>>("FastTimerHitsBarrelByFastsim");
	produces<std::vector<PSimHit>>("FastTimerHitsEndcapByFastsim");
}



///-----------------------------------------------------------------------------
/// Produce method
///-----------------------------------------------------------------------------
void MTDSimHitProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const
{
	//----------------------------------------------------------
	// Debugging message
	//----------------------------------------------------------
	std::cout << "[MTDSimHitProducer::produce] Executed" << std::endl;


	//----------------------------------------------------------
	// Get things from EventSetup
	//----------------------------------------------------------
	const MTDGeometry  * mtdGeom    = &iSetup . getData(mtdGeoToken_    );
	const MagneticField* magField   = &iSetup . getData(magFieldToken_  );
	const Propagator   * propagator = &iSetup . getData(propagatorToken_);


	//----------------------------------------------------------
	// Read SimTrack/SimVertex
	//----------------------------------------------------------
	const auto& simTracks   = iEvent . get(simTrackToken_ );
	const auto& simVertices = iEvent . get(simVertexToken_);
//	std::cout << "[MTDSimHitProducer::produce]"
//		<< "  SimTracks  : " << simTracks   . size()
//		<< ", SimVertices: " << simVertices . size() << std::endl;


	//----------------------------------------------------------
	// Define containers for data output
	//----------------------------------------------------------
	auto btlHits = std::make_unique<edm::PSimHitContainer>();
	auto etlHits = std::make_unique<edm::PSimHitContainer>();


	//----------------------------------------------------------
	// Looping over tracks
	//----------------------------------------------------------
	for ( const SimTrack& track : simTracks )
	{
		//--------------------------------------
		// Neutral tracks -> I don't care. Pass!
		//--------------------------------------
		if ( track . charge() == 0 ) continue;

		//--------------------------------------
		// Find SimVertex
		// No info about the origin if track . vertIndex() were -1
		//--------------------------------------
		if ( track . noVertex() ) continue;
		const SimVertex& vertex = simVertices . at(track . vertIndex());

		//--------------------------------------
		// Check position and momentum
		//--------------------------------------
//		const math::XYZTLorentzVectorD& momentum = track  . momentum(); // (px, py, pz, E)
//		const math::XYZTLorentzVectorD& vtxPos   = vertex . position(); // ( x,  y,  z, t)
//		std::cout << "[MTDSimHitProducer::produce]"
//			<< "  Track PDG=" << track    . type()
//			<< "  pT="        << momentum . Pt()
//			<< "  eta="       << momentum . Eta()
//			<< "  vtx=("      << vtxPos   . x() << ","
//			                  << vtxPos   . y() << ","
//			                  << vtxPos   . z() << ")" << std::endl;

		//--------------------------------------
		// Create FreeTrajectoryState
		//--------------------------------------
		FreeTrajectoryState fts = makeFTS(track, vertex, magField);

		//--------------------------------------
		// Time of Flight accounted from creation time (ns)
        // speed of light = 29.9792458 cm/ns
		//--------------------------------------
		const float c_cm_ns = 29.9792458;

		//--------------------------------------
		// Looping over MTD modules -> propagate toward each module surface
		//--------------------------------------
		for ( const GeomDet* det : mtdGeom -> dets())
		{
			const BoundPlane& surface = det -> surface();

			// Propagate the track to current module
			TrajectoryStateOnSurface tsos = propagator -> propagate(fts, surface);

			// Continue if failed to propagate
			if ( ! tsos . isValid() ) continue;

			// Is propagated location in the boundary?
			const LocalPoint& localPos = tsos . localPosition();
			if ( ! surface . bounds() . inside(localPos) ) continue;

			// Calculate ToF: from creation to hit / c + creation time
			GlobalPoint hitGlobal = det -> surface() . toGlobal(localPos);
			GlobalPoint vtxGlobal(vertex . position() . x(),
			                      vertex . position() . y(),
			                      vertex . position() . z());
			float dist = (hitGlobal - vtxGlobal) . mag(); // cm
			float tof  = dist / c_cm_ns + static_cast<float>(vertex . position() . t()); // ns

			// Define a PSimHit
			PSimHit hit = makeSimHit(tsos, det, track, tof);

			// BTL vs ETL: which one?
			DetId detId(det -> geographicalId());
			if      ( detId . subdetId() == MTDDetId::BTL )
			{
				btlHits -> push_back(hit);
			}
			else if ( detId . subdetId() == MTDDetId::ETL )
			{
				etlHits -> push_back(hit);
			}
		}
	}


	//----------------------------------------------------------
	// Sim hit summary
	//----------------------------------------------------------
	std::cout << "[MTDSimHitProducer::produce]"
		<< "  BTL hits: " << btlHits -> size()
		<< "  ETL hits: " << etlHits -> size() << std::endl;


	//----------------------------------------------------------
	// Put created data into an event
	//----------------------------------------------------------
	iEvent . put(std::move(btlHits), "FastTimerHitsBarrelByFastsim");
	iEvent . put(std::move(etlHits), "FastTimerHitsEndcapByFastsim");
}


///-----------------------------------------------------------------------------
/// Fill descriptions
///-----------------------------------------------------------------------------
void MTDSimHitProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
	edm::ParameterSetDescription desc;
	desc . add<edm::InputTag>("simTrackLabel" , edm::InputTag("g4SimHits"));
	desc . add<edm::InputTag>("simVertexLabel", edm::InputTag("g4SimHits"));
	desc . add<std::string>  ("propagatorName", "PropagatorWithMaterial"  );


	//----------------------------------------------------------
	// If there are parameters to be received from the Python configuration file, define them here.
	//----------------------------------------------------------
	descriptions . add("mtdSimHitProducer", desc);
}



///-----------------------------------------------------------------------------
/// Free trajectory state generator (FTS)
/// It makes FTS using momentum and position
///-----------------------------------------------------------------------------
FreeTrajectoryState MTDSimHitProducer::makeFTS(const SimTrack&      track,
                                               const SimVertex&     vertex,
                                               const MagneticField* field) const
{
	//----------------------------------------------------------
	// Location of SimVertex -> GlobalPoint (Unit maybe cm?)
	//----------------------------------------------------------
	GlobalPoint position(
		vertex . position() . x(),
		vertex . position() . y(),
		vertex . position() . z()
	);


	//----------------------------------------------------------
	// Momentum of SimTrack -> GlobalVector (Unit maybe GeV?)
	//----------------------------------------------------------
	GlobalVector momentum(
		track . momentum() . x(),
		track . momentum() . y(),
		track . momentum() . z()
	);


	//----------------------------------------------------------
	// Charge (charge() returns float type. Let's convert it into integer)
	//----------------------------------------------------------
	int charge = static_cast<int>(track . charge());


	GlobalTrajectoryParameters gtp(position, momentum, charge, field);
	return FreeTrajectoryState(gtp);
}



///-----------------------------------------------------------------------------
/// PSimHit generator
/// Define a PSimHit based on TrajectoryStateOnSurface (TSOS) and track info, then return it.
///-----------------------------------------------------------------------------
PSimHit MTDSimHitProducer::makeSimHit(const TrajectoryStateOnSurface& tsos,
                                      const GeomDet*                  det,
                                      const SimTrack&                 track,
                                      float                           tof) const
{
	//----------------------------------------------------------
	// Global to local already done by TSOS
	//----------------------------------------------------------
	const LocalPoint&  localPos = tsos . localPosition();
	const LocalVector& localMom = tsos . localMomentum();


	//----------------------------------------------------------
	// MTD is thin so let us approximate entryPoint to exitPoint
	//----------------------------------------------------------
	LocalPoint entry(localPos . x(), localPos . y(), -0.15f); // +/- 1.5 mm?
	LocalPoint exit (localPos . x(), localPos . y(),  0.15f);

	float    pabs       = tsos . globalMomentum() . mag();  // Magnitude of momentum
	float    energyLoss = 0.0015f;                          // ~ 1.5 MeV (Let me adjust later...)
	int      pdgId      = track . type();
	uint32_t detId      = det -> geographicalId() . rawId();
	uint16_t process    = 0;                                // 0 means undefined (FastSim)

	return PSimHit(entry, exit, pabs, tof, energyLoss, pdgId, detId, track . trackId(),
	               // localMom's theta and phi (Direction of motion)
	               localMom . theta(), localMom . phi(),
	               process);
}



///-----------------------------------------------------------------------------
/// Register this class as a module in the CMSSW framework.
///-----------------------------------------------------------------------------
DEFINE_FWK_MODULE(MTDSimHitProducer);
