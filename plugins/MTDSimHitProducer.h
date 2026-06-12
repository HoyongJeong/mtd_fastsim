#ifndef FastSimulation_MTD_MTDSimHitProducer_h
#define FastSimulation_MTD_MTDSimHitProducer_h



////////////////////////////////////////////////////////////////////////////////
///
///   MTDSimHitProducer.h
///
///   MTD fastsim try...
///
///   This is a producer code that generates a Hit
/// when a particle passes through the MTD layer?
///
/// - Hoyong Jeong (Korea university)
///
////////////////////////////////////////////////////////////////////////////////



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include <memory>

#include "FastSimulation/SimplifiedGeometryPropagator/interface/InteractionModel.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"



namespace edm
{
	class ParameterSet;
	class ProducesCollector;
	class Event;
}



namespace fastsim
{
	///-----------------------------------------------------------------------------
	/// Class declaration: produces SimHits in the MTD layers
	///-----------------------------------------------------------------------------
	class MTDSimHitProducer : public InteractionModel
	{
		public:
		//----------------------------------------------------------
		// Constructor and destructor
		//----------------------------------------------------------
		MTDSimHitProducer(const std::string& name, const edm::ParameterSet& cfg);
		~MTDSimHitProducer() override = default;


		//----------------------------------------------------------
		// Public method
		//----------------------------------------------------------
		//--------------------------------------
		// Perform the interaction.
		// \param particle    The particle that interacts with the matter.
		// \param layer       The detector layer that interacts with the particle.
		// \param secondaries Particles that are produced in the interaction (if any).
		// \param random      The Random Engine.
		//--------------------------------------
		void interact(Particle& particle,
		              const SimplifiedGeometry& layer,
		              std::vector<std::unique_ptr<Particle>>& secondaries,
		              const RandomEngineAndDistribution& random) override;

		//--------------------------------------
		// Register the SimHit collection
		//--------------------------------------
		void registerProducts(edm::ProducesCollector producesCollector) const override;

		//--------------------------------------
		// Store the SimHit collection
		//--------------------------------------
		void storeProducts(edm::Event& iEvent) override;


		private:
		//----------------------------------------------------------
		// Geometry parameters
		//----------------------------------------------------------
		const double btlRadius_;
		const double btlHalfLength_;
		const double etlRMin_;
		const double etlRMax_;


		//----------------------------------------------------------
		// hit containers
		//----------------------------------------------------------
		std::unique_ptr<edm::PSimHitContainer> btlHits_;
		std::unique_ptr<edm::PSimHitContainer> etlHits_;
	};
} // namespace fastsim



#endif
