import matplotlib
matplotlib.use("Agg")

import numpy as np
import matplotlib.pyplot as plt

masses = np.array([200, 250, 300, 400, 500], dtype=float)
limits = np.array([0.106, 0.194, 0.300, 0.713, 1.379], dtype=float)

# "a+b"는 합으로 처리
xsecs = np.array([
    3.57786272755e-05,
    1.49950878991e-05,
    7.43681976965e-06 + 6.510161879333267e-07,
    2.45309287603e-06 + 4.2478900486666243e-07,
    1.01479289272e-06 + 2.91733616626e-07
], dtype=float)

inv_xsecs = 1.0 / xsecs

def linfit(x, y):
    (a, b), cov = np.polyfit(x, y, deg=1, cov=True)  # y = a x + b
    yhat = a*x + b
    ss_res = np.sum((y - yhat)**2)
    ss_tot = np.sum((y - np.mean(y))**2)
    r2 = 1 - ss_res/ss_tot if ss_tot > 0 else np.nan
    a_err, b_err = np.sqrt(np.diag(cov))
    return a, b, a_err, b_err, r2, yhat

# fits
aL, bL, aL_err, bL_err, r2L, lim_hat = linfit(masses, limits)
aI, bI, aI_err, bI_err, r2I, inv_hat = linfit(masses, inv_xsecs)

print("limit(m) = a*m + b")
print(f"  a = {aL:.6g} ± {aL_err:.2g}")
print(f"  b = {bL:.6g} ± {bL_err:.2g}")
print(f"  R^2 = {r2L:.6g}\n")

print("1/xsec(m) = a*m + b")
print(f"  a = {aI:.6g} ± {aI_err:.2g}")
print(f"  b = {bI:.6g} ± {bI_err:.2g}")
print(f"  R^2 = {r2I:.6g}\n")

# plots
fig, axes = plt.subplots(1, 2, figsize=(11, 4.2))

ax = axes[0]
ax.scatter(masses, limits, label="limit (points)")
ax.plot(masses, lim_hat, label="linear fit")
ax.set_xlabel("mass")
ax.set_ylabel("limit")
ax.set_title(f"limit vs mass (R²={r2L:.3f})")
ax.grid(True, alpha=0.3)
ax.legend()

ax = axes[1]
ax.scatter(masses, inv_xsecs, label="1/xsec (points)")
ax.plot(masses, inv_hat, label="linear fit")
ax.set_xlabel("mass")
ax.set_ylabel("1/xsec")
ax.set_title(f"1/xsec vs mass (R²={r2I:.3f})")
#ax.set_yscale("log")  # 보기 좋게 (피팅은 선형 y로 그대로)
ax.grid(True, which="both", alpha=0.3)
ax.legend()

plt.tight_layout()
plt.savefig("limit_fit.png", dpi=200, bbox_inches="tight")
plt.savefig("limit_fit.pdf", bbox_inches="tight")
print("Saved: limit_fit.png, limit_fit.pdf")
