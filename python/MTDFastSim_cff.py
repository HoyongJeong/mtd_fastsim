import FWCore.ParameterSet.Config as cms

def customiseMTDFastSim(process):
    # 1. Add mtdSimHits to interactionModels
    process.fastSimProducer.interactionModels.mtdSimHits = cms.PSet(
        className     = cms.string('fastsim::MTDSimHitProducer'),
        btlRadius     = cms.double(116.),
        btlHalfLength = cms.double(260.),
        etlRMin       = cms.double(31.),
        etlRMax       = cms.double(120.),
    )

    # 2. Add BTL layer to trackerDefinition
    process.fastSimProducer.trackerDefinition.BarrelLayers.append(
        cms.PSet(
            radius = cms.untracked.double(116.),
            limits = cms.untracked.vdouble(0.0, 260.),
            thickness = cms.untracked.vdouble(0.0),
            interactionModels = cms.untracked.vstring('mtdSimHits'),
        )
    )

    # 3. Add ETL (+z, -z) forward layers to trackerDefinition
    process.fastSimProducer.trackerDefinition.EndcapLayers.append(
        cms.PSet(
            z = cms.untracked.double(290.),
            limits = cms.untracked.vdouble(31., 120.),
            thickness = cms.untracked.vdouble(0.0),
            interactionModels = cms.untracked.vstring('mtdSimHits'),
        )
    )
    process.fastSimProducer.trackerDefinition.EndcapLayers.append(
        cms.PSet(
            z = cms.untracked.double(-290.),
            limits = cms.untracked.vdouble(31., 120.),
            thickness = cms.untracked.vdouble(0.0),
            interactionModels = cms.untracked.vstring('mtdSimHits'),
        )
    )

    # 4. Add hit collection to output
    process.RECOSIMoutput.outputCommands += [
        'keep PSimHits_fastSimProducer_FastSimMTDBarrel_*',
        'keep PSimHits_fastSimProducer_FastSimMTDEndcap_*',
    ]

    return process
