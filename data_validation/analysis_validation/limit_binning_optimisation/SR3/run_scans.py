#!/usr/bin/env python3
import subprocess
import time
import shlex

# === CONFIG ===
SETUP = "/data6/Users/jalmond/2020/HL_SKFlatAnalyzer_UL_LONG/SKFlatAnalyzer/setup.sh"
MAX_PARALLEL = 5
POLL_SECS = 5

# Masses, flavours, eras
MASSES   = [500, 600, 700, 800, 900, 1000]
FLAVOURS = ["EE", "MuMu", "EMu"]
ERAS     = ["2016preVFP", "2016postVFP", "2017", "2018"]

# === Build RAW_CMDS automatically ===
RAW_CMDS = []
for m in MASSES:
    for f in FLAVOURS:
        for e in ERAS:
            RAW_CMDS.append(
                f"scan_sr1_memory.py --mass {m} --flavour {f} --era {e} --n_bins 6 --mnonly"
            )
            

def wrap(cmd: str) -> str:
    """
    Run each command in a fresh bash that:
      - sources your setup with '-q' (so your setup early-returns)
      - unsets PYTHONHOME to avoid Python init_fs_encoding issues
      - calls python3 explicitly (don't rely on alias 'python')
    """
    parts = shlex.split(cmd)
    if parts[0] == "python":
        parts[0] = "python3"
    if parts[0].endswith(".py"):
        parts = ["python3"] + parts

    inner = " ".join(shlex.quote(x) for x in parts)
    return f"bash -lc 'source {shlex.quote(SETUP)} -q; env -u PYTHONHOME {inner}'"

def main():
    commands = [wrap(c) for c in RAW_CMDS]
    running = []

    while commands or running:
        while commands and len(running) < MAX_PARALLEL:
            cmd = commands.pop(0)
            print(f"[LAUNCH] {cmd}", flush=True)
            p = subprocess.Popen(cmd, shell=True, executable="/bin/bash")
            running.append((cmd, p))

        still = []
        for cmd, p in running:
            rc = p.poll()
            if rc is None:
                still.append((cmd, p))
            else:
                print(f"[DONE] {cmd} (exit {rc})", flush=True)
        running = still

        time.sleep(POLL_SECS)

if __name__ == "__main__":
    main()
