# PyROOT: SR 파일과 InvMET 파일이 서로 다른 경우, WW의 SR↔InvMET 전이 비율 R의 스프레드 계산
# - SR: sr_file 에서 읽음
# - InvMET: inv_file 에서 읽음
# - 히스토그램 네이밍은 설정 섹션에서 조절
# 사용 전 pip install uproot 대신 PyROOT 환경(ROOT)이 필요합니다.

import ROOT, math

# =========================
# 설정
# =========================
era = "2018"
proc   = "ww"                     # 대상 프로세스
sr_tag = "sr2"                    # 가독성용 태그(출력 이름용)
sr_file_path  = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/"+era+"/sr2/M1000_MuMu_card_input.root"        # SR ROOT 파일
inv_file_path = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/"+era+"/sr2_InvMET/M1000_MuMu_card_input.root" # InvMET ROOT 파일

# 히스토그램 네이밍 템플릿
#   {proc}: 프로세스 이름
#   {chan}: 채널(파일이 이미 채널을 의미하면 chan을 비워두는 템플릿을 선택)
#
# 케이스 A) 파일이 채널을 의미하고 히스토그램은 "ww", "ww__JESUp" 형태:
HIST_TPL_NOM  = "{proc}"
HIST_TPL_SYST = "{proc}_{syst}{var}"   # 예: ww__JESUp
#
# 케이스 B) 히스토그램에 채널명이 붙는 경우 (예: ww_sr1, ww_sr1__JESUp):
# HIST_TPL_NOM  = "{proc}_{chan}"
# HIST_TPL_SYST = "{proc}_{chan}__{syst}{var}"

# 고려할 nuisance 목록 (환경에 맞게 수정)
systs = [
"CMS_res_j_"+era,
"CMS_scale_j_"+era,
"CMS_eff_j_PUJET_id_"+era,
"CMS_eff_j_PNET_"+era,
"CMS_scale_m_"+era,
"CMS_res_m_"+era,
"CMS_eff_m_reco_"+era,
"CMS_eff_m_id_"+era,
"CMS_eff_m_trigger_"+era,
"CMS_scale_e_"+era,
"CMS_res_e_"+era,
"CMS_eff_e_reco_"+era,
"CMS_eff_e_id_"+era,
"CMS_eff_e_trigger_"+era,
"CMS_btag_hf_corr",
"CMS_btag_hf_uncorr_"+era,
"CMS_btag_lf_corr",
"CMS_btag_lf_uncorr_"+era,
"CMS_scale_met_"+era,
"CMS_l1_ecal_prefiring_"+era,
"CMS_pileup_13TeV",
"CMS_cf_stat_"+era,
"CMS_fake_stat_"+era,
"CMS_fake_highpt_"+era,
]

# 없거나 0/음수 적분을 만나면 건너뛸지(True) 아니면 에러낼지(False)
skip_missing_or_zero = True

# =========================
# 유틸
# =========================
def open_root(path):
    f = ROOT.TFile.Open(path)
    if not f or f.IsZombie():
        raise RuntimeError(f"Cannot open ROOT file: {path}")
    return f

def get_integral(rfile, hname):
    h = rfile.Get(hname)
    if not h:
        return None
    val = h.Integral()
    return val

def hist_name_nom(chan_or_empty, proc):
    return HIST_TPL_NOM.format(proc=proc, chan=chan_or_empty)

def hist_name_syst(chan_or_empty, proc, syst, var):
    return HIST_TPL_SYST.format(proc=proc, chan=chan_or_empty, syst=syst, var=var)

# =========================
# 본 계산
# =========================
f_sr  = open_root(sr_file_path)
f_inv = open_root(inv_file_path)

# 채널 문자열이 히스토그램 이름에 필요 없으면 ""로 전달
chan_for_names = ""  # 케이스 A
# chan_for_names = sr_tag  # 케이스 B 사용시 주석 해제

# Nominal
h_sr_nom  = hist_name_nom(chan_for_names, proc)
h_inv_nom = hist_name_nom(chan_for_names, proc)

N_sr_nom  = get_integral(f_sr,  h_sr_nom)
N_inv_nom = get_integral(f_inv, h_inv_nom)

if (N_sr_nom is None) or (N_inv_nom is None):
    raise RuntimeError(f"Missing nominal hist. SR:{h_sr_nom} or Inv:{h_inv_nom}")
if (not skip_missing_or_zero) and (N_sr_nom <= 0 or N_inv_nom <= 0):
    raise RuntimeError("Zero/negative nominal yield encountered.")
if N_sr_nom is None or N_sr_nom <= 0 or N_inv_nom is None or N_inv_nom <= 0:
    print("[WARN] Zero/neg nominal encountered; results may be meaningless.")

R_nom = (N_sr_nom / N_inv_nom) if (N_sr_nom and N_inv_nom and N_inv_nom != 0) else None
if not R_nom:
    raise RuntimeError("Invalid R_nom (check yields).")

rel_shifts = []  # (syst, var, ΔR/R)

for syst in systs:
    for var in ("Up","Down"):
        h_sr_var  = hist_name_syst(chan_for_names, proc, syst, var)
        h_inv_var = hist_name_syst(chan_for_names, proc, syst, var)

        N_sr_var  = get_integral(f_sr,  h_sr_var)
        N_inv_var = get_integral(f_inv, h_inv_var)

        if (N_sr_var is None) or (N_inv_var is None):
            if skip_missing_or_zero:
                continue
            else:
                raise RuntimeError(f"Missing var hist: {h_sr_var} or {h_inv_var}")

        if (N_sr_var <= 0 or N_inv_var <= 0):
            if skip_missing_or_zero:
                continue
            else:
                raise RuntimeError(f"Non-positive yield in variation: {h_sr_var} or {h_inv_var}")

        R_var = N_sr_var / N_inv_var
        rel   = (R_var / R_nom) - 1.0
        rel_shifts.append((syst, var, rel))

# =========================
# 요약/출력
# =========================
if not rel_shifts:
    print("No usable variations found. TF nuisance likely unnecessary (δ ~ 0%).")
else:
    envelope = max(abs(r) for _,_,r in rel_shifts)
    mean = sum(r for _,_,r in rel_shifts) / len(rel_shifts)
    rms  = math.sqrt(sum((r-mean)**2 for _,_,r in rel_shifts) / len(rel_shifts))

    print(f"[{sr_tag}] R_nom = N_SR/N_Inv = {R_nom:.6f}")
    print("Top contributors to |ΔR/R|:")
    for syst,var,rel in sorted(rel_shifts, key=lambda x: -abs(x[2]))[:8]:
        print(f"  {syst}{var:>4}: ΔR/R = {rel:+.3%}")

    print(f"\nEnvelope(|ΔR/R|) = {envelope:.2%}")
    print(f"RMS(ΔR/R)        = {rms:.2%}")

    # Datacard 제안 (ratio-only TF를 lnN 비대칭으로 정확히 ±envelope 반영)
    up   = 1.0 / (1.0 - envelope)  # e.g. envelope=0.10 -> up=1.111...
    down = 1.0 / (1.0 + envelope)  # -> down=0.909...
    print("\n==> Datacard lnN (asymmetric) suggestion for InvMET/ww:")
    print(f"TF_WW_{sr_tag}Inv  lnN   ...  (SR/ww)=1.00  ...  (InvMET/ww)={up:.3f}/{down:.3f}  ...")

# 파일 닫기(선택)
f_sr.Close()
f_inv.Close()

