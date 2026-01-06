# Place it at CombineTool/CMSSW_10_2_13/python/HiggsAnalysis/CombinedLimit

from HiggsAnalysis.CombinedLimit.PhysicsModel import *

class HNDilepModel(PhysicsModel):
    def doParametersOfInterest(self):
        self.modelBuilder.doVar("r[0,0,10]")
        self.modelBuilder.doSet("POI", ",".join(['r']))
        self.modelBuilder.factory_("expr::r2(\"(@0)*(@0)\", r)")

    def getYieldScale(self, bin, process):
        if "signalDYVBF" in process or "signalDY" in process or "signalVBF" in process or "signalWeinberg" in process:
            return 'r'
        elif "signalSSWW" in process:
            return 'r2'
        else:
            return 1

class HNDilepModel_EMu(PhysicsModel):
    def doParametersOfInterest(self):
        self.modelBuilder.doVar("r[0,0,10]")
        self.modelBuilder.doSet("POI", ",".join(['r']))
        self.modelBuilder.factory_("expr::r2_EMu(\"4*(@0)*(@0)\", r)")

    def getYieldScale(self, bin, process):
        if "signalDYVBF" in process or "signalDY" in process or "signalVBF" in process or "signalWeinberg" in process:
            return 'r'
        elif "signalSSWW" in process:
            return 'r2_EMu'
        else:
            return 1

hnDilepModel_EMu = HNDilepModel_EMu()
hnDilepModel = HNDilepModel()
