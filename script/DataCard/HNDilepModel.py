# Place it at CombineTool/CMSSW_10_2_13/python/HiggsAnalysis/CombinedLimit

from HiggsAnalysis.CombinedLimit.PhysicsModel import *
import re

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

class HNDilepModel_3Ch(PhysicsModel):
    """
    POIs:
      r = |VeN|^2 + |VmuN|^2
      f = |VeN|^2 / (|VeN|^2 + |VmuN|^2)

    Channel factors:
      EE   : f^2
      MuMu : (1-f)^2
      EMu  : f(1-f)

    Process dependence:
      DY/VBF : (|V_l1|^2 |V_l2|^2)/r  -> r * channel_factor
      SSWW   : |V_l1|^2 |V_l2|^2      -> r^2 * channel_factor

    One single template-scale parameter:
      preprocessing always scales:
        DY/VBF templates by r0 (= s)
        SSWW   templates by r0^2 (= s^2)
      This model divides by those factors so the fitted r,f are "physical".
    """

    def __init__(self):
        super().__init__()
        self.rRange = (0.0, 10.0)
        self.fRange = (0.0, 1.0)

        # single scale parameter (your preprocessing s)
        self.r0 = 1.0

        # bin-name patterns (override via --PO if needed)
        self.re_EE   = re.compile(r"(?:^|_|\b)EE(?:_|$|\b)")
        self.re_MuMu = re.compile(r"(?:^|_|\b)MuMu(?:_|$|\b)")
        self.re_EMu  = re.compile(r"(?:^|_|\b)EMu(?:_|$|\b)")

    def setPhysicsOptions(self, physOptions):
        for po in physOptions:
            if po.startswith("rRange="):
                lo, hi = po.replace("rRange=", "").split(",")
                self.rRange = (float(lo), float(hi))
            elif po.startswith("fRange="):
                lo, hi = po.replace("fRange=", "").split(",")
                self.fRange = (float(lo), float(hi))
            elif po.startswith("r0="):
                self.r0 = float(po.replace("r0=", ""))
            elif po.startswith("reEE="):
                self.re_EE = re.compile(po.replace("reEE=", ""))
            elif po.startswith("reMuMu="):
                self.re_MuMu = re.compile(po.replace("reMuMu=", ""))
            elif po.startswith("reEMu="):
                self.re_EMu = re.compile(po.replace("reEMu=", ""))

        if self.r0 == 0.0:
            raise RuntimeError("Invalid r0: must be non-zero.")

    def _channel_from_bin(self, binname: str) -> str:
        if self.re_EE.search(binname):
            return "EE"
        if self.re_MuMu.search(binname):
            return "MuMu"
        if self.re_EMu.search(binname):
            return "EMu"
        raise RuntimeError(
            f"Cannot infer channel from bin name '{binname}'. "
            "Include EE/MuMu/EMu in bin name, or pass --PO reEE=..., reMuMu=..., reEMu=..."
        )

    def doParametersOfInterest(self):
        rLo, rHi = self.rRange
        fLo, fHi = self.fRange

        self.modelBuilder.doVar(f"r[1.0,{rLo},{rHi}]")
        self.modelBuilder.doVar(f"f[0.5,{fLo},{fHi}]")
        self.modelBuilder.doSet("POI", "r,f")

        r0 = self.r0
        r0sq = r0 * r0

        # DY/VBF scales: (r * chanFactor) / r0
        self.modelBuilder.factory_(f"expr::scale_lin_EE('(@0*@1*@1)/{r0}', r, f)")
        self.modelBuilder.factory_(f"expr::scale_lin_MuMu('(@0*(1-@1)*(1-@1))/{r0}', r, f)")
        self.modelBuilder.factory_(f"expr::scale_lin_EMu('(@0*@1*(1-@1))/{r0}', r, f)")

        # SSWW scales: (r^2 * chanFactor) / r0^2
        self.modelBuilder.factory_(f"expr::scale_quad_EE('(@0*@0*@1*@1)/{r0sq}', r, f)")
        self.modelBuilder.factory_(f"expr::scale_quad_MuMu('(@0*@0*(1-@1)*(1-@1))/{r0sq}', r, f)")
        self.modelBuilder.factory_(f"expr::scale_quad_EMu('(@0*@0*@1*(1-@1))/{r0sq}', r, f)")

    def getYieldScale(self, bin, process):
        if process not in ["signalDY", "signalVBF", "signalSSWW"]:
            return 1

        ch = self._channel_from_bin(bin)

        if process in ["signalDY", "signalVBF"]:
            return f"scale_lin_{ch}"
        if process == "signalSSWW":
            return f"scale_quad_{ch}"

        return 1

hnDilepModel_3ch = HNDilepModel_3Ch()
hnDilepModel_EMu_Full = HNDilepModel_EMu_Full()
hnDilepModel_EMu = HNDilepModel_EMu()
hnDilepModel = HNDilepModel()
