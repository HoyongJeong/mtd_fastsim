import FWCore.ParameterSet.Config as cms

mtdSimHitProducer = cms.EDProducer("MTDSimHitProducer",
    btlRadius     = cms.double(116.),
    btlHalfLength = cms.double(260.),
    etlZ          = cms.double(290.),
    etlRMin       = cms.double(31.),
    etlRMax       = cms.double(120.),
    simTracks     = cms.InputTag("fastSimProducer"),
    simVertices   = cms.InputTag("fastSimProducer"),
)

def customiseMTDFastSim(process):
    process.mtdSimHitProducer = mtdSimHitProducer
    process.simulation_step += process.mtdSimHitProducer

    # Add MTD SimHit collection to RECOSIMoutput
    process.RECOSIMoutput.outputCommands += [
        'keep PSimHits_mtdSimHitProducer_FastSimMTDBarrel_*',
        'keep PSimHits_mtdSimHitProducer_FastSimMTDEndcap_*',
    ]

    return process
