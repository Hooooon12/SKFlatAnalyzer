#!/usr/bin/env python3

# Asymptotic expected:
# python create-batch.py -l [RunList*.txt] --Asymptotic
#
# Asymptotic observed:
# python create-batch.py -l [RunList*.txt] --Asymptotic --Unblind
#
# CLs expected:
# python create-batch.py -l [RunList*.txt] --Q3 -t 1000 -n 100 --rRange 0.1:5.0:0.5
#
# CLs observed:
# python create-batch.py -l [RunList*.txt] --Unblind -t 1000 -n 100 --rRange 0.1:5.0:0.5

# RunList*.txt is created by MakeRunList.py .

import os, sys
import subprocess as cmd
import argparse
import math
import numpy as np
import random
import re

parser = argparse.ArgumentParser(description='option')
parser.add_argument('--pdf', action='store_true', help='do pdfseparate; run this after getting all impacts')
parser.add_argument('-i', dest='Input', help='take a single argument. [NOTE] feed realpath of a card (or workspace) !!')
parser.add_argument('-l', dest='RunLists', nargs='+', help='take args as a list, return error when there is no arg')
parser.add_argument('--Full', action='store_true') # NOTE combine -M HybridNew --LHCmode LHC-limits --expectedFromGrid only isn't full blinded. This is justified only when the data_obs itself is the Asimov set (which was indeed the case before we unblid the data. After unblinding, the script has been updated to generate prefit toys.)
parser.add_argument('--Q1', action='store_true')
parser.add_argument('--Q2', action='store_true')
parser.add_argument('--Q3', action='store_true')
parser.add_argument('--Q4', action='store_true')
parser.add_argument('--Q5', action='store_true')
# Parallelization & Grid Options for CLs
parser.add_argument('-t', dest='Ntoy', default='1000', help='Total N of toys for pseudo-data')
parser.add_argument('-n', dest='Split', default=1, type=int, help='Number of parallel jobs to split the toys into')
parser.add_argument(
    '--rRange',
    dest='rRange',
    help=(
        'For CLs: min:max:step, e.g. 0.1:5.0:0.2. '
        'For Impact: halfWidth or min:max[:step], e.g. 200 or -200:200. '
        'Impact high-mass uses this range; low-mass uses 1/10 of it.'
    ),
    default=None
)
parser.add_argument('--Diagnostic', action='store_true', help='Save toys and verbosity for debugging')
parser.add_argument('--tag', dest='UserTag', default="", help='Unique tag to prevent overwrite DAGMan (e.g. v2, run2)')
parser.add_argument('--seedBase', dest='SeedBase', default=None, type=int,
                    help='Base seed for GOF split toys. If omitted, a random base seed is generated.')
# Other works
parser.add_argument('--Asymptotic', action='store_true')
parser.add_argument('--Work', action='store_true', help='create workspace')
parser.add_argument('--FitDiag', action='store_true', help='check nuisance fit')
parser.add_argument('--Impact', action='store_true', help='check impacts')
parser.add_argument('--FastScan', action='store_true', help='fast scan')
parser.add_argument('--MDfit', action='store_true', help='multidimension fits')
parser.add_argument('--r', default='0', help='(3ch, EMuFull only) inject r')
parser.add_argument('--f', default='0.5', help='(3ch, EMuFull for HNL only) inject f')
parser.add_argument('--wMuMu', default='1', help='(3ch, for Weinberg only) inject wMuMu')
parser.add_argument('--wEE',   default='0', help='(3ch, for Weinberg only) inject wEE')
parser.add_argument('--wEMu',  default='0', help='(3ch, for Weinberg only) inject wEMu')
parser.add_argument('--Breakdown', action='store_true', help='uncertainty breakdown')
parser.add_argument('--GOF', action='store_true', help='goodness of fit test')
parser.add_argument(
    '--mask',
    dest='Masks',
    nargs='+',
    default=[],
    metavar='CHANNEL',
    help=(
        'Mask one or more analysis channels. '
        'Examples: --mask zz_cr, --mask zz_cr wz_cr1, '
        '--mask year18_zz_cr. '
        'For Run2, an unqualified channel such as zz_cr is expanded '
        'to all eras. For Run2Sum, it remains mask_zz_cr.'
    ),
)
parser.add_argument('--InjectSignal', default='0', help='inject signals to asimov')
parser.add_argument('--Unblind', action='store_true', help='unblind the data')
# GOF toy-generation diagnostic options
parser.add_argument('--bypassFrequentistFit', action='store_true',
                    help='For GOF toys: add --bypassFrequentistFit to toy commands')
parser.add_argument('--toysNoSystematics', action='store_true',
                    help='For GOF toys: add --toysNoSystematics to toy commands')
args = parser.parse_args()

if args.toysNoSystematics and args.bypassFrequentistFit:
  print("[ERROR] --toysNoSystematics and --bypassFrequentistFit should not be used together.")
  print("        --toysNoSystematics disables nuisance randomization in toy generation,")
  print("        while --bypassFrequentistFit is only meaningful for --toysFrequentist toys.")
  print("        Please choose only one diagnostic mode.")
  sys.exit(1)

IsNuis = False # Limit extraction setting
Ncheck = 0
for check in ['FitDiag','Impact','Breakdown','FastScan','MDfit','GOF']:
  if vars(args)[check] is True:
    this_check = check
    IsNuis = True # Statistical tests
    Ncheck += 1
if Ncheck > 1:
  print("More than 1 Nuisance flag activated; This is not supported.")
  print("Exiting ...")
  sys.exit(1)

if args.Masks and args.Work:
  print("[ERROR] --mask does not modify workspace creation.")
  print("        Create the workspace normally with --Work.")
  print("        Apply --mask only when running a statistical method.")
  sys.exit(1)

if args.Masks and args.FastScan:
  print("[ERROR] --mask is not implemented for --FastScan.")
  sys.exit(1)

if args.Masks and args.Breakdown:
  print("[ERROR] --mask is not implemented for --Breakdown.")
  sys.exit(1)

AsimovSetting = "-t -1 --expectSignal="+args.InjectSignal
AsimovName = "s"+args.InjectSignal
RunBlind = "--run blind"
LimitModeLabel = "Expected"

if args.Unblind:
  AsimovSetting = ""
  AsimovName = "Unblind"
  RunBlind = ""
  LimitModeLabel = "Observed"

CLsFlagRequested = args.Full or args.Q1 or args.Q2 or args.Q3 or args.Q4 or args.Q5

if args.Asymptotic and CLsFlagRequested:
  print("[INFO] --Asymptotic requested together with --Full/--Q*: CLs flags will be ignored; only Asymptotic jobs will be created.")

pwd = os.getcwd()
CMSSW_BASE = os.environ['CMSSW_BASE']

failure, result = cmd.getstatusoutput('combine --help')
if failure:
  print("[!!ERROR!!] cannot run combine.")
  print("Please set proper cmsenv first.")
  print("Exiting ...")
  sys.exit(1)

if args.Input is not None:
  args.RunLists = []
  args.RunLists.append(args.Input)

# --- Helper Functions ---

def parse_r_range(r_str):
    """
    Accepts:
      - "" / None -> []
      - "min:max" -> [min, max]
      - "min:max:step" -> list of grid points from min to max (inclusive-ish)
    """
    if not r_str:
        return []

    parts = r_str.split(':')
    try:
        if len(parts) == 2:
            start, end = map(float, parts)
            return [round(start, 4), round(end, 4)]

        if len(parts) == 3:
            start, end, step = map(float, parts)
            if step == 0:
                raise ValueError("step must be non-zero")

            # Create grid points (including end point if close)
            points = np.arange(start, end + step/1000.0, step).tolist()
            return [round(p, 4) for p in points]

        raise ValueError("expected 2 or 3 fields")
    except Exception:
        print(f"[ERROR] rRange format '{r_str}' is invalid. Use min:max or min:max:step")
        sys.exit(1)

def parse_impact_r_bounds(r_str, scale=1.0):
    """
    Impact r range helper.

    Defaults:
      high mass: scale=1.0 -> [-100, 100]
      low mass : scale=0.1 -> [-10, 10]

    Accepted inputs:
      None          -> default
      "200"         -> [-200, 200]
      "-200:200"    -> [-200, 200]
      "-200:200:10" -> [-200, 200], step ignored for Impact
    """
    default_half_width = 100.0

    if not r_str:
        r_min, r_max = -default_half_width, default_half_width
    else:
        parts = r_str.split(':')
        try:
            if len(parts) == 1:
                half_width = abs(float(parts[0]))
                r_min, r_max = -half_width, half_width
            elif len(parts) in (2, 3):
                r_min, r_max = float(parts[0]), float(parts[1])
            else:
                raise ValueError("expected halfWidth or min:max[:step]")
        except Exception:
            print(f"[ERROR] Impact rRange format '{r_str}' is invalid.")
            print("        Use halfWidth, e.g. 200, or min:max[:step], e.g. -200:200")
            sys.exit(1)

    return r_min * scale, r_max * scale

def expected_from_grid_flag(quant_val):
    """
    Expected HybridNew limit uses --expectedFromGrid.
    Observed HybridNew limit must not use this option.
    """
    return "" if quant_val is None else f"--expectedFromGrid {quant_val} "

def hybrid_quant_suffix(quant_val):
    """
    Combine output filename contains .quantX only for expectedFromGrid outputs.
    Observed HybridNew output has no .quantX suffix.
    """
    return "" if quant_val is None else f".quant{quant_val}"

def write_prefit_asimov_and_get_data_opt(
    runfile,
    card,
    tag,
    mask_options="",
):
    """
    Make a pre-fit b-only Asimov toy, then return the -D option that tells
    Combine to use that toy instead of data_obs.

    Apply the same channel masks used in the subsequent HybridNew command.
    """
    mask_opt = f" {mask_options}" if mask_options else ""

    runfile.write(
        f"combine -M GenerateOnly {card} "
        f"-t -1 --expectSignal 0 --saveToys "
        f"-n {tag}"
        f"{mask_opt}\n"
    )

    return (
        f"-D higgsCombine{tag}.GenerateOnly.mH120.123456.root:"
        f"toys/toy_asimov "
    )

# ----------------------------------------------------------------------
# GOF helper functions
# ----------------------------------------------------------------------

MASKABLE_CHANNELS = [
    "sr1",
    "sr2",
    "sr3",
    "cr1_InvMET",
    "cr2_InvMET",
    "cr3_InvMET",
    "cr1_InvBJet",
    "cr2_InvBJet",
    "cr3_InvBJet",
    "wz_cr1",
    "wz_cr2",
    "wz_cr3",
    "zg_cr",
    "zz_cr",
]

RUN2_ERA_PREFIXES = {
    "2016preVFP": "year16a",
    "2016postVFP": "year16b",
    "2017": "year17",
    "2018": "year18",
}


def unique_preserve_order(items):
    """
    Remove duplicates without changing the original ordering.
    """
    return list(dict.fromkeys(items))


def fail_mask(message):
    print(f"[ERROR] {message}")
    print("")
    print("Valid base channel names are:")
    print("  " + ", ".join(MASKABLE_CHANNELS))
    print("")
    print("Examples:")
    print("  --mask zz_cr")
    print("  --mask zz_cr wz_cr1")
    print("  --mask year18_zz_cr")
    sys.exit(1)


def infer_mask_layout(shortcard):
    """
    Determine how channel-mask parameter names are stored in the workspace.

    Run2Sum:
      mask_sr1, mask_zz_cr, ...

    Run2:
      mask_year16a_sr1, ..., mask_year18_zz_cr

    Single-era card:
      mask_sr1, mask_zz_cr, ...
    """
    if shortcard.startswith("Run2Sum_"):
        return "Run2Sum", None

    if shortcard.startswith("Run2_"):
        return "Run2", None

    for era in RUN2_ERA_PREFIXES:
        if shortcard.startswith(f"{era}_"):
            return "SingleEra", era

    fail_mask(
        f"Cannot infer the mask layout from card name '{shortcard}'. "
        "Expected a name beginning with Run2Sum_, Run2_, "
        "2016preVFP_, 2016postVFP_, 2017_, or 2018_."
    )


def parse_mask_selector(selector):
    """
    Accepted forms:

      zz_cr
      mask_zz_cr
      year18_zz_cr
      mask_year18_zz_cr

    Returns:
      era=None or a canonical era name
      channel=base channel name
    """
    raw = str(selector).strip()

    if not raw:
        fail_mask("An empty channel name was passed to --mask.")

    if raw.startswith("mask_"):
        raw = raw[len("mask_"):]

    era = None
    channel = raw

    # Exact Run2 workspace channel syntax: year18_zz_cr
    for this_era, era_prefix in RUN2_ERA_PREFIXES.items():
        prefix = f"{era_prefix}_"

        if raw.startswith(prefix):
            era = this_era
            channel = raw[len(prefix):]
            break

    if channel not in MASKABLE_CHANNELS:
        fail_mask(
            f"Unknown channel '{channel}' obtained from --mask {selector}."
        )

    return era, channel


def resolve_mask_parameters(mask_requests, shortcard):
    """
    Convert user-facing --mask arguments into actual workspace parameters.

    Examples:

      Run2Sum + zz_cr
        -> mask_zz_cr

      Run2 + zz_cr
        -> mask_year16a_zz_cr
           mask_year16b_zz_cr
           mask_year17_zz_cr
           mask_year18_zz_cr

      single-era 2018 card + zz_cr
        -> mask_zz_cr
    """
    if not mask_requests:
        return []

    layout, single_era = infer_mask_layout(shortcard)
    resolved = []

    for request in mask_requests:
        requested_era, channel = parse_mask_selector(request)

        if layout == "Run2Sum":
            if requested_era is not None:
                fail_mask(
                    f"Era-specific mask '{request}' cannot be used with "
                    f"Run2Sum card '{shortcard}'. Run2Sum has no separate "
                    "year16a/year16b/year17/year18 channels."
                )

            resolved.append(f"mask_{channel}")

        elif layout == "Run2":
            if requested_era is None:
                eras_to_mask = list(RUN2_ERA_PREFIXES.keys())
            else:
                eras_to_mask = [requested_era]

            for era in eras_to_mask:
                era_prefix = RUN2_ERA_PREFIXES[era]
                resolved.append(f"mask_{era_prefix}_{channel}")

        elif layout == "SingleEra":
            if requested_era is not None and requested_era != single_era:
                fail_mask(
                    f"Mask '{request}' selects era {requested_era}, but "
                    f"card '{shortcard}' is a {single_era} card."
                )

            # A single-era workspace does not have year18_ etc. prefixes.
            resolved.append(f"mask_{channel}")

    return unique_preserve_order(resolved)

def sanitize_gof_tag(tag):
    """
    Keep output/job names safe for shell, ROOT filenames, and DAG job names.
    """
    if tag is None:
        return ""
    return re.sub(r"[^A-Za-z0-9_.-]+", "_", str(tag)).strip("_")

def make_mask_request_tag(mask_requests):
    """
    Build a compact directory/file tag from the user-facing mask requests.

    Examples:
      []                         -> ""
      ["zz_cr"]                  -> "mask-zz_cr"
      ["2018:zz_cr", "sr2"]      -> "mask-2018_zz_cr-sr2"
    """
    if not mask_requests:
        return ""

    clean_requests = [
        sanitize_gof_tag(request)
        for request in mask_requests
    ]
    clean_requests = [x for x in clean_requests if x]

    if not clean_requests:
        return ""

    return "mask-" + "-".join(clean_requests)


def make_effective_user_tag(args):
    """
    Combine the manually supplied --tag and the automatically generated
    mask tag.

    Examples:
      no tag, no mask
        -> ""

      --tag v1
        -> "v1"

      --mask zz_cr
        -> "mask-zz_cr"

      --tag v1 --mask zz_cr
        -> "v1_mask-zz_cr"
    """
    pieces = []

    user_tag = sanitize_gof_tag(args.UserTag)
    if user_tag:
        pieces.append(user_tag)

    mask_tag = make_mask_request_tag(args.Masks)
    if mask_tag:
        pieces.append(mask_tag)

    return "_".join(pieces)

def make_gof_variant_suffix(args):
    """
    Suffix for GOF diagnostic outputs.

    Default:
      ""

    Examples:
      --bypassFrequentistFit        -> "_bypass"
      --toysNoSystematics           -> "_noSyst"
      --bypassFrequentistFit --tag v1 -> "_bypass_v1"
    """
    pieces = []

    if args.bypassFrequentistFit:
        pieces.append("bypass")

    if args.toysNoSystematics:
        pieces.append("noSyst")

    effective_tag = make_effective_user_tag(args)
    if effective_tag:
        pieces.append(effective_tag)

    if not pieces:
        return ""

    return "_" + "_".join(pieces)

def make_gof_toy_mode_options(args):
    """
    Return the toy-generation mode for GOF toy commands.

    Default:
      --toysFrequentist

    Diagnostics:
      --bypassFrequentistFit
        -> --toysFrequentist --bypassFrequentistFit

      --toysNoSystematics
        -> --toysNoSystematics
           Do NOT also add --toysFrequentist.
    """
    if args.toysNoSystematics:
        return "--toysNoSystematics"

    opts = ["--toysFrequentist"]

    if args.bypassFrequentistFit:
        opts.append("--bypassFrequentistFit")

    return " ".join(opts)

def make_mask_set_freeze_options(
    mask_parameters=None,
    extra_set_parameters=None,
    extra_freeze_parameters=None,
):
    """
    Build Combine --setParameters and --freezeParameters options.

    mask_parameters must contain actual workspace parameter names such as:
      mask_zz_cr
      mask_year18_zz_cr

    extra_set_parameters is a list of (name, value), for example:
      [("r", 0)]

    extra_freeze_parameters is a list of parameter names, for example:
      ["r"]
    """
    mask_parameters = unique_preserve_order(mask_parameters or [])
    extra_set_parameters = extra_set_parameters or []
    extra_freeze_parameters = extra_freeze_parameters or []

    assignments = [
        f"{mask_parameter}=1"
        for mask_parameter in mask_parameters
    ]

    assignments.extend(
        f"{name}={value}"
        for name, value in extra_set_parameters
    )

    frozen_parameters = unique_preserve_order(
        mask_parameters + list(extra_freeze_parameters)
    )

    options = []

    if assignments:
        options.append(
            "--setParameters " + ",".join(assignments)
        )

    if frozen_parameters:
        options.append(
            "--freezeParameters " + ",".join(frozen_parameters)
        )

    return " ".join(options)


def make_gof_bonly_set_freeze_options(mask_parameters=None):
    """
    b-only GOF:
      - set requested channel masks to 1 and freeze them
      - set r=0 and freeze r
    """
    return make_mask_set_freeze_options(
        mask_parameters=mask_parameters,
        extra_set_parameters=[("r", 0)],
        extra_freeze_parameters=["r"],
    )


def make_gof_cronly_set_freeze_options(mask_parameters):
    """
    CR-only GOF:
      - mask the supplied channel list
      - set r=0 and freeze r

    The caller is responsible for supplying the automatically generated
    SR mask list plus any additional user-requested masks.
    """
    return make_mask_set_freeze_options(
        mask_parameters=mask_parameters,
        extra_set_parameters=[("r", 0)],
        extra_freeze_parameters=["r"],
    )

def write_gof_env_setup(runfile, cmssw_base):
    """
    Setup CMSSW inside a Condor scratch directory.

    Important:
      Do NOT cd to the shared GOF directory here.
      The job should run in worker scratch, create outputs there,
      then let Condor transfer outputs back.
    """
    runfile.write("#!/bin/bash\n")
    runfile.write("set -e\n")
    runfile.write("ulimit -s unlimited\n")
    runfile.write("STARTDIR=$PWD\n")
    runfile.write(f"pushd {cmssw_base}/src\n")
    runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
    runfile.write("eval `scramv1 runtime -sh`\n")
    runfile.write("popd\n")
    runfile.write("cd ${STARTDIR}\n")
    runfile.write("\n")

def write_gof_submit_file(base_dir, job_name, transfer_inputs=None, transfer_outputs=None,
                          memory=8000, cpus=1):
    """
    Condor submit file for GOF DAG nodes.

    This uses Condor file transfer:
      - combine writes ROOT files in worker scratch
      - Condor transfers only declared outputs back to base_dir
      - merge job can receive obs/toy files through transfer_input_files
    """
    transfer_inputs = transfer_inputs or []
    transfer_outputs = transfer_outputs or []

    with open(f"{base_dir}/submit_{job_name}.sub", "w") as subfile:
        subfile.write("universe = vanilla\n")
        subfile.write("getenv = True\n")
        subfile.write('+SingularityImage = "/cvmfs/singularity.opensciencegrid.org/opensciencegrid/osgvo-el9:latest"\n')
        subfile.write("should_transfer_files = YES\n")
        subfile.write("when_to_transfer_output = ON_EXIT\n")
        subfile.write(f"request_memory = {memory}\n")
        subfile.write(f"request_cpus = {cpus}\n")
        subfile.write(f"executable = run_{job_name}.sh\n")
        subfile.write(f"log = logs/{job_name}.log\n")
        subfile.write(f"output = logs/{job_name}.out\n")
        subfile.write(f"error = logs/{job_name}.err\n")

        if transfer_inputs:
            subfile.write("transfer_input_files = " + ",".join(transfer_inputs) + "\n")

        if transfer_outputs:
            subfile.write("transfer_output_files = " + ",".join(transfer_outputs) + "\n")

        subfile.write("queue\n")

def create_hybrid_grid_dag(
    WP,
    shortcard,
    card,
    quantile,
    quant_val,
    total_toys,
    n_split,
    r_points,
    pwd,
    is_diagnostic,
    user_tag,
    is_unblind,
    mask_options="",
    user_tag_path="",
):
    """
    Creates a DAG that:
    1. Runs N jobs in parallel. Each job iterates over the full r_grid but runs (Total/N) toys.
    2. Merges the outputs (Hadd).
    3. Reads the result to extract limits.
    """
    base_dir = (
        f"Batch/{WP}/full_CLs/"
        f"{shortcard}{user_tag_path}"
    )

    mask_opt = f"{mask_options} " if mask_options else ""

    r_tag = f"{r_points[0]}_{r_points[-1]}"
    full_tag = f"{r_tag}_{user_tag}" if user_tag else r_tag

    dag_filename = f'{shortcard}_{quantile}_{full_tag}.dag'
    toys_per_job = math.ceil(int(total_toys) / n_split)
    
    # Diagnostic flags: Save everything if requested
    extra_flags = "--saveToys --saveHybridResult -v 1" if is_diagnostic else ""
    expected_flag = expected_from_grid_flag(quant_val)
    quant_suffix = hybrid_quant_suffix(quant_val)

    dag_content = ""
    grid_output_files = []

    # 1. Parallel Jobs (Split by Toys)
    for k in range(n_split):
        job_name = f"{quantile}_part{k}_{full_tag}"
        part_suffix = f"{shortcard}_{quantile}_part{k}_{full_tag}"
        
        # Script generation: Loop over r_points INSIDE the shell script
        # This avoids creating thousands of condor jobs for each point.
        with open(f"{base_dir}/run_{job_name}.sh", 'w') as runfile:
            runfile.write("#!/bin/bash\n")

            # Setup CMSSW Environment
            runfile.write(f"pushd {CMSSW_BASE}/src\n")
            runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
            runfile.write("eval `scramv1 runtime -sh`\n")
            runfile.write("popd\n") # Return to working directory

            runfile.write("ulimit -s unlimited\n")
            data_opt = ""
            if not is_unblind:
                asimov_tag = f"_{part_suffix}_prefitAsimov"
                data_opt = write_prefit_asimov_and_get_data_opt(
                    runfile=runfile,
                    card=card,
                    tag=asimov_tag,
                    mask_options=mask_options,
                )
            
            # Loop over r values
            for r in r_points:
                # Generate a random seed here in Python
                # This allows us to predict the output filename exactly.
                this_seed = random.randint(100000, 999999)
                
                # Command explanation:
                # --singlePoint {r}: Fix r, don't auto-search.
                # --clsAcc 0: Calculate CLs exactly (no approximations).
                # -s {this_seed}: We enforce the seed.
                cmd = (
                    f"combine -M HybridNew "
                    f"--LHCmode LHC-limits "
                    f"{card} "
                    f"{data_opt}"
                    f"{mask_opt}"
                    f"-n {part_suffix}_r{r} "
                    f"{expected_flag}"
                    f"-T {toys_per_job} "
                    f"--singlePoint {r} "
                    f"--clsAcc 0 "
                    f"-s {this_seed} "
                    f"{extra_flags}"
                )
                
                runfile.write(f"echo 'Running Point r={r} with seed {this_seed}'\n")
                runfile.write(f"{cmd}\n")
                
                # Construct the exact filename that combine will produce with this seed
                # Format: higgsCombine{Name}.HybridNew.mH120.{Seed}.quant{Quant}.root
                expected_file = f"higgsCombine{part_suffix}_r{r}.HybridNew.mH120.{this_seed}{quant_suffix}.root"
                grid_output_files.append(f"{pwd}/{base_dir}/{expected_file}") # Add to global merge list
            
            runfile.write("echo 'All points done for this chunk.'\n")

        with open(f"{base_dir}/submit_{job_name}.sub", 'w') as subfile:
            subfile.write("universe = vanilla\n")
            subfile.write("getenv = True\n")
            subfile.write("request_memory = 4000\n")
            subfile.write("request_cpus = 1\n")
            subfile.write(f"executable = run_{job_name}.sh\n")
            subfile.write(f"log = logs/{job_name}.log\n")
            subfile.write(f"output = logs/{job_name}.out\n")
            subfile.write(f"error = logs/{job_name}.out\n")
            subfile.write("should_transfer_files = YES\n")
            subfile.write("when_to_transfer_output = ON_EXIT\n")
            subfile.write("queue\n")

        dag_content += f"JOB {job_name} submit_{job_name}.sub\n"
        
    # 2. Merge Job (Hadd)
    # We produce ONE big merged file containing all toys for all r points
    merged_filename = f"higgsCombine{shortcard}_{quantile}_merged_{full_tag}.HybridNew.mH120{quant_suffix}.root"
    merge_job_name = f"{quantile}_merge_{full_tag}"
    
    with open(f"{base_dir}/run_{merge_job_name}.sh", 'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write(f"pushd {CMSSW_BASE}/src\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("eval `scramv1 runtime -sh`\n")
        runfile.write("popd\n")
        # Hadd allows merging files with same tree structures (HybridResult)
        # Note: Argument list might be too long for bash, so we use xargs or a response file if needed.
        # For ~10 splits * ~20 points = 200 files, it's usually fine.
        files_str = " ".join(grid_output_files)
        runfile.write(f"hadd -f {merged_filename} {files_str}\n")

    with open(f"{base_dir}/submit_{merge_job_name}.sub", 'w') as subfile:
        subfile.write("universe = vanilla\n")
        subfile.write("getenv = True\n")
        subfile.write(f"executable = run_{merge_job_name}.sh\n")
        subfile.write(f"log = logs/{merge_job_name}.log\n")
        subfile.write(f"output = logs/{merge_job_name}.out\n")
        subfile.write(f"error = logs/{merge_job_name}.out\n")
        subfile.write("should_transfer_files = YES\n")
        subfile.write("when_to_transfer_output = ON_EXIT\n")
        subfile.write("queue\n")

    dag_content += f"JOB {merge_job_name} submit_{merge_job_name}.sub\n"
    parent_line = "PARENT " + " ".join([f"{quantile}_part{k}_{full_tag}" for k in range(n_split)])
    dag_content += f"{parent_line} CHILD {merge_job_name}\n"

    # scripts to merge multiple runs' outputs

    final_merged_filename = f"higgsCombine{shortcard}_{quantile}_merged_final.HybridNew.mH120{quant_suffix}.root"
    final_merge_job_name = f"{quantile}_merge_final"
    
    with open(f"{base_dir}/run_{final_merge_job_name}.sh", 'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write(f"pushd {CMSSW_BASE}/src\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("eval `scramv1 runtime -sh`\n")
        runfile.write("popd\n")
        runfile.write(
            f"hadd -f {final_merged_filename} "
            f"$(find {pwd}/{base_dir} "
            f"-maxdepth 1 "
            f"-type f "
            f"-name '*merged*.root' "
            f"! -name '*merged_final*' "
            f"-exec realpath {{}} +)\n"
        )

    with open(f"{base_dir}/submit_{final_merge_job_name}.sub", 'w') as subfile:
        subfile.write("universe = vanilla\n")
        subfile.write("getenv = True\n")
        subfile.write(f"executable = run_{final_merge_job_name}.sh\n")
        subfile.write(f"log = logs/{final_merge_job_name}.log\n")
        subfile.write(f"output = logs/{final_merge_job_name}.out\n")
        subfile.write(f"error = logs/{final_merge_job_name}.out\n")
        subfile.write("should_transfer_files = YES\n")
        subfile.write("when_to_transfer_output = ON_EXIT\n")
        subfile.write(f"batch_name = {shortcard}_{WP}_{quantile}_merge_final\n")
        subfile.write("queue\n")

    # 3. Read/Limit Job
    read_job_name = f"{quantile}_read_{full_tag}"
    this_output = f"{shortcard}_{quantile}_{full_tag}.root"
    plot_output_name = f"limit_scan_{shortcard}_{quantile}_{full_tag}"
    
    with open(f"{base_dir}/run_{read_job_name}.sh", 'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write(f"pushd {CMSSW_BASE}/src\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("eval `scramv1 runtime -sh`\n")
        runfile.write("popd\n")
        # --readHybridResults: reads the merged grid
        # --grid: input file
        # It will interpolate between the r points we scanned.
        data_opt = ""
        if not is_unblind:
            asimov_tag = (
                f"_{shortcard}_{quantile}_"
                f"{full_tag}_prefitAsimov_read"
            )
            data_opt = write_prefit_asimov_and_get_data_opt(
                runfile=runfile,
                card=card,
                tag=asimov_tag,
                mask_options=mask_options,
            )

        runfile.write(
            f"combine -M HybridNew "
            f"--LHCmode LHC-limits "
            f"{card} "
            f"{data_opt}"
            f"{mask_opt}"
            f"-n {shortcard}_{full_tag} "
            f"--readHybridResults "
            f"--grid={pwd}/{base_dir}/{merged_filename} "
            f"{expected_flag}"
            f"--plot={plot_output_name}.png\n"
        )
        # Optional: Plotting command hint (can't run easily without X11, but script is ready)
        # runfile.write(f"plotLimitGrid.py {this_output} ... \n")
        runfile.write("echo 'Limit calculation and plotting done.'\n")

    with open(f"{base_dir}/submit_{read_job_name}.sub", 'w') as subfile:
        subfile.write("universe = vanilla\n")
        subfile.write("getenv = True\n")
        subfile.write(f"executable = run_{read_job_name}.sh\n")
        subfile.write(f"log = logs/{read_job_name}.log\n")
        subfile.write(f"output = logs/{read_job_name}.out\n")
        subfile.write(f"error = logs/{read_job_name}.out\n")
        subfile.write("should_transfer_files = YES\n")
        subfile.write("when_to_transfer_output = ON_EXIT\n")

        expected_this = f"higgsCombine{shortcard}_{full_tag}.HybridNew.mH120{quant_suffix}.root"
        expected_plot = f"{plot_output_name}.png"

        # Transfer both ROOT file and PNG
        subfile.write(f"transfer_output_files = {expected_this},{expected_plot}\n")
        subfile.write(f"transfer_output_remaps = \"{expected_this} = output/{this_output}; {expected_plot} = output/{expected_plot}\"\n")
        subfile.write("queue\n")

    dag_content += f"JOB {read_job_name} submit_{read_job_name}.sub\n"
    dag_content += f"PARENT {merge_job_name} CHILD {read_job_name}\n"

    # scripts to run CLs using final merged grids

    final_read_job_name = f"{quantile}_read_final"
    final_output = f"{shortcard}_{quantile}_final.root"
    final_plot_output_name = f"limit_scan_{shortcard}_{quantile}_final"
    
    with open(f"{base_dir}/run_{final_read_job_name}.sh", 'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write(f"pushd {CMSSW_BASE}/src\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("eval `scramv1 runtime -sh`\n")
        runfile.write("popd\n")
        data_opt = ""
        if not is_unblind:
            asimov_tag = (
                f"_{shortcard}_{quantile}_"
                f"final_prefitAsimov_read"
            )
            data_opt = write_prefit_asimov_and_get_data_opt(
                runfile=runfile,
                card=card,
                tag=asimov_tag,
                mask_options=mask_options,
            )

        runfile.write(
            f"combine -M HybridNew "
            f"--LHCmode LHC-limits "
            f"{card} "
            f"{data_opt}"
            f"{mask_opt}"
            f"-n {shortcard}_final "
            f"--readHybridResults "
            f"--grid={pwd}/{base_dir}/{merged_filename} "
            f"{expected_flag}"
            f"--plot={final_plot_output_name}.png\n"
        )
        runfile.write("echo 'Limit calculation and plotting done.'\n")

    with open(f"{base_dir}/submit_{final_read_job_name}.sub", 'w') as subfile:
        subfile.write("universe = vanilla\n")
        subfile.write("getenv = True\n")
        subfile.write(f"executable = run_{final_read_job_name}.sh\n")
        subfile.write(f"log = logs/{final_read_job_name}.log\n")
        subfile.write(f"output = logs/{final_read_job_name}.out\n")
        subfile.write(f"error = logs/{final_read_job_name}.out\n")
        subfile.write("should_transfer_files = YES\n")
        subfile.write("when_to_transfer_output = ON_EXIT\n")
        subfile.write(f"batch_name = {shortcard}_{WP}_{quantile}_read_final\n")

        expected_this = f"higgsCombine{shortcard}_final.HybridNew.mH120{quant_suffix}.root"
        expected_plot = f"{final_plot_output_name}.png"

        # Transfer both ROOT file and PNG
        subfile.write(f"transfer_output_files = {expected_this},{expected_plot}\n")
        subfile.write(f"transfer_output_remaps = \"{expected_this} = output/{final_output}; {expected_plot} = output/{expected_plot}\"\n")
        subfile.write("queue\n")

    with open(f"{base_dir}/{dag_filename}", 'w') as f:
        f.write(dag_content)

    return dag_filename

def create_gof_split_dag(
    WP,
    shortcard,
    this_shortcard,
    card,
    total_toys,
    n_split,
    pwd,
    cmssw_base,
    asimov_name,
    is_unblind,
    toy_mode_opts="--toysFrequentist",
    variant_suffix="",
    seed_base=None,
    requested_mask_parameters=None,
    cronly_mask_parameters=None,
    user_tag_path="",
):
    """
    Split GOF toys over Condor DAG jobs with Condor file transfer.

    This version is designed to reduce shared-directory IO:
      - obs/toy jobs run in worker scratch
      - obs/toy ROOT files are transferred back by Condor
      - merge job receives obs/toy ROOT files through transfer_input_files
      - hadd/CollectGoodnessOfFit/plotGof run in merge worker scratch
      - final outputs are transferred back

    Modes:
      is_unblind=True:
        b-only GOF using real data:
          obs  = gof_bonly_obs
          toys = gof_bonly_toys

      is_unblind=False:
        CR-only GOF using real data in CRs, with SR masks frozen:
          obs  = gof_CRonly_obs
          toys = gof_CRonly_toysFreq

        Also preserves the previous b-only Asimov GOF command:
          gof_Asimov

    Produced in:
      <WP>/<shortcard>/GOF/<asimov_name>/
    """

    requested_mask_parameters = unique_preserve_order(
        requested_mask_parameters or []
    )
    cronly_mask_parameters = unique_preserve_order(
        cronly_mask_parameters or requested_mask_parameters
    )

    base_dir = (
        f"{WP}/{shortcard}/GOF/"
        f"{asimov_name}{user_tag_path}"
    )
    logs_dir = f"{base_dir}/logs"
    os.makedirs(logs_dir, exist_ok=True)

    total_toys = int(total_toys)
    n_split = int(n_split)

    if n_split < 1:
        print("[ERROR] GOF split must be >= 1")
        sys.exit(1)

    if total_toys < 1:
        print("[ERROR] GOF total toys must be >= 1")
        sys.exit(1)

    # Exact split:
    #   1000 toys / 20 jobs = [50, 50, ..., 50]
    # If not divisible, first 'rem' jobs get one extra toy.
    q, rem = divmod(total_toys, n_split)
    toy_counts = [q + (1 if k < rem else 0) for k in range(n_split)]
    toy_counts = [x for x in toy_counts if x > 0]
    n_actual = len(toy_counts)

    # Unique seeds for this GOF split run.
    #
    # If --seedBase is given:
    #   seeds = seedBase+1, seedBase+2, ...
    #
    # If --seedBase is omitted:
    #   choose a random base seed using OS entropy.
    #
    # Keep seeds below 2^31-1 to avoid any possible integer-boundary issues.
    max_seed = 2147483000

    if seed_base is None:
        rng = random.SystemRandom()
        base_seed = rng.randint(1000000, max_seed - n_actual - 1)
    else:
        base_seed = int(seed_base)

    if base_seed < 1 or base_seed + n_actual >= max_seed:
        print(f"[ERROR] Invalid GOF seed base: {base_seed}")
        print(f"        Need 1 <= seedBase and seedBase + nSplit < {max_seed}")
        sys.exit(1)

    seeds = [base_seed + 1 + k for k in range(n_actual)]

    rootfile = f"{pwd}/{WP}/{shortcard}/{this_shortcard}.root"

    mode_tag = "bonly" if is_unblind else "CRonly"

    print(f"[GOF seed] {this_shortcard} {asimov_name} {mode_tag}{variant_suffix}:")
    print(f"           base_seed = {base_seed}")
    print(f"           seeds     = {seeds[0]} ... {seeds[-1]}  ({n_actual} jobs)")

    dag_tag = f"{mode_tag}{variant_suffix}"

    dag_name = f"{this_shortcard}_GOF_{dag_tag}_Ntoy{total_toys}_Split{n_actual}.dag"
    dag_path = f"{base_dir}/{dag_name}"

    if is_unblind:
        set_freeze_opts = make_gof_bonly_set_freeze_options(
            requested_mask_parameters
        )
        obs_name_suffix = f"_gof_bonly_obs_{this_shortcard}{variant_suffix}"
        toy_name_core = "gof_bonly_toys"
        json_base = f"gof_{this_shortcard}{variant_suffix}"
        title_right = this_shortcard
    else:
        set_freeze_opts = make_gof_cronly_set_freeze_options(
            cronly_mask_parameters
        )
        obs_name_suffix = f"_gof_CRonly_obs_{this_shortcard}{variant_suffix}"
        toy_name_core = "gof_CRonly_toysFreq"
        json_base = f"gof_{this_shortcard}_CRonly{variant_suffix}"
        title_right = f"CR-only {this_shortcard}"

    dag_lines = []

    # ------------------------------------------------------------------
    # 0) Optional b-only Asimov GOF job for non-unblind / CR-only mode.
    #    This preserves the old non-Unblind behavior.
    # ------------------------------------------------------------------
    asimov_job = None

    if not is_unblind:
        asimov_job = f"gof_{dag_tag}_asimov"
        asimov_seed = 123456
        asimov_name_suffix = f"_gof_Asimov_{this_shortcard}{variant_suffix}"
        asimov_file = (
            f"higgsCombine{asimov_name_suffix}.GoodnessOfFit.mH120.{asimov_seed}.root"
        )

        with open(f"{base_dir}/run_{asimov_job}.sh", "w") as runfile:
            write_gof_env_setup(runfile, cmssw_base)
            runfile.write("echo '[GOF] Running b-only Asimov saturated GOF'\n")
            runfile.write(
                f"combine -M GoodnessOfFit {rootfile} "
                f"-t -1 -s {asimov_seed} "
                f"{make_gof_bonly_set_freeze_options(requested_mask_parameters)} "
                f"--algo saturated "
                f"-n {asimov_name_suffix}\n"
            )

        os.system(f"chmod +x {base_dir}/run_{asimov_job}.sh")
        write_gof_submit_file(
            base_dir=base_dir,
            job_name=asimov_job,
            transfer_outputs=[asimov_file],
        )
        dag_lines.append(f"JOB {asimov_job} submit_{asimov_job}.sub\n")

    # ------------------------------------------------------------------
    # 1) Observed GOF job
    # ------------------------------------------------------------------
    obs_job = f"gof_{dag_tag}_obs"
    obs_file = f"higgsCombine{obs_name_suffix}.GoodnessOfFit.mH120.root"

    with open(f"{base_dir}/run_{obs_job}.sh", "w") as runfile:
        write_gof_env_setup(runfile, cmssw_base)
        runfile.write(f"echo '[GOF] Running observed {mode_tag} saturated GOF'\n")
        runfile.write(
            f"combine -M GoodnessOfFit {rootfile} "
            f"{set_freeze_opts} "
            f"--algo saturated "
            f"-n {obs_name_suffix}\n"
        )

    os.system(f"chmod +x {base_dir}/run_{obs_job}.sh")
    write_gof_submit_file(
        base_dir=base_dir,
        job_name=obs_job,
        transfer_outputs=[obs_file],
    )
    dag_lines.append(f"JOB {obs_job} submit_{obs_job}.sub\n")

    # ------------------------------------------------------------------
    # 2) Toy jobs
    # ------------------------------------------------------------------
    toy_output_files = []
    toy_jobs = []

    for k, ntoy in enumerate(toy_counts):
        seed = seeds[k]
        job = f"gof_{dag_tag}_toy_part{k}"
        toy_jobs.append(job)

        name_suffix = f"_{toy_name_core}_part{k}_Ntoy{ntoy}_{this_shortcard}{variant_suffix}"
        expected_root = f"higgsCombine{name_suffix}.GoodnessOfFit.mH120.{seed}.root"
        toy_output_files.append(expected_root)

        with open(f"{base_dir}/run_{job}.sh", "w") as runfile:
            write_gof_env_setup(runfile, cmssw_base)
            runfile.write(
                f"echo '[GOF] Running {mode_tag} toy part {k}: ntoy={ntoy}, seed={seed}'\n"
            )
            runfile.write(
                f"combine -M GoodnessOfFit {rootfile} "
                f"-t {ntoy} {toy_mode_opts} "
                f"{set_freeze_opts} "
                f"--algo saturated "
                f"-s {seed} "
                f"-n {name_suffix}\n"
            )

        os.system(f"chmod +x {base_dir}/run_{job}.sh")
        write_gof_submit_file(
            base_dir=base_dir,
            job_name=job,
            transfer_outputs=[expected_root],
        )
        dag_lines.append(f"JOB {job} submit_{job}.sub\n")

    # ------------------------------------------------------------------
    # 3) Merge + Collect + Plot job
    #
    #    Important:
    #      obs_file and toy_output_files are transferred into the merge
    #      worker scratch through transfer_input_files.
    #      hadd / Collect / plot run in scratch.
    #      only final outputs are transferred back.
    # ------------------------------------------------------------------
    merge_job = f"gof_{dag_tag}_merge_collect_plot"

    merged_toy_file = (
        f"higgsCombine_{toy_name_core}_Ntoy{total_toys}_{this_shortcard}"
        f"{variant_suffix}.GoodnessOfFit.mH120.merged.root"
    )

    json_file = f"{json_base}.json"
    plot_base = f"{json_base}_plot"
    plot_png = f"{plot_base}.png"
    plot_pdf = f"{plot_base}.pdf"

    merge_inputs = [obs_file] + toy_output_files
    merge_outputs = [merged_toy_file, json_file, plot_png, plot_pdf]

    with open(f"{base_dir}/run_{merge_job}.sh", "w") as runfile:
        write_gof_env_setup(runfile, cmssw_base)

        runfile.write("echo '[GOF] Checking transferred input files'\n")
        for f in merge_inputs:
            runfile.write(f"test -f {f}\n")

        runfile.write("\n")
        runfile.write("echo '[GOF] Merging toy ROOT files in worker scratch'\n")
        runfile.write(f"hadd -f {merged_toy_file} " + " ".join(toy_output_files) + "\n")

        runfile.write("\n")
        runfile.write("echo '[GOF] Collecting GOF result'\n")
        runfile.write(
            f"combineTool.py -M CollectGoodnessOfFit "
            f"--input {obs_file} {merged_toy_file} "
            f"-o {json_file}\n"
        )

        runfile.write("\n")
        runfile.write("echo '[GOF] Plotting GOF result'\n")
        runfile.write(
            f"plotGof.py {json_file} "
            f"--statistic saturated --mass 120.0 "
            f"-o {plot_base} "
            f"--title-right=\"{title_right}\"\n"
        )

        runfile.write("\n")
        runfile.write("echo '[GOF] Final files in worker scratch:'\n")
        runfile.write(f"ls -lh {merged_toy_file} {json_file} {plot_base}* || true\n")
        runfile.write("echo '[GOF] Done.'\n")

    os.system(f"chmod +x {base_dir}/run_{merge_job}.sh")
    write_gof_submit_file(
        base_dir=base_dir,
        job_name=merge_job,
        transfer_inputs=merge_inputs,
        transfer_outputs=merge_outputs,
    )
    dag_lines.append(f"JOB {merge_job} submit_{merge_job}.sub\n")

    parent_jobs = []
    if asimov_job is not None:
        parent_jobs.append(asimov_job)

    parent_jobs.append(obs_job)
    parent_jobs.extend(toy_jobs)

    dag_lines.append(f"PARENT {' '.join(parent_jobs)} CHILD {merge_job}\n")

    with open(dag_path, "w") as dagfile:
        dagfile.writelines(dag_lines)

    return dag_name

def fmt_w_label(x): # formatting with label
    """
    0.25 -> 0p25
    1.0  -> 1p0
    """
    return str(x).replace(".", "p").replace("-", "m")

def make_weinberg_w_points(step=0.25):
    """
    Simplex scan:
      wMuMu + wEMu + wEE = 1

    step=0.25 gives 15 points.
    step=0.20 gives 21 points.
    step=0.10 gives 66 points.
    """
    n = int(round(1.0 / step))
    points = []

    for i in range(n + 1):
        for j in range(n + 1 - i):
            k = n - i - j

            wMuMu = round(i * step, 10)
            wEMu = round(j * step, 10)
            wEE = round(k * step, 10)

            label = (
                "wMuMu" + fmt_w_label(wMuMu)
                + "_wEMu" + fmt_w_label(wEMu)
                + "_wEE" + fmt_w_label(wEE)
            )

            points.append((wMuMu, wEMu, wEE, label))

    return points

def make_hnl_f_points(shortcard):
    """
    f scan points for 3ch HNL Asymptotic limits.
    This keeps the MuMu/EE/EMu/envelope choices in one place.
    """
    all_points = [round(0.05 * i, 2) for i in range(21)]  # 0.0, 0.05, ..., 1.0

    if "MuMu" in shortcard:
        return all_points[:-1]   # 0.0 to 0.95
    if "EE" in shortcard:
        return all_points[1:]    # 0.05 to 1.0
    if "EMu" in shortcard:
        return all_points[1:-1]  # 0.05 to 0.95

    return all_points            # actual 3ch combined limit: 0.0 to 1.0


def write_condor_queue_values(submitfile, var_name, values):
    """
    Write a compact HTCondor queue line from a Python list.
    """
    submitfile.write(f"queue {var_name} in (" + " ".join(str(v) for v in values) + ")\n")

def create_asymptotic_batch(
    WP,
    shortcard,
    card,
    run_blind,
    limit_mode_label,
    pwd,
    mask_parameters=None,
    user_tag_path="",
    user_tag_suffix="",
):
    """
    Create and submit AsymptoticLimits jobs.

    Directory is shared between Expected and Observed:
      Batch/<WP>/Asymptotic/<shortcard>/

    Expected/Observed are separated by:
      - run script name
      - submit script name
      - condor batch name
      - log/output filenames
      - combine -n suffix
      - transferred output ROOT filename
    """
    mask_parameters = unique_preserve_order(
        mask_parameters or []
    )

    base_dir = (
        f"Batch/{WP}/Asymptotic/"
        f"{shortcard}{user_tag_path}"
    )
    output_dir = f"{base_dir}/output"
    logs_dir = f"{base_dir}/logs"

    os.makedirs(base_dir, exist_ok=True)
    os.makedirs(output_dir, exist_ok=True)
    os.makedirs(logs_dir, exist_ok=True)

    run_name = f"run_Asymptotic_{limit_mode_label}.sh"
    submit_name = f"submit_Asymptotic_{limit_mode_label}.sh"
    run_path = f"{base_dir}/{run_name}"
    submit_path = f"{base_dir}/{submit_name}"

    os.system(f"cp Batch/submit_skeleton.sh {submit_path}")

    blind_opt = f" {run_blind}" if run_blind else ""

    # Case 1: 3ch / EMuFull physics models
    if "EMuFull" in WP or "3ch" in shortcard or "3ch" in WP:

        # Case 1a: HNL 3ch f scan
        if "Weinberg" not in shortcard:

            hnl_limit_options = make_mask_set_freeze_options(
                mask_parameters=mask_parameters,
                extra_set_parameters=[
                    ("r", 0),
                    ("f", "${F_VAL}"),
                ],
                extra_freeze_parameters=[
                    "f",
                ],
            )

            with open(run_path, "w") as runfile:
                runfile.write("#!/bin/bash\n")
                runfile.write("set -e\n")
                runfile.write("F_VAL=$1\n")
                runfile.write("ulimit -s unlimited\n")
                runfile.write(
                    f"combine -M AsymptoticLimits "
                    f"{card}"
                    f"{blind_opt} "
                    f"{hnl_limit_options} "
                    f"-n _{limit_mode_label}_f${{F_VAL}}\n"
                )

            os.system(f"chmod +x {run_path}")

            combine_output = f"higgsCombine_{limit_mode_label}_f$(f_val).AsymptoticLimits.mH120.root"
            remapped_output = f"output/{shortcard}_Asymptotic_{limit_mode_label}_f$(f_val).root"

            with open(submit_path, "a") as submitfile:
                submitfile.write(f"executable = {run_name}\n")
                submitfile.write("arguments = $(f_val)\n")
                submitfile.write(f"log = logs/{shortcard}_Asymptotic_{limit_mode_label}_f$(f_val).log\n")
                submitfile.write(f"output = logs/{shortcard}_Asymptotic_{limit_mode_label}_f$(f_val).out\n")
                submitfile.write(f"error = logs/{shortcard}_Asymptotic_{limit_mode_label}_f$(f_val).out\n")
                submitfile.write(f"transfer_output_files = {combine_output}\n")
                submitfile.write(f"transfer_output_remaps = \"{combine_output} = {remapped_output}\"\n")
                write_condor_queue_values(submitfile, "f_val", make_hnl_f_points(shortcard))

        # Case 1b: Weinberg 3ch w scan
        else:
            w_points = make_weinberg_w_points(step=0.5)

            weinberg_limit_options = make_mask_set_freeze_options(
                mask_parameters=mask_parameters,
                extra_set_parameters=[
                    ("r", 0),
                    ("wMuMu", "${W_MuMu}"),
                    ("wEMu", "${W_EMU}"),
                    ("wEE", "${W_EE}"),
                ],
                extra_freeze_parameters=[
                    "wMuMu",
                    "wEMu",
                    "wEE",
                ],
            )

            with open(run_path, "w") as runfile:
                runfile.write("#!/bin/bash\n")
                runfile.write("set -e\n")
                runfile.write("W_MuMu=$1\n")
                runfile.write("W_EMU=$2\n")
                runfile.write("W_EE=$3\n")
                runfile.write("W_LABEL=$4\n")
                runfile.write("ulimit -s unlimited\n")
                runfile.write("\n")
                runfile.write("echo \"Running Weinberg point: ${W_LABEL}\"\n")
                runfile.write("echo \"  wMuMu = ${W_MuMu}\"\n")
                runfile.write("echo \"  wEMu  = ${W_EMU}\"\n")
                runfile.write("echo \"  wEE   = ${W_EE}\"\n")
                runfile.write("\n")
                runfile.write(
                    f"combine -M AsymptoticLimits "
                    f"{card}"
                    f"{blind_opt} "
                    f"{weinberg_limit_options} "
                    f"--setParameterRanges r=0,10000 "
                    f"-n _{limit_mode_label}_${{W_LABEL}}\n"
                )

            os.system(f"chmod +x {run_path}")

            combine_output = f"higgsCombine_{limit_mode_label}_$(w_label).AsymptoticLimits.mH120.root"
            remapped_output = f"output/{shortcard}_Asymptotic_{limit_mode_label}_$(w_label).root"

            with open(submit_path, "a") as submitfile:
                submitfile.write(f"executable = {run_name}\n")
                submitfile.write("arguments = $(w_mumu) $(w_emu) $(w_ee) $(w_label)\n")
                submitfile.write(f"log = logs/{shortcard}_Asymptotic_{limit_mode_label}_$(w_label).log\n")
                submitfile.write(f"output = logs/{shortcard}_Asymptotic_{limit_mode_label}_$(w_label).out\n")
                submitfile.write(f"error = logs/{shortcard}_Asymptotic_{limit_mode_label}_$(w_label).out\n")
                submitfile.write(f"transfer_output_files = {combine_output}\n")
                submitfile.write(f"transfer_output_remaps = \"{combine_output} = {remapped_output}\"\n")
                submitfile.write("queue w_mumu,w_emu,w_ee,w_label from (\n")
                for wMuMu, wEMu, wEE, label in w_points:
                    submitfile.write(f"{wMuMu} {wEMu} {wEE} {label}\n")
                submitfile.write(")\n")

    # Case 2: ordinary per-channel HNL / non-3ch cards
    else:

        asymptotic_mask_options = make_mask_set_freeze_options(
            mask_parameters
        )

        with open(run_path, "w") as runfile:
            runfile.write("#!/bin/bash\n")
            runfile.write("set -e\n")
            runfile.write("ulimit -s unlimited\n")
            runfile.write(
                f"combine -M AsymptoticLimits "
                f"{card}"
                f"{blind_opt} "
                f"{asymptotic_mask_options} "
                f"-n _{limit_mode_label}\n"
            )

        os.system(f"chmod +x {run_path}")

        combine_output = f"higgsCombine_{limit_mode_label}.AsymptoticLimits.mH120.root"
        remapped_output = f"output/{shortcard}_Asymptotic_{limit_mode_label}.root"

        with open(submit_path, "a") as submitfile:
            submitfile.write(f"executable = {run_name}\n")
            submitfile.write(f"log = logs/{shortcard}_Asymptotic_{limit_mode_label}.log\n")
            submitfile.write(f"output = logs/{shortcard}_Asymptotic_{limit_mode_label}.out\n")
            submitfile.write(f"error = logs/{shortcard}_Asymptotic_{limit_mode_label}.out\n")
            submitfile.write(f"transfer_output_files = {combine_output}\n")
            submitfile.write(f"transfer_output_remaps = \"{combine_output} = {remapped_output}\"\n")
            submitfile.write("queue\n")

    os.chdir(base_dir)
    batch_name = (
        f"{shortcard}_{WP}_Asymptotic_"
        f"{limit_mode_label}{user_tag_suffix}"
    )

    os.system(
        f'condor_submit -a "priority = -15" '
        f'{submit_name} '
        f'-batch-name {batch_name}'
    )
    os.chdir(pwd)

# ----------------------------------------------------------------------
# User tag helpers for nuisance-check outputs
# ----------------------------------------------------------------------
UserTagClean = make_effective_user_tag(args)

# For directory paths:
#   tag=""   -> ""
#   tag="v1" -> "/v1"
UserTagPath = f"/{UserTagClean}" if UserTagClean else ""

# For filenames / combine names / condor batch names:
#   tag=""   -> ""
#   tag="v1" -> "_v1"
UserTagSuffix = f"_{UserTagClean}" if UserTagClean else ""

# --- Main Logic ---

for RunList in args.RunLists:
  cards = open(RunList).readlines() if args.Input is None else [args.Input]
  NCARD = len(cards)

  if args.Input is None:
    WP = RunList.split('.')[-2].replace('RunList_','')
  
    for runlist_prefix in ["Run2Sum_", "Run2_"]:
      if WP.startswith(runlist_prefix):
        WP = WP[len(runlist_prefix):] # Remove only Run2* at the head
        break
  else:
    WP = args.Input.split('/')[-2]

  if args.pdf:
    os.system(f'mkdir -p {this_check}/{WP}/{AsimovName}{UserTagPath}')
  elif args.Work or IsNuis: # Make workspace or perform statistical tests
    with open(WP+'/submit_skeleton.sh','w') as skel:
      skel.write("universe = vanilla\n")
      skel.write("+SingularityImage = \"/cvmfs/singularity.opensciencegrid.org/opensciencegrid/osgvo-el9:latest\"\n")
      skel.write("should_transfer_files = YES\n")
      skel.write("when_to_transfer_output = ON_EXIT\n")
      skel.write("request_memory = 8000\n")
      skel.write("request_cpus = 2\n") if args.Impact else skel.write("request_cpus = 1\n")
  else: # Extract limits
    os.system('mkdir -p Batch/'+WP)
    with open('Batch/submit_skeleton.sh','w') as skel:
      skel.write("universe = vanilla\n")
      skel.write("getenv   = True\n")
      skel.write("should_transfer_files = YES\n")
      skel.write("when_to_transfer_output = ON_EXIT\n")
      skel.write("request_memory = 4000\n")
      skel.write("request_cpus = 1\n")
  
  for i in range(0,NCARD):
    card = cards[i].strip('\n')
    if '#' in card: continue
    shortcard = card.split('/')[-1].replace(".root","").replace(".txt","").replace("card_","") # Run2_EE_Ext_M500_syst

    if args.Masks and not card.endswith(".root"):
      print(
          f"[ERROR] --mask requires a ROOT workspace "
          f"created with --channel-masks."
      )
      print(f"        Input: {card}")
      print(
          "        A plain text datacard does not contain "
          "mask_<channel> parameters."
      )
      sys.exit(1)

    this_mass = "0" if "Weinberg" in shortcard else shortcard.split('_M')[-1].split('_')[0]

    requested_mask_parameters = (
        resolve_mask_parameters(args.Masks, shortcard)
        if args.Masks
        else []
    )

    # In the existing non-Unblind GOF convention, all SRs are automatically
    # masked. The SR names must be expanded differently for Run2 and Run2Sum.
    cronly_mask_parameters = list(requested_mask_parameters)

    if args.GOF and not args.Unblind:
        automatic_sr_masks = resolve_mask_parameters(
            ["sr1", "sr2", "sr3"],
            shortcard,
        )

        cronly_mask_parameters = unique_preserve_order(
            automatic_sr_masks + requested_mask_parameters
        )

    mask_only_options = make_mask_set_freeze_options(
        requested_mask_parameters
    )

    if requested_mask_parameters:
        print(f"[MASK] {shortcard}")
        print(
            "       requested masks: "
            + ", ".join(args.Masks)
        )
        print(
            "       workspace parameters: "
            + ", ".join(requested_mask_parameters)
        )

    if args.GOF and not args.Unblind:
        print(
            "       CR-only mask parameters: "
            + ", ".join(cronly_mask_parameters)
        )

    if "EMuFull" in WP or "3ch" in shortcard or "3ch" in WP:
      if not args.Unblind:
        if "Weinberg" not in shortcard: AsimovName = fmt_w_label(f"r{args.r}f{args.f}") # HNL
        else: AsimovName = fmt_w_label(f"r{args.r}wMuMu{args.wMuMu}wEE{args.wEE}wEMu{args.wEMu}") # Weinberg
 
    if args.pdf:
      if args.FitDiag or args.GOF:
        this_shortcard = shortcard
      else:
        this_shortcard = shortcard+"_DefMod" if ((float(this_mass) > 3000.) or "SSWW" in shortcard) else shortcard

      if args.Impact:
        SRname = match.group() if (match := re.search(r'sr\d+', shortcard)) else ""
      
        impact_base = f"Impact_{this_shortcard}_{AsimovName}{UserTagSuffix}"
        impact_first_page = f"{impact_base}_1.pdf"
        impact_summary_pdf = f"Impact_{this_shortcard}_{AsimovName}.pdf"
        impact_summary_png = f"Impact_{this_shortcard}_{AsimovName}{UserTagSuffix}"
      
        os.system(f'mkdir -p {this_check}/{WP}/{AsimovName}{UserTagPath}/{SRname}')
      
        os.chdir(f"{pwd}/{WP}/{shortcard}/{this_check}/{AsimovName}{UserTagPath}")
        os.system(f"pdfseparate {impact_base}.pdf -f 1 -l 1 {impact_first_page}")
        os.system(
            f"cp {impact_first_page} "
            f"{pwd}/{this_check}/{WP}/{AsimovName}{UserTagPath}/{SRname}/{impact_summary_pdf}"
        )
      
        os.chdir(f"{pwd}/{this_check}/{WP}/{AsimovName}{UserTagPath}/{SRname}")
        os.system(f"pdftoppm -png -singlefile {impact_summary_pdf} {impact_summary_png}")
        os.chdir(pwd)
      if args.MDfit:
        source_dir = (
            f"{pwd}/{WP}/{shortcard}/{this_check}/"
            f"{AsimovName}{UserTagPath}"
        )
        target_dir = (
            f"{pwd}/{this_check}/{WP}/"
            f"{AsimovName}{UserTagPath}"
        )

        os.makedirs(target_dir, exist_ok=True)
        os.chdir(source_dir)

        if not (
            "EMuFull" in WP
            or "3ch" in shortcard
            or "3ch" in WP
        ):
          for scan_range in [
              "rRange1",
              "rRange10",
              "rRange100",
          ]:
            plot_base = (
                f"MDfit_{scan_range}_"
                f"{this_shortcard}_{AsimovName}"
            )

            os.system(
                f"cp {plot_base}.pdf "
                f"{plot_base}.png "
                f"{target_dir}"
            )

        os.chdir(pwd)
      if args.FitDiag:
        source_dir = (
            f"{pwd}/{WP}/{shortcard}/{this_check}/"
            f"{AsimovName}{UserTagPath}"
        )
        target_dir = (
            f"{pwd}/{this_check}/{WP}/"
            f"{AsimovName}{UserTagPath}"
        )

        os.makedirs(target_dir, exist_ok=True)
        os.chdir(source_dir)

        os.system(
            f"python3 $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/test/diffNuisances.py -a -A fitDiagnostics_{this_shortcard}.root > pulls_{this_shortcard}.txt\n"
            f"cp pulls_{this_shortcard}.txt "
            f"{target_dir}/"
            f"pulls_{this_shortcard}_{AsimovName}"
            f"{UserTagSuffix}.txt"
        )

        os.chdir(pwd)
      if args.GOF:
        os.chdir(
            f"{pwd}/{WP}/{shortcard}/{this_check}/"
            f"{AsimovName}{UserTagPath}"
        )
        os.system(
            f"cp gof_{this_shortcard}* "
            f"{pwd}/{this_check}/{WP}/"
            f"{AsimovName}{UserTagPath}"
        )
        os.chdir(pwd)
      if args.Breakdown:
        os.chdir(pwd+"/"+WP+"/"+shortcard+'/'+this_check+'/'+AsimovName)
        os.system("cp "+this_shortcard+"_"+AsimovName+"_Breakdown.pdf "+this_shortcard+"_"+AsimovName+"_Breakdown.png "+pwd+"/"+this_check+"/"+WP+"/"+AsimovName)
        os.chdir(pwd)
      continue
    elif args.Work:
      os.system('mkdir -p '+WP+'/'+shortcard)
      os.system('cp '+WP+'/submit_skeleton.sh '+WP+'/'+shortcard+'/submit_Workspace.sh')
    elif IsNuis:
      if "EMuFull" in WP or "3ch" in shortcard or "3ch" in WP:
        if "DefMod" in shortcard: continue # Must use the actual physics model
      nuis_dir = f"{WP}/{shortcard}/{this_check}/{AsimovName}{UserTagPath}"
      nuis_submit = f"submit_{this_check}_{AsimovName}{UserTagSuffix}.sh"
      
      os.system(f'mkdir -p {nuis_dir}')
      os.system(f'cp {WP}/submit_skeleton.sh {nuis_dir}/{nuis_submit}')
      os.system(f'cp {WP}/{shortcard}/{shortcard}.root {nuis_dir}')
      if ((float(this_mass) > 3000.) or "SSWW" in shortcard):
        os.system(f'cp {WP}/{shortcard}/{shortcard}_DefMod.root {nuis_dir}')
    elif not args.Asymptotic:
      # CLs extraction
      cls_base_dir = (
          f"Batch/{WP}/full_CLs/"
          f"{shortcard}{UserTagPath}"
      )

      os.makedirs(f"{cls_base_dir}/output", exist_ok=True)
      os.makedirs(f"{cls_base_dir}/logs", exist_ok=True)
      
      quantiles_to_run = []
      if args.Unblind:
        print("[INFO] --Unblind requested: CLs will run observed limit only; Q1-Q5/Full are ignored.")
        quantiles_to_run.append(('Obs', None))
      else:
        if args.Full or args.Q1: quantiles_to_run.append(('Q1', '0.025'))
        if args.Full or args.Q2: quantiles_to_run.append(('Q2', '0.160'))
        if args.Full or args.Q3: quantiles_to_run.append(('Q3', '0.500'))
        if args.Full or args.Q4: quantiles_to_run.append(('Q4', '0.840'))
        if args.Full or args.Q5: quantiles_to_run.append(('Q5', '0.975'))

      for q_name, q_val in quantiles_to_run:
          
          # [Refactored] Parallel + Grid Logic
          if args.Split > 1:
              if not args.rRange:
                  print(f"[ERROR] You requested split jobs (-n {args.Split}) but provided no rRange.")
                  print("For HybridNew parallelization, you MUST provide --rRange min:max:step (e.g. 0.1:5.0:0.5)")
                  print("This is to ensure all jobs simulate the same r-values for merging.")
                  sys.exit(1)
              
              r_points = parse_r_range(args.rRange)
              print(f"[{shortcard}] Grid Scan Mode: {len(r_points)} points from {r_points[0]} to {r_points[-1]}")
              
              dag_file = create_hybrid_grid_dag(
                  WP=WP,
                  shortcard=shortcard,
                  card=card,
                  quantile=q_name,
                  quant_val=q_val,
                  total_toys=args.Ntoy,
                  n_split=args.Split,
                  r_points=r_points,
                  pwd=pwd,
                  is_diagnostic=args.Diagnostic,
                  user_tag=UserTagClean,
                  is_unblind=args.Unblind,
                  mask_options=mask_only_options,
                  user_tag_path=UserTagPath,
              )
 
              os.chdir(cls_base_dir)

              # [Modified] Submit and Show Cluster ID
              batch_name = (
                  f"{shortcard}_{WP}_{q_name}"
                  f"_Ntoy{args.Ntoy}{UserTagSuffix}"
              )
              if args.rRange: batch_name += f"_{args.rRange}"
              
              command = f'condor_submit_dag -batch-name {batch_name} {dag_file}'
              status, output = cmd.getstatusoutput(command)
              
              if status == 0:
                  print(f"\n[Submission Success] Job submitted: {batch_name}")
                  print(output)
                  match = re.search(r'cluster (\d+)', output)
                  if match:
                      print(f"To remove this job: condor_rm {match.group(1)}  OR  condor_rm -constraint 'JobBatchName == \"{batch_name}\"'")
                  print("="*60 + "\n")
              else:
                  print(f"[Submission Failed] Error executing: {command}")
                  print(output)

              os.chdir(pwd)
              
          else:
              # [Fallback] Single Job (Existing Logic with Diagnostic capability)
              os.system(
                  f"cp Batch/submit_skeleton.sh "
                  f"{cls_base_dir}/submit_{q_name}.sh"
              )
              extra_flags = f"--saveToys --saveHybridResult -v 1 --plot=limit_scan_{shortcard}_{q_name}.png" if args.Diagnostic else ""
              expected_flag = expected_from_grid_flag(q_val)
              quant_suffix = hybrid_quant_suffix(q_val)
              name_suffix = shortcard if q_val is not None else f"{shortcard}_Obs"
              
              with open(
                  f"{cls_base_dir}/run_{q_name}.sh",
                  "w",
              ) as runfile:
                runfile.write("#!/bin/bash\n")

                data_opt = ""

                if not args.Unblind:
                  asimov_tag = (
                      f"_{shortcard}_{q_name}_"
                      f"prefitAsimov"
                  )
                  data_opt = write_prefit_asimov_and_get_data_opt(
                      runfile=runfile,
                      card=card,
                      tag=asimov_tag,
                      mask_options=mask_only_options,
                  )

                mask_opt = (
                    f"{mask_only_options} "
                    if mask_only_options
                    else ""
                )

                runfile.write(
                    f"combine -M HybridNew "
                    f"--LHCmode LHC-limits "
                    f"{card} "
                    f"{data_opt}"
                    f"{mask_opt}"
                    f"-n {name_suffix} "
                    f"{expected_flag}"
                    f"-T {args.Ntoy} "
                    f"{extra_flags}\n"
                )

              with open(
                  f"{cls_base_dir}/submit_{q_name}.sh",
                  "a",
              ) as submitfile:
                submitfile.write(f"executable = run_{q_name}.sh\n")
                submitfile.write(f"log = {shortcard}_{q_name}.log\n")
                submitfile.write(f"output = {shortcard}_{q_name}.out\n")
                submitfile.write(f"error = {shortcard}_{q_name}.out\n")
                if args.Diagnostic:
                  this_hybrid_output = f"higgsCombine{name_suffix}.HybridNew.mH120.123456{quant_suffix}.root"
                  submitfile.write(f"transfer_output_files = {this_hybrid_output},limit_scan_{shortcard}_{q_name}.png\n")
                  submitfile.write(f"transfer_output_remaps = \"{this_hybrid_output} = output/{shortcard}_{q_name}.root\"\n")
                else:
                  this_hybrid_output = f"higgsCombine{name_suffix}.HybridNew.mH120{quant_suffix}.root"
                  submitfile.write(f"transfer_output_files = {this_hybrid_output}\n")
                  submitfile.write(f"transfer_output_remaps = \"{this_hybrid_output} = output/{shortcard}_{q_name}.root\"\n")
                submitfile.write("queue\n")
              
              os.chdir(cls_base_dir)

              batch_name = (
                  f"{shortcard}_{WP}_{q_name}"
                  f"{UserTagSuffix}"
              )

              os.system(
                  f"condor_submit submit_{q_name}.sh "
                  f"-batch-name {batch_name}"
              )

              os.chdir(pwd)

    if args.Asymptotic:
      create_asymptotic_batch(
          WP=WP,
          shortcard=shortcard,
          card=card,
          run_blind=RunBlind,
          limit_mode_label=LimitModeLabel,
          pwd=pwd,
          mask_parameters=requested_mask_parameters,
          user_tag_path=UserTagPath,
          user_tag_suffix=UserTagSuffix,
      )

    if args.Work:
      with open(WP+"/"+shortcard+"/MakeWorkspace.sh",'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("pushd "+pwd+"/"+WP+"/"+shortcard+"\n")
        runfile.write("echo Setting cmsenv environment...\n")
        runfile.write("cmsenv\n")
        card = card.replace(".root",".txt") # The Runlist contains card_name.root by default.
        if "3ch" in shortcard:
          if "Weinberg" in shortcard: # Weinberg
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=10000 --PO mode=Weinberg --channel-masks -o "+shortcard+".root\n") # consistent scaling with the LimitInput
          else: # HNL
            if (float(this_mass) > 3000.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=0.1 --PO mode=HNL --channel-masks -o "+shortcard+".root\n") # consistent r0 with the LimitInput
            elif (float(this_mass) <= 100.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=1   --PO mode=HNL --channel-masks -o "+shortcard+".root\n") # for MDfit showing; LimitInput was scaled by 0.001
            else:
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=0.1 --PO mode=HNL --channel-masks -o "+shortcard+".root\n") # for MDfit showing; LimitInput was scaled by 0.01
        elif "3ch" in WP: # "3ch" not in shortcard but still WP is 3ch --> HNL 3ch vs 1D envelope test
          if "Weinberg" in shortcard: # Weinberg
            continue
          else: # HNL
            ForceChannel = (
                            "--PO 'reMuMu=.*' --PO 'reEE=^$' --PO 'reEMu=^$'" if "MuMu" in shortcard else
                            "--PO 'reEE=.*' --PO 'reMuMu=^$' --PO 'reEMu=^$'" if "EE" in shortcard else
                            "--PO 'reEMu=.*' --PO 'reEE=^$' --PO 'reMuMu=^$'" if "EMu" in shortcard else
                            "ERROR"
                        )
            if (float(this_mass) > 3000.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=0.1 --PO mode=HNL "+ForceChannel+" --channel-masks -o "+shortcard+".root\n") # consistent r0 with the LimitInput
            elif (float(this_mass) <= 100.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=1   --PO mode=HNL "+ForceChannel+" --channel-masks -o "+shortcard+".root\n") # for MDfit showing; LimitInput was scaled by 0.001
            else:
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_3ch "+card+" --PO r0=0.1 --PO mode=HNL "+ForceChannel+" --channel-masks -o "+shortcard+".root\n") # for MDfit showing; LimitInput was scaled by 0.01
        elif "EMu" in shortcard:
          if "EMuFull" in WP:
            if (float(this_mass) > 3000.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu_Full "+card+" --PO r0=0.1 --channel-masks -o "+shortcard+".root\n")
            elif (float(this_mass) <= 100.):
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu_Full "+card+" --PO r0=1 --channel-masks -o "+shortcard+".root\n")
            else:
              runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu_Full "+card+" --PO r0=0.1 --channel-masks -o "+shortcard+".root\n")
          else:
            runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel_EMu "+card+" --channel-masks -o "+shortcard+".root\n")
        else:
          runfile.write("text2workspace.py -P HiggsAnalysis.CombinedLimit.HNDilepModel:hnDilepModel "+card+" --channel-masks -o "+shortcard+".root\n")
        if not("EMuFull" in WP or "3ch" in shortcard or "3ch" in WP) and ((float(this_mass) > 3000.) or "SSWW" in shortcard): # mass is above 3000 GeV so it only contains SSWW, or SSWW only --> add DefMod for impact check
          runfile.write("text2workspace.py "+card+" --channel-masks -o "+shortcard+"_DefMod.root\n") # impact with default physics model with SSWW: see https://cms-talk.web.cern.ch/t/0-impact-on-poi-negative-bin-issue/42793
      with open(WP+"/"+shortcard+"/submit_Workspace.sh",'a') as submitfile:
        submitfile.write("executable = MakeWorkspace.sh\n")
        submitfile.write("log = "+shortcard+"_Workspace.log\n")
        submitfile.write("output = "+shortcard+"_Workspace.out\n")
        submitfile.write("error = "+shortcard+"_Workspace.out\n")
        submitfile.write("queue\n")
      os.chdir(WP+"/"+shortcard)
      os.system('condor_submit -a "priority = -15" submit_Workspace.sh -batch-name '+shortcard+'_'+WP+'_Workspace')
      os.chdir(pwd)

    if IsNuis:
      # ------------------------------------------------------------------
      # Split GOF mode:
      #
      #   ./create-batch.py -l RunList_XXX.txt --GOF --Unblind -t 1000 -n 20
      #     -> unblind b-only GOF, split toys, DAGMan
      #
      #   ./create-batch.py -l RunList_XXX.txt --GOF -t 1000 -n 20
      #     -> non-unblind CR-only GOF, split toys, DAGMan
      #
      # Notes:
      #   - only args.Split > 1 uses DAGMan
      #   - args.Split == 1 falls through to the existing single-job GOF block
      #   - GOF toy diagnostic options are passed only to toy commands
      # ------------------------------------------------------------------
      if args.GOF and args.Split > 1:
        list_shortcard = [shortcard, shortcard+"_DefMod"] if ((float(this_mass) > 3000.) or "SSWW" in shortcard) else [shortcard]

        gof_variant_suffix = make_gof_variant_suffix(args)
        gof_toy_mode_opts = make_gof_toy_mode_options(args)

        for this_shortcard in list_shortcard:
          if "DefMod" in this_shortcard:
            continue # Must use the actual physics model

          dag_file = create_gof_split_dag(
            WP=WP,
            shortcard=shortcard,
            this_shortcard=this_shortcard,
            card=card,
            total_toys=args.Ntoy,
            n_split=args.Split,
            pwd=pwd,
            cmssw_base=CMSSW_BASE,
            asimov_name=AsimovName,
            is_unblind=args.Unblind,
            toy_mode_opts=gof_toy_mode_opts,
            variant_suffix=gof_variant_suffix,
            seed_base=args.SeedBase,
            requested_mask_parameters=requested_mask_parameters,
            cronly_mask_parameters=cronly_mask_parameters,
            user_tag_path=UserTagPath,
          )

          os.chdir(
            f"{WP}/{shortcard}/{this_check}/"
            f"{AsimovName}{UserTagPath}"
          )

          gof_mode_label = "Unblind" if args.Unblind else "CRonly"
          batch_name = (
            f"{shortcard}_{WP}_GOF_{gof_mode_label}"
            f"_Ntoy{args.Ntoy}_Split{args.Split}{gof_variant_suffix}"
          )

          command = f'condor_submit_dag -batch-name {batch_name} {dag_file}'
          status, output = cmd.getstatusoutput(command)

          if status == 0:
            print(f"\n[GOF DAG Submission Success] {batch_name}")
            print(output)
            match = re.search(r'cluster (\d+)', output)
            if match:
              print(f"To remove this DAG: condor_rm {match.group(1)}")
            print("="*60 + "\n")
          else:
            print(f"[GOF DAG Submission Failed] Error executing: {command}")
            print(output)

          os.chdir(pwd)

        continue

      list_shortcard = [shortcard, shortcard+"_DefMod"] if ((float(this_mass) > 3000.) or "SSWW" in shortcard) else [shortcard]
      
      nuis_dir = f"{WP}/{shortcard}/{this_check}/{AsimovName}{UserTagPath}"
      nuis_run = f"Run{this_check}_{AsimovName}{UserTagSuffix}.sh"
      nuis_submit = f"submit_{this_check}_{AsimovName}{UserTagSuffix}.sh"
      
      with open(f"{nuis_dir}/{nuis_run}", 'w') as runfile:
        runfile.write("#!/bin/bash\n")
        runfile.write(f"pushd {pwd}/{nuis_dir}\n")
        runfile.write("echo Setting cmsenv environment...\n")
        runfile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n")
        runfile.write("cmsenv\n")
        runfile.write("echo Done.\n")
        runfile.write("popd\n")

        for this_shortcard in list_shortcard:
          if args.FitDiag:
            if "DefMod" in this_shortcard: continue # Must use the actual physics model
            runfile.write("echo Running FitDiagnostics...\n") # Asimov set as default; FIXME later to choose whether Asimov or not
            runfile.write(
              f"combine -M FitDiagnostics "
              f"{pwd}/{WP}/{shortcard}/{this_shortcard}.root "
              f"--rMin -10 --rMax 10 "
              f"--saveShapes "
              f"--saveWithUncertainties "
              f"--numToysForShapes 1000 "
              f"--saveNormalizations "
              f"--saveWorkspace "
              f"--verbose 3 "
              f"{mask_only_options} "
              f"-n _{this_shortcard} "
              f"--plots "
              f"{AsimovSetting}\n"
            )
            runfile.write(f"python3 $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/test/diffNuisances.py -a -A fitDiagnostics_{this_shortcard}.root > pulls_{this_shortcard}.txt\n")
          elif args.GOF:
            if "DefMod" in this_shortcard: continue # Must use the actual physics model

            runfile.write("echo Running the goodness of fit test...\n")

            gof_variant_suffix = make_gof_variant_suffix(args)
            gof_toy_mode_opts = make_gof_toy_mode_options(args)

            if args.Unblind:
              obs_suffix = f"_gof_bonly_obs_{this_shortcard}{gof_variant_suffix}"
              toy_suffix = f"_gof_bonly_toys_Ntoy{args.Ntoy}_{this_shortcard}{gof_variant_suffix}"

              obs_file = f"higgsCombine{obs_suffix}.GoodnessOfFit.mH120.root"
              toy_file = f"higgsCombine{toy_suffix}.GoodnessOfFit.mH120.123456.root"

              json_file = f"gof_{this_shortcard}{gof_variant_suffix}.json"
              plot_base = f"gof_{this_shortcard}{gof_variant_suffix}_plot"

              runfile.write(
                f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                f"{make_gof_bonly_set_freeze_options(requested_mask_parameters)} "
                f"--algo saturated "
                f"-n {obs_suffix}\n"
              )

              runfile.write(
                f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                f"-t {args.Ntoy} {gof_toy_mode_opts} "
                f"{make_gof_bonly_set_freeze_options(requested_mask_parameters)} "
                f"--algo saturated "
                f"-n {toy_suffix}\n"
              )

              runfile.write(
                f"combineTool.py -M CollectGoodnessOfFit "
                f"--input {obs_file} {toy_file} "
                f"-o {json_file}\n"
              )

              runfile.write(
                f"plotGof.py {json_file} "
                f"--statistic saturated --mass 120.0 "
                f"-o {plot_base} "
                f"--title-right=\"{this_shortcard}\"\n"
              )

            else:
              asimov_suffix = f"_gof_Asimov_{this_shortcard}{gof_variant_suffix}"

              obs_suffix = f"_gof_CRonly_obs_{this_shortcard}{gof_variant_suffix}"
              toy_suffix = f"_gof_CRonly_toysFreq_Ntoy{args.Ntoy}_{this_shortcard}{gof_variant_suffix}"

              obs_file = f"higgsCombine{obs_suffix}.GoodnessOfFit.mH120.root"
              toy_file = f"higgsCombine{toy_suffix}.GoodnessOfFit.mH120.123456.root"

              json_file = f"gof_{this_shortcard}_CRonly{gof_variant_suffix}.json"
              plot_base = f"gof_{this_shortcard}_CRonly{gof_variant_suffix}_plot"

              runfile.write(
                f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                f"-t -1 "
                f"{make_gof_bonly_set_freeze_options(requested_mask_parameters)} "
                f"--algo saturated "
                f"-n {asimov_suffix}\n"
              )

              runfile.write(
                f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                f"--algo saturated "
                f"{make_gof_cronly_set_freeze_options(cronly_mask_parameters)} "
                f"-n {obs_suffix}\n"
              )

              runfile.write(
                f"combine -M GoodnessOfFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                f"-t {args.Ntoy} {gof_toy_mode_opts} "
                f"--algo saturated "
                f"{make_gof_cronly_set_freeze_options(cronly_mask_parameters)} "
                f"-n {toy_suffix}\n"
              )

              runfile.write(
                f"combineTool.py -M CollectGoodnessOfFit "
                f"--input {obs_file} {toy_file} "
                f"-o {json_file}\n"
              )

              runfile.write(
                f"plotGof.py {json_file} "
                f"--statistic saturated --mass 120.0 "
                f"-o {plot_base} "
                f"--title-right=\"CR-only {this_shortcard}\"\n"
              )
          elif args.Impact:
            impact_label = f"{this_shortcard}_{AsimovName}{UserTagSuffix}"
          
            if (float(this_mass) > 3000.):
              rMin, rMax = parse_impact_r_bounds(args.rRange, scale=1.0)
            else:
              rMin, rMax = parse_impact_r_bounds(args.rRange, scale=0.1)
          
            impact_range = f"--rMin {rMin:g} --rMax {rMax:g}"
          
            runfile.write(
              f"combineTool.py -M Impacts "
              f"-d {pwd}/{WP}/{shortcard}/{this_shortcard}.root "
              f"-m {this_mass} "
              f"{impact_range} "
              f"{mask_only_options} "
              f"--robustFit 1 --doInitialFit "
              f"--name Impact_{impact_label} "
              f"{AsimovSetting}\n"
            )
          
            runfile.write(
              f"combineTool.py -M Impacts "
              f"-d {pwd}/{WP}/{shortcard}/{this_shortcard}.root "
              f"-m {this_mass} "
              f"{impact_range} "
              f"{mask_only_options} "
              f"--robustFit 1 --doFits "
              f"--name Impact_{impact_label} "
              f"{AsimovSetting}\n"
            )

            runfile.write(
              f"combineTool.py -M Impacts "
              f"-d {pwd}/{WP}/{shortcard}/{this_shortcard}.root "
              f"-m {this_mass} "
              f"--output {impact_label}_impacts.json "
              f"--name Impact_{impact_label}\n"
            )
          
            runfile.write(
              f"plotImpacts.py "
              f"-i {impact_label}_impacts.json "
              f"-o Impact_{impact_label}\n"
            )
            #runfile.write(f"combineTool.py -M Impacts -d {pwd}/{WP}/{shortcard}/{this_shortcard}.root -m {this_mass} --rMin -10 --rMax 10 --robustFit 1 --doInitialFit --cminDefaultMinimizerStrategy 0 --name Impact_{this_shortcard}_{AsimovName}_{args.UserTag} {AsimovSetting}\n")
            #runfile.write(f"combineTool.py -M Impacts -d {pwd}/{WP}/{shortcard}/{this_shortcard}.root -m {this_mass} --rMin -10 --rMax 10 --robustFit 1 --doFits --cminDefaultMinimizerStrategy 0 --name Impact_{this_shortcard}_{AsimovName}_{args.UserTag} {AsimovSetting}\n") # Use this only when the autoMCStat gives fit failure and everything is ok
          elif args.FastScan:
            runfile.write(f"combineTool.py -M FastScan -w {pwd}/{WP}/{shortcard}/{this_shortcard}.root:w -o {this_shortcard}_Asimov_nll {AsimovSetting}\n")
            runfile.write(f"combineTool.py -M FastScan -w {pwd}/{WP}/{shortcard}/{this_shortcard}.root:w -o {this_shortcard}_nll\n")
          elif args.MDfit:
            if "EMuFull" in WP or "3ch" in shortcard or "3ch" in WP:
              if "DefMod" in this_shortcard: continue # Must use the actual physics model
              if "Weinberg" not in this_shortcard: # HNL
                hnl_mdfit_options = make_mask_set_freeze_options(
                    mask_parameters=requested_mask_parameters,
                    extra_set_parameters=[
                        ("r", args.r),
                        ("f", args.f),
                    ],
                )

                runfile.write(
                    f"combineTool.py -M MultiDimFit "
                    f"{pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                    f"-t -1 "
                    f"{hnl_mdfit_options} "
                    f"--setParameterRanges r=0,2:f=0,1 "
                    f"--algo grid "
                    f"--points=2601 "
                    f"--alignEdges 1 "
                    f"--robustFit 1 "
                    f"--saveNLL "
                    f"--name _{this_shortcard}_grid_2D_Asimov_"
                    f"r{fmt_w_label(args.r)}"
                    f"f{fmt_w_label(args.f)}\n"
                )
              else: # Weinberg
                weinberg_mdfit_options = make_mask_set_freeze_options(
                    mask_parameters=requested_mask_parameters,
                    extra_set_parameters=[
                        ("r", args.r),
                        ("wMuMu", args.wMuMu),
                        ("wEE", args.wEE),
                        ("wEMu", args.wEMu),
                    ],
                )

                runfile.write(
                    f"combineTool.py -M MultiDimFit "
                    f"{pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                    f"-t -1 "
                    f"{weinberg_mdfit_options} "
                    f"--setParameterRanges r=0,2 "
                    f"--algo grid "
                    f"--points=201 "
                    f"--alignEdges 1 "
                    f"--robustFit 1 "
                    f"--saveNLL "
                    f"--name _{this_shortcard}_grid_2D_Asimov_"
                    f"r{fmt_w_label(args.r)}"
                    f"wMuMu{fmt_w_label(args.wMuMu)}"
                    f"wEE{fmt_w_label(args.wEE)}"
                    f"wEMu{fmt_w_label(args.wEMu)}\n"
                )
            else:
              runfile.write(
                  f"combineTool.py -M MultiDimFit "
                  f"{pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                  f"--algo grid "
                  f"--points=41 "
                  f"--rMin -1 --rMax 1 "
                  f"--alignEdges 1 "
                  f"{mask_only_options} "
                  f"{AsimovSetting} "
                  f"--name .{this_shortcard}_{AsimovName}_rRange1\n"
              )

              runfile.write(
                  f"combineTool.py -M MultiDimFit "
                  f"{pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                  f"--algo grid "
                  f"--points=41 "
                  f"--rMin -10 --rMax 10 "
                  f"--alignEdges 1 "
                  f"{mask_only_options} "
                  f"{AsimovSetting} "
                  f"--name .{this_shortcard}_{AsimovName}_rRange10\n"
              )

              runfile.write(
                  f"combineTool.py -M MultiDimFit "
                  f"{pwd}/{WP}/{shortcard}/{this_shortcard}.root "
                  f"--algo grid "
                  f"--points=401 "
                  f"--rMin -100 --rMax 100 "
                  f"--alignEdges 1 "
                  f"{mask_only_options} "
                  f"{AsimovSetting} "
                  f"--name .{this_shortcard}_{AsimovName}_rRange100\n"
              )
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_rRange1.MultiDimFit.mH120.root -o MDfit_rRange1_{this_shortcard}_{AsimovName}\n")
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_rRange10.MultiDimFit.mH120.root -o MDfit_rRange10_{this_shortcard}_{AsimovName}\n")
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_rRange100.MultiDimFit.mH120.root -o MDfit_rRange100_{this_shortcard}_{AsimovName}\n")
          elif args.Breakdown:
            rRange = {}
            if (float(this_mass) <= 100.):
              rRange['rMin'] = -0.5
              rRange['rMax'] = 0.5
              rRange['points'] = 21
            elif (float(this_mass) <= 3000.):
              rRange['rMin'] = -5
              rRange['rMax'] = 5
              rRange['points'] = 21
            else:
              rRange['rMin'] = -10
              rRange['rMax'] = 10
              rRange['points'] = 41
            runfile.write(f"combine -M MultiDimFit {pwd}/{WP}/{shortcard}/{this_shortcard}.root --points={rRange['points']} --rMin {rRange['rMin']} --rMax {rRange['rMax']} --alignEdges 1 {AsimovSetting} --saveWorkspace --saveFitResult -n .{this_shortcard}_{AsimovName}_saveWorkspace\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_total\n")
            # ChatGPT split 'theory' into 'pdf' and 'scale'.
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert,pileup {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi,btag_sf {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi,btag_sf,prefire {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,HEM {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,HEM --freezeParameters 'rgx{{prop_bin.+}}' {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM_mcstat\n")
            if "E" in this_shortcard: runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeNuisanceGroups jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,HEM,cf --freezeParameters 'rgx{{prop_bin.+}}' {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM_mcstat_cf\n")
            runfile.write(f"combine -M MultiDimFit higgsCombine.{this_shortcard}_{AsimovName}_saveWorkspace.MultiDimFit.mH120.root --algo grid --snapshotName MultiDimFit --setParameterRanges r={rRange['rMin']},{rRange['rMax']} --freezeParameters allConstrainedNuisances {AsimovSetting} --saveFitResult -n .{this_shortcard}_{AsimovName}_freeze_all\n")
            if "E" not in this_shortcard:
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf.MultiDimFit.mH120.root:\"jet+pdf\":5 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale.MultiDimFit.mH120.root:\"jet+pdf+scale\":6 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake.MultiDimFit.mH120.root:\"jet+pdf+scale+fake\":7 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep\":8 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup\":9 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi\":10 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag\":11 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire\":12 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire+met\":13 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire+met+xsec\":14 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire+met+xsec+HEM\":15 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM_mcstat.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire+met+xsec+HEM+mcstat\":16 higgsCombine.{this_shortcard}_{AsimovName}_freeze_all.MultiDimFit.mH120.root:\"stat\":17 --output {this_shortcard}_{AsimovName}_Breakdown --y-max 10 --y-cut 40 --breakdown \"jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,HEM,mc_stat,rest,stat\"\n")
            else:
              runfile.write(f"plot1DScan.py higgsCombine.{this_shortcard}_{AsimovName}_total.MultiDimFit.mH120.root --main-label \"Total Uncert.\" --others higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet.MultiDimFit.mH120.root:\"jet\":4 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf.MultiDimFit.mH120.root:\"jet+pdf\":5 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale.MultiDimFit.mH120.root:\"jet+pdf+scale\":6 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake.MultiDimFit.mH120.root:\"jet+pdf+scale+fake\":7 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep\":8 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup\":9 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi\":10 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag\":11 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire\":12 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire+met\":13 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire+met+xsec\":14 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire+met+xsec+HEM\":15 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM_mcstat.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire+met+xsec+HEM+mcstat\":16 higgsCombine.{this_shortcard}_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM_mcstat_cf.MultiDimFit.mH120.root:\"jet+pdf+scale+fake+lep+pileup+lumi+btag+prefire+met+xsec+HEM+mcstat+cf\":17 higgsCombine.{this_shortcard}_{AsimovName}_freeze_all.MultiDimFit.mH120.root:\"stat\":18 --output {this_shortcard}_{AsimovName}_Breakdown --y-max 10 --y-cut 40 --breakdown \"jet_uncert,pdf,scale,fake,lep_uncert,pileup,lumi,btag_sf,prefire,met_energy,xsec,HEM,mc_stat,cf,rest,stat\"\n")

        runfile.write("echo Done.\n")

      with open(f"{nuis_dir}/{nuis_submit}", 'a') as submitfile:
        submitfile.write(f"executable = {nuis_run}\n")
        submitfile.write(f"log = {shortcard}_Run{this_check}_{AsimovName}{UserTagSuffix}.log\n")
        submitfile.write(f"output = {shortcard}_Run{this_check}_{AsimovName}{UserTagSuffix}.out\n")
        submitfile.write(f"error = {shortcard}_Run{this_check}_{AsimovName}{UserTagSuffix}.out\n")
        submitfile.write("should_transfer_files = YES\n")
        submitfile.write("when_to_transfer_output = ON_EXIT\n")
        submitfile.write("queue\n")

      os.chdir(nuis_dir)
      
      if args.MDfit and ("EMuFull" in WP or "3ch" in shortcard or "3ch" in WP):
        if "Weinberg" not in shortcard: # HNL
          batch_name = (
            f"{shortcard}_{WP}_{this_check}{UserTagSuffix}"
            f"_grid_2D_Asimov_r{fmt_w_label(args.r)}f{fmt_w_label(args.f)}"
          )
        else: # Weinberg
          batch_name = (
            f"{shortcard}_{WP}_{this_check}{UserTagSuffix}"
            f"_grid_2D_Asimov_r{fmt_w_label(args.r)}"
            f"wMuMu{fmt_w_label(args.wMuMu)}"
            f"wEE{fmt_w_label(args.wEE)}"
            f"wEMu{fmt_w_label(args.wEMu)}"
          )
      else:
        batch_name = f"{shortcard}_{WP}_{this_check}_{AsimovName}{UserTagSuffix}"
      
      os.system(f'condor_submit -a "priority = -15" {nuis_submit} -batch-name {batch_name}')
      os.chdir(pwd)
