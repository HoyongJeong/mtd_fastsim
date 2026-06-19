////////////////////////////////////////////////////////////////////////////////
///
///   MTDSimHitProducer.cc
///
///   MTD fastsim try since 2026. Please work...
///
/// - Hoyong Jeong (hoyong5419@korea.ac.kr, Korea University)
///
////////////////////////////////////////////////////////////////////////////////



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "MTDSimHitProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ProducesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"

#include "FastSimulation/SimplifiedGeometryPropagator/interface/Particle.h"
#include "FastSimulation/SimplifiedGeometryPropagator/interface/SimplifiedGeometry.h"
#include "FastSimulation/SimplifiedGeometryPropagator/interface/InteractionModelFactory.h"
#include "FastSimulation/SimplifiedGeometryPropagator/interface/Constants.h"

#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

#include "DataFormats/GeometryVector/interface/LocalPoint.h"

#include <cmath>



namespace fastsim
{
	///-----------------------------------------------------------------------------
	/// Define constructor
	///-----------------------------------------------------------------------------
	MTDSimHitProducer::MTDSimHitProducer(const std::string& name, const edm::ParameterSet& cfg)
		: InteractionModel(name)
		, btlRadius_     (cfg . getParameter<double>("btlRadius"    ))
		, btlHalfLength_ (cfg . getParameter<double>("btlHalfLength"))
		, etlRMin_       (cfg . getParameter<double>("etlRMin"      ))
		, etlRMax_       (cfg . getParameter<double>("etlRMax"      ))
		, btlHits_       (std::make_unique<edm::PSimHitContainer>()  )
		, etlHits_       (std::make_unique<edm::PSimHitContainer>()  )
	{
		//----------------------------------------------------------
		// Debugging message
		//----------------------------------------------------------
		std::cout << "[MTDSimHitProducer::MTDSimHitProducer] Constructed" << std::endl;
	}



	///-----------------------------------------------------------------------------
	/// Register the SimHit collection
	///-----------------------------------------------------------------------------
	void MTDSimHitProducer::registerProducts(edm::ProducesCollector producesCollector) const
	{
		producesCollector . produces<edm::PSimHitContainer>("FastSimMTDBarrel");
		producesCollector . produces<edm::PSimHitContainer>("FastSimMTDEndcap");
	}



	///-----------------------------------------------------------------------------
	/// Store the SimHit collection
	///-----------------------------------------------------------------------------
	void MTDSimHitProducer::storeProducts(edm::Event& iEvent)
	{
		iEvent . put(std::move(btlHits_), "FastSimMTDBarrel");
		iEvent . put(std::move(etlHits_), "FastSimMTDEndcap");
		btlHits_ = std::make_unique<edm::PSimHitContainer>();
		etlHits_ = std::make_unique<edm::PSimHitContainer>();
	}



	///-----------------------------------------------------------------------------
	/// Interact method
	///-----------------------------------------------------------------------------
	void MTDSimHitProducer::interact(Particle& particle,
	                                 const SimplifiedGeometry& layer,
	                                 std::vector<std::unique_ptr<Particle>>& secondaries,
	                                 const RandomEngineAndDistribution& random)
	{
		//----------------------------------------------------------
		// Debugging message
		//----------------------------------------------------------
//		std::cout << "[MTDSimHitProducer::interact] Called"
//		          << " isForward="    << layer    . isForward()
//		          << " geomProperty=" << layer    . getGeomProperty()
//		          << " charge="       << particle . charge()
//		          << " p="            << particle . momentum() . P()
//		          << " eta="          << particle . momentum() . Eta()
//		          << std::endl;


		//----------------------------------------------------------
		// Read position and momentum
		//----------------------------------------------------------
		const double x  = particle . position() . X();
		const double y  = particle . position() . Y();
		const double z  = particle . position() . Z();
		const double px = particle . momentum() . X();
		const double py = particle . momentum() . Y();
		const double pz = particle . momentum() . Z();
		const double p  = particle . momentum() . P();


		//----------------------------------------------------------
		// Exceptions
		//----------------------------------------------------------
		//--------------------------------------
		// Ignore the neutral
		//--------------------------------------
		if ( particle . charge() == 0 ) return;

		//--------------------------------------
		// No material
		//--------------------------------------
		if ( layer . getThickness(particle . position(), particle . momentum()) < 1E-10 ) return;

		//--------------------------------------
		// Ignore the rest
		//--------------------------------------
		if ( p == 0. ) return;


		//----------------------------------------------------------
		// The energy deposit in the layer
		//----------------------------------------------------------
		const double energyDeposit = particle . getEnergyDeposit();
		particle . setEnergyDeposit(0); // I've just copied this from TrackerSimHitProducer


		//----------------------------------------------------------
		// simTrackId: use mother if there is
		//----------------------------------------------------------
		const int simTrackId = particle . getMotherSimTrackIndex() >= 0
		                     ? particle . getMotherSimTrackIndex()
		                     : particle . simTrackIndex();


		//----------------------------------------------------------
		// Get time when the particle hits the layer 
		//----------------------------------------------------------
		// Approximate way: t = r/v
//		const float tof = static_cast<float>(particle . position() . R() / fastsim::Constants::speedOfLight);
		// Read from pos = (x, y, z, t)
		const float tof = static_cast<float>(particle . position() . T());


		//----------------------------------------------------------
		// Hit point 
		//----------------------------------------------------------
		const Local3DPoint hitPoint(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));


		//----------------------------------------------------------
		// BTL: barrel layer
		//----------------------------------------------------------
		if ( ! layer . isForward() )
		{
			if ( std::abs(z) > btlHalfLength_ ) return;

			btlHits_ -> emplace_back(hitPoint,                          // Entry
			                         hitPoint,                          // Exit (point-like)
			                         static_cast<float>(p),             // Pabs
			                         tof,                               // Time of flight
			                         static_cast<float>(energyDeposit), // Eloss
			                         particle . pdgId(),                // Particle ID
			                         0U,                                // DetUnitId (placeholder)
			                         simTrackId,                        // Track ID
			                         0.f,                               // Theta
			                         0.f);                              // Phi
		}
		//----------------------------------------------------------
		// ETL: forward layer
		//----------------------------------------------------------
		else
		{
			const double r = std::sqrt(x*x + y*y);
			if ( r < etlRMin_ || r > etlRMax_ ) return;

			etlHits_ -> emplace_back(hitPoint,
			                         hitPoint,
			                         static_cast<float>(p),
			                         tof,
			                         static_cast<float>(energyDeposit),
			                         particle . pdgId(),
			                         0U,
			                         simTrackId,
			                         0.f,
			                         0.f);
		}
	}
} // namespace fastsim



DEFINE_EDM_PLUGIN(fastsim::InteractionModelFactory,
                  fastsim::MTDSimHitProducer      ,
                  "fastsim::MTDSimHitProducer"    );
