import FWCore.ParameterSet.Config as cms

def customiseMTDFastSim(process):
    ################################################################################
    # Add mtdSimHits to interactionModels
    ################################################################################
    process.fastSimProducer.interactionModels.mtdSimHits = cms.PSet(
        className     = cms.string('fastsim::MTDSimHitProducer'),
        btlRadius     = cms.double(116.),
        btlHalfLength = cms.double(260.),
        etlRMin       = cms.double(31.),
        etlRMax       = cms.double(120.),
    )


    ################################################################################
    # Interaction models for BTL (LYSO: Lu2SiO5)
    ################################################################################
    process.fastSimProducer.interactionModels.pairProductionBTL = cms.PSet(
        className       = cms.string('fastsim::PairProduction'),
        Z               = cms.double(54.0),
        photonEnergyCut = cms.double(0.1),
    )

    process.fastSimProducer.interactionModels.nuclearInteractionBTL = cms.PSet(
        className     = cms.string('fastsim::NuclearInteraction'),
        distCut       = cms.double(0.02),
        hadronEnergy  = cms.double(0.2),
        saveOutput    = cms.untracked.bool(False),
    )

    process.fastSimProducer.interactionModels.bremsstrahlungBTL = cms.PSet(
        className               = cms.string('fastsim::Bremsstrahlung'),
        Z                       = cms.double(54.0),
        minPhotonEnergy         = cms.double(0.1),
        minPhotonEnergyFraction = cms.double(0.005),
    )

    process.fastSimProducer.interactionModels.energyLossBTL = cms.PSet(
        className      = cms.string('fastsim::EnergyLoss'),
        A              = cms.double(175.94),
        Z              = cms.double(54.0),
        density        = cms.double(7.4),
        radLen         = cms.double(1.14),
        minMomentumCut = cms.double(0.1),
    )

    process.fastSimProducer.interactionModels.multipleScatteringBTL = cms.PSet(
        className = cms.string('fastsim::MultipleScattering'),
        minPt     = cms.double(0.2),
        radLen    = cms.double(1.14),
    )


    ################################################################################
    # Interaction models for ETL (Silicon)
    ################################################################################
    process.fastSimProducer.interactionModels.pairProductionETL = cms.PSet(
        className       = cms.string('fastsim::PairProduction'),
        Z               = cms.double(14.0),
        photonEnergyCut = cms.double(0.1),
    )

    process.fastSimProducer.interactionModels.nuclearInteractionETL = cms.PSet(
        className    = cms.string('fastsim::NuclearInteraction'),
        distCut      = cms.double(0.02),
        hadronEnergy = cms.double(0.2),
        saveOutput   = cms.untracked.bool(False),
    )

    process.fastSimProducer.interactionModels.bremsstrahlungETL = cms.PSet(
        className               = cms.string('fastsim::Bremsstrahlung'),
        Z                       = cms.double(14.0),
        minPhotonEnergy         = cms.double(0.1),
        minPhotonEnergyFraction = cms.double(0.005),
    )

    process.fastSimProducer.interactionModels.energyLossETL = cms.PSet(
        className      = cms.string('fastsim::EnergyLoss'),
        A              = cms.double(28.0855),
        Z              = cms.double(14.0),
        density        = cms.double(2.329),
        radLen         = cms.double(9.36),
        minMomentumCut = cms.double(0.1),
    )

    process.fastSimProducer.interactionModels.multipleScatteringETL = cms.PSet(
        className = cms.string('fastsim::MultipleScattering'),
        minPt     = cms.double(0.2),
        radLen    = cms.double(9.36),
    )


    ################################################################################
    # Add BTL layer to trackerDefinition
    ################################################################################
    process.fastSimProducer.trackerDefinition.BarrelLayers.append(
        cms.PSet(
            radius = cms.untracked.double(121.),
            limits = cms.untracked.vdouble(0.0, 260.),
            thickness = cms.untracked.vdouble(3.29), # 3.75cm / X0(LYSO=1.14cm)
            interactionModels = cms.untracked.vstring(
                'pairProductionBTL',
                'nuclearInteractionBTL',
                'bremsstrahlungBTL',
                'energyLossBTL',
                'multipleScatteringBTL',
                'mtdSimHits',
            ),
        )
    )

#    btlLayer = cms.PSet(
#        radius = cms.untracked.double(116.),
#        limits = cms.untracked.vdouble(0.0, 260.),
#        thickness = cms.untracked.vdouble(0.1),
#        interactionModels = cms.untracked.vstring('mtdSimHits'),
#    )

#    oldLayers = process.fastSimProducer.trackerDefinition.BarrelLayers
#    newLayers = cms.VPSet()
#    inserted = False
#    for layer in oldLayers:
#        if not inserted and hasattr(layer, 'radius') \
#                and layer.radius.value() >= 120.0:
#            newLayers.append(btlLayer)
#            inserted = True
#        newLayers.append(layer)
#    if not inserted:
#        newLayers.append(btlLayer)
#    process.fastSimProducer.trackerDefinition.BarrelLayers = newLayers


    ################################################################################
    # Add ETL forward layer to trackerDefinition
    ################################################################################
#    process.fastSimProducer.trackerDefinition.EndcapLayers.append(
#        cms.PSet(
#           z = cms.untracked.double(301.),
#            limits = cms.untracked.vdouble(31., 120.),
#            thickness = cms.untracked.vdouble(0.032), # 300um / X0(Si=9.36cm)
#            interactionModels = cms.untracked.vstring(
#                'pairProductionETL',
#                'nuclearInteractionETL',
#                'bremsstrahlungETL',
#                'energyLossETL',
#                'multipleScatteringETL',
#                'mtdSimHits'
#            ),
#        )
#    )

    etlLayer = cms.PSet(
        z = cms.untracked.double(301.),
        limits = cms.untracked.vdouble(31., 120.),
        thickness = cms.untracked.vdouble(0.0032), # 300um / X0(Si=9.36cm)
        interactionModels = cms.untracked.vstring(
            'pairProductionETL',
            'nuclearInteractionETL',
            'bremsstrahlungETL',
            'energyLossETL',
            'multipleScatteringETL',
            'mtdSimHits'),
    )
   
    oldEndcapLayers = process.fastSimProducer.trackerDefinition.EndcapLayers
    newEndcapLayers = cms.VPSet()
    inserted = False
    for layer in oldEndcapLayers:
        if not inserted and hasattr(layer, 'z') and layer.z.value() >= 301.:
            newEndcapLayers.append(etlLayer)
            inserted = True
        newEndcapLayers.append(layer)
    if not inserted:
        newEndcapLayers.append(etlLayer)
    process.fastSimProducer.trackerDefinition.EndcapLayers = newEndcapLayers


    ################################################################################
    # 4. Add hit collection to output
    ################################################################################
    if hasattr(process, 'RECOSIMoutput'):
        process.RECOSIMoutput.outputCommands += [
            'keep PSimHits_fastSimProducer_FastSimMTDBarrel_*',
            'keep PSimHits_fastSimProducer_FastSimMTDEndcap_*',
        ]

    return process
