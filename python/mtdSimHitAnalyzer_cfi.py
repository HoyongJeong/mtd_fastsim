import FWCore.ParameterSet.Config as cms

mtdSimHitAnalyzer = cms.EDAnalyzer(
    "MTDSimHitAnalyzer",
    btlHitsLabel = cms.InputTag("mtdSimHitProducer", "FastTimerHitsBarrelByFastsim"),
    etlHitsLabel = cms.InputTag("mtdSimHitProducer", "FastTimerHitsEndcapByFastsim"),
)
