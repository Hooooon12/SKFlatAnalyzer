import matplotlib
matplotlib.use("Agg")  # pyplot import 전에!

import numpy as np
import matplotlib.pyplot as plt

# --- 입력 ---
m = np.array([200, 250, 300, 400, 500], dtype=float)
L = np.array([0.106, 0.194, 0.300, 0.713, 1.379], dtype=float)

# xsec: "a+b"는 합으로 처리
sigma = np.array([
    3.57786272755e-05,
    1.49950878991e-05,
    7.43681976965e-06 + 6.510161879333267e-07,
    2.45309287603e-06 + 4.2478900486666243e-07,
    1.01479289272e-06 + 2.91733616626e-07
], dtype=float)

inv = 1.0 / sigma
prod = L * sigma  # 역비례면 거의 상수여야 함

# --- 1) 인접 비율 & 전체 비율 ---
rat_L   = L[1:] / L[:-1]
rat_inv = inv[1:] / inv[:-1]
ror = rat_L / rat_inv  # 1이면 "같은 배수로" 변하는 것

print("Adjacent ratios (L and 1/sigma):")
for i in range(len(m)-1):
    print(f"  {int(m[i])}->{int(m[i+1])}:  L x{rat_L[i]:.3f},  (1/sigma) x{rat_inv[i]:.3f},  ratio-of-ratios={ror[i]:.3f}")

print(f"\nEndpoint ratios 200->500:  L x{(L[-1]/L[0]):.3f},  (1/sigma) x{(inv[-1]/inv[0]):.3f}")

# --- 2) shape 비교: 정규화해서 겹치기 ---
ref = 0  # 200 GeV를 기준 (원하면 2로 바꿔서 300 기준 등)
Ln = L / L[ref]
In = inv / inv[ref]

# --- 3) 역비례 검증: L*sigma가 상수인지 ---
print("\nL*sigma by mass (should be ~const if L ∝ 1/sigma):")
for mi, pi in zip(m, prod):
    print(f"  {int(mi)}: {pi:.6e}")
print(f"  (200)/(500) = {prod[0]/prod[-1]:.3f}")

# --- (옵션) L vs inv 직접 관계: best scale k로 얼마나 맞는지 ---
# L ≈ k * inv (원점 통과 비례)
k = (inv @ L) / (inv @ inv)
L_pred = k * inv
ss_res = np.sum((L - L_pred)**2)
ss_tot = np.sum((L - np.mean(L))**2)
r2 = 1 - ss_res/ss_tot
print(f"\nProportional fit: L ≈ k*(1/sigma): k={k:.6e}, R^2={r2:.4f}")

# --- Plot A: normalized curves overlay ---
plt.figure(figsize=(6.2,4.2))
plt.plot(m, Ln, marker="o", label="L / L(ref)")
plt.plot(m, In, marker="o", label="(1/sigma) / (1/sigma)(ref)")
plt.xlabel("mass")
plt.ylabel("normalized value")
plt.title(f"Shape comparison (ref mass = {int(m[ref])})")
plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig("shape_overlay.png", dpi=200)

# --- Plot B: product vs mass ---
plt.figure(figsize=(6.2,4.2))
plt.plot(m, prod, marker="o")
plt.xlabel("mass")
plt.ylabel("L * sigma")
plt.title("Check inverse proportionality: L*sigma vs mass")
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("L_times_sigma.png", dpi=200)

# --- Plot C: L vs (1/sigma) scatter + proportional line ---
plt.figure(figsize=(6.2,4.2))
plt.scatter(inv, L, label="points")
xline = np.linspace(inv.min()*0.9, inv.max()*1.1, 100)
plt.plot(xline, k*xline, label=f"L = k*(1/sigma), R²={r2:.3f}")
plt.xlabel("1/sigma")
plt.ylabel("L")
plt.title("Direct relation: does L track 1/sigma?")
plt.grid(True, alpha=0.3)
plt.legend()
plt.tight_layout()
plt.savefig("L_vs_invxsec.png", dpi=200)

print("\nSaved: shape_overlay.png, L_times_sigma.png, L_vs_invxsec.png")

