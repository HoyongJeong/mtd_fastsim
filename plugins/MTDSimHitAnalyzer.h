#ifndef FastSimulation_MTD_MTDSimHitAnalyzer_h
#define FastSimulation_MTD_MTDSimHitAnalyzer_h



////////////////////////////////////////////////////////////////////////////////
///
///   MTDSimHitAnalyzer.h
///
///   A simple analyzer to read hit result
///
/// - Hoyong Jeong (Korea university)
///
////////////////////////////////////////////////////////////////////////////////



///----------------------------------------------------------------------------
/// Headers
///----------------------------------------------------------------------------
#include "FWCore/Framework/interface/global/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"



///----------------------------------------------------------------------------
/// Class declaration
///----------------------------------------------------------------------------
class MTDSimHitAnalyzer : public edm::global::EDAnalyzer<>
{
	public:
	//----------------------------------------------------------
	// Constructor and destructor
	//----------------------------------------------------------
	explicit MTDSimHitAnalyzer(const edm::ParameterSet& iConfig);


	//----------------------------------------------------------
	// Public methods
	//----------------------------------------------------------
	void analyze(edm::StreamID, const edm::Event& iEvent, const edm::EventSetup&) const override;


	private:
	//----------------------------------------------------------
	// PSimHit tokens
	//----------------------------------------------------------
	edm::EDGetTokenT<edm::PSimHitContainer> btlToken_;
	edm::EDGetTokenT<edm::PSimHitContainer> etlToken_;
};



#endif
