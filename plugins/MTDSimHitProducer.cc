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
#include "FastSimulation/SimplifiedGeometryPropagator/interface/Constants.h"
#include <cmath>



///-----------------------------------------------------------------------------
/// Define constructor
///-----------------------------------------------------------------------------
MTDSimHitProducer::MTDSimHitProducer(const edm::ParameterSet& cfg)
	: btlRadius_     (cfg . getParameter<double>("btlRadius"    ))
	, btlHalfLength_ (cfg . getParameter<double>("btlHalfLength"))
	, etlZ_          (cfg . getParameter<double>("etlZ"         ))
	, etlRMin_       (cfg . getParameter<double>("etlRMin"      ))
	, etlRMax_       (cfg . getParameter<double>("etlRMax"      ))
	, simTrackToken_ (consumes<edm::SimTrackContainer> (cfg . getParameter<edm::InputTag>("simTracks"  )))
	, simVertexToken_(consumes<edm::SimVertexContainer>(cfg . getParameter<edm::InputTag>("simVertices")))
{
	//----------------------------------------------------------
	// Debugging message
	//----------------------------------------------------------
	std::cout << "[MTDSimHitProducer::MTDSimHitProducer] Constructed" << std::endl;


	//----------------------------------------------------------
	// Define labels of data product that this module produces
	//----------------------------------------------------------
	produces<edm::PSimHitContainer>("FastSimMTDBarrel");
	produces<edm::PSimHitContainer>("FastSimMTDEndcap");
}



///-----------------------------------------------------------------------------
/// Produce method
///-----------------------------------------------------------------------------
void MTDSimHitProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup&) const
{
	//----------------------------------------------------------
	// Debugging message
	//----------------------------------------------------------
	std::cout << "[MTDSimHitProducer::produce] Executed" << std::endl;


	//----------------------------------------------------------
	// Define containers for data output
	//----------------------------------------------------------
	auto btlHits = std::make_unique<edm::PSimHitContainer>();
	auto etlHits = std::make_unique<edm::PSimHitContainer>();


	//----------------------------------------------------------
	// Read SimTrack/SimVertex
	//----------------------------------------------------------
	const auto& simTracks   = iEvent . get(simTrackToken_ );
	const auto& simVertices = iEvent . get(simVertexToken_);


	//----------------------------------------------------------
	// Looping over tracks
	//----------------------------------------------------------
	for ( const auto& track : simTracks )
	{
		//--------------------------------------
		// Neutral tracks -> I don't care. Pass!
		//--------------------------------------
		if ( track . charge() == 0 ) continue;

		//--------------------------------------
		// Get start position and momentum from the vertex
		//--------------------------------------
		unsigned int vtxIdx = track . vertIndex();
        if ( vtxIdx >= simVertices . size() ) continue;

		const auto& vtx = simVertices[vtxIdx];
		const math::XYZTLorentzVectorD pos4 = vtx . position();

		double x  = pos4 . X();
		double y  = pos4 . Y();
		double z  = pos4 . Z();
		double px = track . momentum() . X();
		double py = track . momentum() . Y();
		double pz = track . momentum() . Z();
		double p  = track . momentum() . P();
        if ( p == 0. ) continue;

		//--------------------------------------
		// BTL: intersection with infinite cylinder r=btlRadius_
		// solve: (x + px*t)^2 + (y + py*t)^2 = R^2
		//--------------------------------------
		{
			double a    = px*px + py*py;
			double b    = 2. * (x*px + y*py);
			double c    = x*x + y*y - btlRadius_*btlRadius_;
			double disc = b*b - 4.*a*c;

			if ( a > 0. && disc >= 0. )
			{
				double sqrtDisc = std::sqrt(disc);
				//------------------
				// Take the forward solution (smallest positive t)
				//------------------
				for ( double sign : {-1., 1.} )
				{
					double t = (-b + sign*sqrtDisc) / (2.*a);
					if ( t <= 0. ) continue;

					double hx = x + px*t;
					double hy = y + py*t;
					double hz = z + pz*t;

					// check z acceptance
					if ( std::abs(hz) > btlHalfLength_ ) continue;

					float tof = static_cast<float>(std::sqrt(hx*hx + hy*hy + hz*hz) / fastsim::Constants::speedOfLight); // ns

					// local frame: just use global coords as proxy (no real geometry)
					Local3DPoint entry(static_cast<float>(hx), static_cast<float>(hy), static_cast<float>(hz));
					Local3DPoint exit = entry; // point-like for now

					btlHits->emplace_back(entry,
					                      exit,
					                      static_cast<float>(p), // pabs
					                      tof,
					                      0.f,                   // eloss (placeholder)
					                      track . type(),        // pdgId
					                      0,                     // detUnitId (placeholder)
					                      track . trackId(),
					                      0.f,                   // theta
					                      0.f);                  // phi

					break; // first valid intersection is enough
				}
			}
		}

		//--------------------------------------
		// ETL: intersection with disks at z = ±etlZ_
		// solve: z + pz*t = ±etlZ_  =>  t = (±etlZ_ - z) / pz
		//--------------------------------------
		if ( pz != 0. )
		{
			for (double diskZ : {etlZ_, -etlZ_})
			{
				double t = (diskZ - z) / pz;
				if ( t <= 0. ) continue;

				
				double hx = x + px*t;
				double hy = y + py*t;
				double hr = std::sqrt(hx*hx + hy*hy);

				if ( hr < etlRMin_ || hr > etlRMax_ ) continue;

				float tof = static_cast<float>(std::sqrt(hx*hx + hy*hy + diskZ*diskZ) / fastsim::Constants::speedOfLight);

				Local3DPoint entry(static_cast<float>(hx), static_cast<float>(hy), static_cast<float>(diskZ));
				Local3DPoint exit = entry;

				etlHits -> emplace_back(ientry,
				                        exit,
				                        static_cast<float>(p),
				                        tof,
				                        0.f,
				                        track.type(),
				                        0,
				                        track.trackId(),
				                        0.f,
				                        0.f);
			}
		}
	}


	//----------------------------------------------------------
	// Put created data into an event
	//----------------------------------------------------------
	iEvent . put(std::move(btlHits), "FastSimMTDBarrel");
	iEvent . put(std::move(etlHits), "FastSimMTDEndcap");
}


///-----------------------------------------------------------------------------
/// Fill descriptions
///-----------------------------------------------------------------------------
void MTDSimHitProducer::fillDescriptions(edm::ConfigurationDescriptions& descs)
{
	edm::ParameterSetDescription desc;

	desc . add<double>("btlRadius"    , 116.);  // cm
	desc . add<double>("btlHalfLength", 260.);  // cm
	desc . add<double>("etlZ"         , 290.);  // cm
	desc . add<double>("etlRMin"      ,  31.);  // cm
	desc . add<double>("etlRMax"      , 120.);  // cm
	desc . add<edm::InputTag>("simTracks"  , edm::InputTag("fastSimProducer"));
	desc . add<edm::InputTag>("simVertices", edm::InputTag("fastSimProducer"));

	descs . addDefault(desc);
}



DEFINE_FWK_MODULE(MTDSimHitProducer);
