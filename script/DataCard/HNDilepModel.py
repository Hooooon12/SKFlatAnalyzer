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

class HNDilepModel_EMu_Full(PhysicsModel):
    """
    POIs:
      a = |VeN|^2, b = |VmN|^2.
      r : total mixing strength proxy (a+b)
      f : electron fraction a/(a+b)

    emu channel scaling (user's definition):
      signalDY, signalVBF :  ~ r * f * (1-f)
      signalSSWW          :  ~ r^2 * f * (1-f)

    Each process can have its own reference (r0_proc, f0_proc) used to build templates.
    """

    def __init__(self):
        super().__init__()
        # default reference point (can be overridden via --PO)
        self.rRange = (0.0, 10.0)
        self.fRange = (0.0, 1.0)
        self.fInit = 0.5
        self.r0    = 1.0

    def setPhysicsOptions(self, physOptions):
        for po in physOptions:
            if po.startswith("rRange="):
                lo, hi = po.replace("rRange=", "").split(",")
                self.rRange = (float(lo), float(hi))
            if po.startswith("fRange="):
                lo, hi = po.replace("fRange=", "").split(",")
                self.fRange = (float(lo), float(hi))
            if po.startswith("fInit="):
                self.fInit = float(po.replace("fInit=", ""))
            if po.startswith("r0="):
                self.r0 = float(po.replace("r0=", ""))

        if self.r0 == 0.0:
            raise RuntimeError("Invalid r0: must be non-zero.")

    def doParametersOfInterest(self):
        rLo, rHi = self.rRange
        fLo, fHi = self.fRange

        self.modelBuilder.doVar(f"r[1.0,{rLo},{rHi}]")
        self.modelBuilder.doVar(f"f[{self.fInit},{fLo},{fHi}]")

        self.modelBuilder.doSet("POI", "r,f")

        # DY & VBF : r * f(1-f)
        denomDY  = self.r0
        denomVBF = self.r0
        # SSWW : r^2 * f(1-f)
        denomSSWW  = self.r0 ** 2

        self.modelBuilder.factory_(f"expr::scale_signalDY('(@0*@1*(1-@1))/{denomDY}', r, f)")
        self.modelBuilder.factory_(f"expr::scale_signalVBF('(@0*@1*(1-@1))/{denomVBF}', r, f)")
        self.modelBuilder.factory_(f"expr::scale_signalSSWW('(@0*@0*@1*(1-@1))/{denomSSWW}', r, f)")

    def getYieldScale(self, bin, process):
        if process == "signalDY":
            return "scale_signalDY"
        if process == "signalVBF":
            return "scale_signalVBF"
        if process == "signalSSWW":
            return "scale_signalSSWW"
        return 1

hnDilepModel_EMu_Full = HNDilepModel_EMu_Full()
hnDilepModel_EMu = HNDilepModel_EMu()
hnDilepModel = HNDilepModel()
