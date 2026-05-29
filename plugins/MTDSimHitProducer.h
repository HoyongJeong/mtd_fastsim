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
//----------------------------------------------------------
// Core
//----------------------------------------------------------
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"


//----------------------------------------------------------
// data formats
//----------------------------------------------------------
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"



///-----------------------------------------------------------------------------
/// Class declaration
///-----------------------------------------------------------------------------
class MTDSimHitProducer : public edm::global::EDProducer<>
{
	public:
	//----------------------------------------------------------
	// Constructor and destructor
	//----------------------------------------------------------
	explicit MTDSimHitProducer(const edm::ParameterSet& cfg);
	~MTDSimHitProducer() override = default;


	//----------------------------------------------------------
	// Public method
	//----------------------------------------------------------
	static void fillDescriptions(edm::ConfigurationDescriptions&);
	void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;


	private:
	//----------------------------------------------------------
	// Geometry parameters
	//----------------------------------------------------------
	const double btlRadius_;
	const double btlHalfLength_;
	const double etlZ_;
	const double etlRMin_;
	const double etlRMax_;


	//----------------------------------------------------------
	// Input tokens
	//----------------------------------------------------------
	const edm::EDGetTokenT<edm::SimTrackContainer>  simTrackToken_;
	const edm::EDGetTokenT<edm::SimVertexContainer> simVertexToken_;
};



#endif
