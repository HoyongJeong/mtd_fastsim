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

    # Add analyzer
    from FastSimulation.MTD.mtdSimHitAnalyzer_cfi import mtdSimHitAnalyzer
    process.mtdSimHitAnalyzer = mtdSimHitAnalyzer

    if hasattr(process, 'simulation_step'):
        process.simulation_step += process.mtdSimHitProducer

    # Update output commands
    output_modules = [process.outputModules_()[out] for out in process.outputModules_()]
    for out_mod in output_modules:
        if hasattr(out_mod, 'outputCommands'):
            # Plz save my custom products...
            out_mod.outputCommands.append('keep *_mtdSimHitProducer_*_*')

    return process
