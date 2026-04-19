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
    mode = hnl
      POIs:
        r = |VeN|^2 + |VmuN|^2
        f = |VeN|^2 / (|VeN|^2 + |VmuN|^2)

      Channel factors:
        EE   : f^2
        MuMu : (1-f)^2
        EMu  : f(1-f)

      Process dependence:
        DY/VBF/DYVBF : r   * channel_factor / r0
        SSWW         : r^2 * channel_factor / r0^2

    mode = weinberg
      POI:
        r = overall signal-strength for the reference sample
            (current convention: r = (200 TeV / Lambda)^2 )

      Fixed non-POI parameters:
        wEE, wEMu, wMuMu : C^2s

      Channel dependence:
        signalWeinberg(EE)   : r * wEE   / r0
        signalWeinberg(EMu)  : r * wEMu  / r0
        signalWeinberg(MuMu) : r * wMuMu / r0

      Important:
        wEE, wEMu, wMuMu are NOT POIs.
        They are theory-point-dependent fixed numbers set with --setParameters and then frozen with --freezeParameters.
    """

    def __init__(self):
        super().__init__()

        # POI ranges
        self.rRange = (0.0, 10.0)
        self.fRange = (0.0, 1.0)

        # single scale parameter
        self.r0 = 1.0

        # bin-name patterns (override via --PO if needed)
        self.re_EE   = re.compile(r"(?:^|_|\b)EE(?:_|$|\b)")
        self.re_MuMu = re.compile(r"(?:^|_|\b)MuMu(?:_|$|\b)")
        self.re_EMu  = re.compile(r"(?:^|_|\b)EMu(?:_|$|\b)")

    def setPhysicsOptions(self, physOptions):
        for po in physOptions:
            if po.startswith("mode="):
                self.mode = po.replace("mode=", "").strip().lower()
            elif po.startswith("rRange="):
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
            else:
                raise RuntimeError(f"Unknown physics option: {po}")

        if self.mode not in ("hnl", "weinberg"):
            raise RuntimeError("mode must be either 'hnl' or 'weinberg'")

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

    def _define_hnl_scalings(self):
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

    def _define_weinberg_scalings(self):
        # These are fixed per theory point at runtime:
        # --setParameters wEE=...,wEMu=...,wMuMu=...
        # --freezeParameters wEE,wEMu,wMuMu

        r0 = self.r0

        self.modelBuilder.doVar("wEE[1.0,0.0,1.0]")
        self.modelBuilder.doVar("wEMu[1.0,0.0,1.0]")
        self.modelBuilder.doVar("wMuMu[1.0,0.0,1.0]")

        # Weinberg is linear in r
        self.modelBuilder.factory_(f"expr::scale_w_EE('(@0*@1)/{r0}', r, wEE)")
        self.modelBuilder.factory_(f"expr::scale_w_EMu('(@0*@1)/{r0}', r, wEMu)")
        self.modelBuilder.factory_(f"expr::scale_w_MuMu('(@0*@1)/{r0}', r, wMuMu)")

    def doParametersOfInterest(self):
        rLo, rHi = self.rRange
        self.modelBuilder.doVar(f"r[1.0,{rLo},{rHi}]")

        if self.mode == "hnl":
            fLo, fHi = self.fRange
            self.modelBuilder.doVar(f"f[0.5,{fLo},{fHi}]")
            self._define_hnl_scalings()
            self.modelBuilder.doSet("POI", "r,f")

        elif self.mode == "weinberg":
            self._define_weinberg_scalings()
            self.modelBuilder.doSet("POI", "r")        

    def getYieldScale(self, bin, process):

        ch = self._channel_from_bin(bin)

        # ---------------------------
        # HNL mode
        # ---------------------------
        if self.mode == "hnl":
            if process in ["signalDY", "signalVBF", "signalDYVBF"]:
                return f"scale_lin_{ch}"

            if process in ["signalSSWW"]:
                return f"scale_quad_{ch}"

            if process in ["signalWeinberg"]:
                return 0

            return 1

        # ---------------------------
        # Weinberg mode
        # ---------------------------
        if self.mode == "weinberg":
            if process in ["signalWeinberg"]:
                return f"scale_w_{ch}"

            if process in ["signalDY", "signalVBF", "signalDYVBF", "signalSSWW"]:
                return 0

            return 1

        return 1

hnDilepModel_3ch = HNDilepModel_3Ch()
hnDilepModel_EMu_Full = HNDilepModel_EMu_Full()
hnDilepModel_EMu = HNDilepModel_EMu()
hnDilepModel = HNDilepModel()
