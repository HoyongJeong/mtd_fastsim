////////////////////////////////////////////////////////////////////////////////
///
///   MTDSimHitAnalyzer.cc
///
///   The definition of MTDSimHitAnalyzer class
///
/// - Hoyong Jeong (hoyong5419@korea.ac.kr, Korea University)
///
////////////////////////////////////////////////////////////////////////////////



///-----------------------------------------------------------------------------
/// Headers
///-----------------------------------------------------------------------------
#include "MTDSimHitAnalyzer.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "SimDataFormats/TrackingHit/interface/PSimHit.h"



///-----------------------------------------------------------------------------
/// Define constructor
///-----------------------------------------------------------------------------
MTDSimHitAnalyzer::MTDSimHitAnalyzer(const edm::ParameterSet& iConfig)
	: btlToken_(consumes<edm::PSimHitContainer>(iConfig . getParameter<edm::InputTag>("btlHitsLabel")))
	, etlToken_(consumes<edm::PSimHitContainer>(iConfig . getParameter<edm::InputTag>("etlHitsLabel")))
{
}



///-----------------------------------------------------------------------------
/// Analyze method
///-----------------------------------------------------------------------------
void MTDSimHitAnalyzer::analyze(edm::StreamID, const edm::Event& iEvent, const edm::EventSetup&) const
{
	const auto& btlHits = iEvent . get(btlToken_);
	const auto& etlHits = iEvent . get(etlToken_);


	edm::LogInfo("MTDSimHitAnalyzer")
		<< "=== Event " << iEvent . id() . event() << " ==="
		<< "  BTL=" << btlHits . size()
		<< "  ETL=" << etlHits . size();
	

	for ( const auto& hit : btlHits )
	{
		edm::LogInfo("MTDSimHitAnalyzer")
			<< "[BTL] "
			<< "  detId="   << hit . detUnitId()
			<< "  trackId=" << hit . trackId()
			<< "  pdg="     << hit . particleType()
			<< "  tof="     << hit . timeOfFlight()  << " ns"
			<< "  eloss="   << hit . energyLoss()    << " GeV"
			<< "  pabs="    << hit . pabs()          << " GeV"
			<< "  entry=("  << hit . entryPoint() . x()
			<< ","          << hit . entryPoint() . y()
			<< ","          << hit . entryPoint() . z() << ")"
			<< "  exit=("   << hit . exitPoint() . x()
			<< ","          << hit . exitPoint() . y()
			<< ","          << hit . exitPoint() . z() << ")";
	}


	for ( const auto& hit : etlHits )
	{
		edm::LogInfo("MTDSimHitAnalyzer")
			<< "[ETL]"
			<< "  detId="   << hit . detUnitId()
			<< "  tof="     << hit . timeOfFlight() << " ns"
			<< "  eloss="   << hit . energyLoss()   << " GeV";
	}
}



///-----------------------------------------------------------------------------
/// Register this class as a module in the CMSSW framework.
///-----------------------------------------------------------------------------
DEFINE_FWK_MODULE(MTDSimHitAnalyzer);
