import FWCore.ParameterSet.Config as cms



mtdSimHitProducer = cms.EDProducer(
    "MTDSimHitProducer",
    simTrackLabel  = cms.InputTag("g4SimHits"),
    simVertexLabel = cms.InputTag("g4SimHits"),
    propagatorName = cms.string("PropagatorWithMaterial"),
)



def customiseMTD(process):
    # Add Propagator ESProducer (providing TrackingComponentsRecord)
    process.load("TrackingTools.MaterialEffects.MaterialPropagator_cfi")
    process.load("TrackingTools.MaterialEffects.OppositeMaterialPropagator_cfi")

    # Load this module
    process.load("FastSimulation.MTD.mtdSimHitProducer_cfi")

    if hasattr(process, 'simulation_step'):
        process.simulation_step += process.mtdSimHitProducer

    return process
