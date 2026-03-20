////////////////////////////////////////////////////////////////////////////////
///
///   MTDSimHitProducer.cc
///
///   MTD fastsim try...
///
/// - Hoyong Jeong (Korea university)
///
////////////////////////////////////////////////////////////////////////////////



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "FastSimulation/MTD/plugins/MTDSimHitProducer.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>



///-----------------------------------------------------------------------------
/// Define constructor
///-----------------------------------------------------------------------------
MTDSimHitProducer::MTDSimHitProducer(const edm::ParameterSet& iConfig)
{
	//----------------------------------------------------------
	// Debugging message
	//----------------------------------------------------------
	std::cout << "MTDSimHitProducer constructed" << std::endl;


	//----------------------------------------------------------
	// Define labels of data product that this module produces
	//----------------------------------------------------------
	produces<std::vector<PSimHit>>("FastTimerHitsBarrel");
	produces<std::vector<PSimHit>>("FastTimerHitsEndcap");
}



///-----------------------------------------------------------------------------
/// Produce method
///-----------------------------------------------------------------------------
void MTDSimHitProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const
{
	//----------------------------------------------------------
	// Debugging message
	//----------------------------------------------------------
	std::cout << "MTDSimHitProducer::produce executed" << std::endl;


	//----------------------------------------------------------
	// 1. Define containers for data
	//----------------------------------------------------------
	auto btlHits = std::make_unique<std::vector<PSimHit>>();
	auto etlHits = std::make_unique<std::vector<PSimHit>>();


	//----------------------------------------------------------
	// 2. Core logic placed here
	//    Read the event here, track the particle trajectory, and push_back Hit at the point where it meets MTD.
	//    Ex: btlHits -> emplace_back(...);
	//----------------------------------------------------------


	//----------------------------------------------------------
	// 3. Put created data into an event
	//----------------------------------------------------------
	iEvent . put(std::move(btlHits), "FastTimerHitsBarrel");
	iEvent . put(std::move(etlHits), "FastTimerHitsEndcap");
}


///-----------------------------------------------------------------------------
/// Fill descriptions
///-----------------------------------------------------------------------------
void MTDSimHitProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
	edm::ParameterSetDescription desc;


	//----------------------------------------------------------
	// If there are parameters to be received from the Python configuration file, define them here.
	//----------------------------------------------------------
	descriptions . add("mtdSimHitProducer", desc);
}



///-----------------------------------------------------------------------------
/// Register this class as a module in the CMSSW framework.
///-----------------------------------------------------------------------------
DEFINE_FWK_MODULE(MTDSimHitProducer);
