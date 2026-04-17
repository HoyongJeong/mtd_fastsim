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
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/InputTag.h"


//----------------------------------------------------------
// MTD geometry
//----------------------------------------------------------
#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"
#include "Geometry/Records/interface/MTDDigiGeometryRecord.h"


//----------------------------------------------------------
// Manetic field
// Tracks are not linear due to magnet!
//----------------------------------------------------------
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"


//----------------------------------------------------------
// Propagator will calculate path in the magnetic field
//----------------------------------------------------------
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"


//----------------------------------------------------------
// Trajectory state
//----------------------------------------------------------
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"


//----------------------------------------------------------
// SimTrack / SimVertex
//----------------------------------------------------------
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"


//----------------------------------------------------------
// SimHit data format
//----------------------------------------------------------
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"


//----------------------------------------------------------
// Standard
//----------------------------------------------------------
#include <vector>
#include <memory>
#include <string>



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
	//----------------------------------------------------------
	// EventSetup tokens
	// ESGetToken: handler that retreives gemotry from EventSetup
	// In global::EDProducer, it should be registered as consumes<> in constructor
	//----------------------------------------------------------
	const edm::ESGetToken<MTDGeometry  , MTDDigiGeometryRecord   > mtdGeoToken_;
	const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magFieldToken_;
	const edm::ESGetToken<Propagator   , TrackingComponentsRecord> propagatorToken_;


	//----------------------------------------------------------
	// Event token
	//----------------------------------------------------------
	const edm::EDGetTokenT<edm::SimTrackContainer>  simTrackToken_;
	const edm::EDGetTokenT<edm::SimVertexContainer> simVertexToken_;


	//----------------------------------------------------------
	// Method to generate free trajectory state
	//----------------------------------------------------------
	FreeTrajectoryState makeFTS(const SimTrack&, const SimVertex&, const MagneticField*) const;


	//----------------------------------------------------------
	// Method to generate a hit 
	//----------------------------------------------------------
	PSimHit makeSimHit(const TrajectoryStateOnSurface& tsos,
	                   const GeomDet*                  det,
	                   const SimTrack&                 track,
	                   float                           tof) const;
};



#endif
