import FWCore.ParameterSet.Config as cms

mtdSimHitProducer = cms.EDProducer("MTDSimHitProducer")

def customiseMTD(process):
    process.load("FastSimulation.MTD.mtdSimHitProducer_cfi")
    if hasattr(process, 'simulation_step'):
        process.simulation_step += process.mtdSimHitProducer
    return process
