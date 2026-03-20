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
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

#include <vector>
#include <memory>



///-----------------------------------------------------------------------------
/// Class declaration
///-----------------------------------------------------------------------------
class MTDSimHitProducer : public edm::global::EDProducer<>
{
	public:
	//----------------------------------------------------------
	// Constructor and destructor
	//----------------------------------------------------------
	explicit MTDSimHitProducer(const edm::ParameterSet&);
	~MTDSimHitProducer() override = default;


	//----------------------------------------------------------
	// Public method
	//----------------------------------------------------------
	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;


	private:
	// Later, particle trajectory calculators, Geometry handlers, etc. will be declared here.
	// Configuration values ​​received from edm::ParameterSet are also stored here.
};



#endif
