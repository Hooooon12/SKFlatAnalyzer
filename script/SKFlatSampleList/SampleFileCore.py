#!/usr/bin/env python3

import os
import sys
import ROOT
import csv
from urllib.request import urlopen
import subprocess
from datetime import datetime

def is_data(alias):
    """
    Checks if the given alias corresponds to a data-related category.
    """
    data_keywords = ["DoubleEG", "DoubleMuon", "EGamma", "MuonEG", "SingleElectron", "SingleMuon"]
    return any(keyword in alias for keyword in data_keywords)


def now():
    """
    Returns the current date and time as a string in MMDDHHMM format.
    """
    return datetime.now().strftime("%m%d%H%M")




def make_work_dir(verbose):
    """
    Creates a temporary working directory and returns its name.
    Optionally prints the directory name if 'verbose' is True.
    """
    dir_name = f'tmp_{now()}'
    try:
        os.makedirs(dir_name, exist_ok=True)
        if verbose:
            print(f"Work directory = {dir_name}")
        return dir_name
    except OSError as e:
        print(f"Error creating directory {dir_name}: {e}")
        return None


def print_message(msg_type, msg, format_val=10):
    """
    Prints a formatted message based on the message type.
    """
    print_format = "%" * format_val
    if msg_type == 1:
        print("\n")
        print(print_format)
        print(f"@@ {msg}")
        print(print_format)
    else:
        print(msg)
    

def check_setup():
    """
    Checks if the SKFLAT_WD environment variable is set.
    Prompts the user to set up the SKFlatAnalyzer area if not set.
    Returns True if set, False otherwise.
    """
    skflat_wd = os.environ.get("SKFLAT_WD")
    if skflat_wd:
        return True
    else:
        print("Please set up the SKFlatAnalyzer area before running.")
        # Optionally, provide guidance on how to set the environment variable
        # For example:
        # print("Set SKFLAT_WD using 'export SKFLAT_WD=/path/to/skflat' in your shell.")
        return False


def ReadConfig(conf_var):
    """
    Reads the configuration file and returns a list of values associated with the given conf_var.
    """
    arr_conf = []
    CONFIG_File = 'config/config.txt'
    
    try:
        with open(CONFIG_File, "r") as _conf:
            for line in _conf:
                line = line.replace(',', '').replace('=', '')
                parts = line.split()
                
                if not parts:
                    continue
                
                section_name = parts[0].lower()
                if conf_var.lower() == "eras" and 'arr_eras' in section_name:
                    arr_conf.extend(parts[1:])
                elif conf_var.lower() == "miniaodversions" and 'miniaod_versions' in section_name:
                    arr_conf.extend(parts[1:])
                elif conf_var.lower() == "skflatversion" and 'skflatversion' in section_name:
                    return parts[1]
    except FileNotFoundError:
        print(f"Configuration file '{CONFIG_File}' not found.")
    except Exception as e:
        print(f"An error occurred while reading the config file: {e}")
    
    return arr_conf


def nevents(_era, _skoutput, _alias, _filename):
    root_file_path = os.path.join(_skoutput, "GetEffLumi", _era, f"GetEffLumi_{_alias}.root")
    if os.path.isfile(root_file_path):
        try:
            with ROOT.TFile(root_file_path, "READ") as _file:
                _hist = _file.Get("sumW")
                if _hist:
                    return int(_hist.GetEntries())
                else:
                    print(f"Histogram 'sumW' not found in {root_file_path}")
        except Exception as e:
            print(f"Error reading ROOT file {root_file_path}: {e}")
    else:
        nevent_tree1 = 0
        try:
            with open(_filename, "r") as _localfile:
                for line in _localfile:
                    file_path = line.split()[0]
                    if os.path.isfile(file_path):
                        try:
                            with ROOT.TFile(file_path, "READ") as _file:
                                _tree = _file.Get("recoTree/SKFlat")
                                if _tree:
                                    nevent_tree1 += _tree.GetEntries()
                                else:
                                    print(f"TTree 'recoTree/SKFlat' not found in {file_path}")
                        except Exception as e:
                            print(f"Error reading ROOT file {file_path}: {e}")
                    else:
                        print(f"File does not exist: {file_path}")
        except FileNotFoundError:
            print(f"File list {_filename} not found.")
        except Exception as e:
            print(f"Error reading file list {_filename}: {e}")
        return nevent_tree1



def get_url_from_era(_era, is_signal):
    """
    Depending on era, loop over directories in tamsa and add lines to Samples_MC.txt.
    """
    maod_versions = ["MiniAODv2"]
    urls = []
    for maod_version in maod_versions:
        gd_file_name = f'config/GoogleDocLink{"Signal" if is_signal else ""}{maod_version}.txt'
        try:
            with open(gd_file_name, "r") as gd_file:
                for line in gd_file:
                    parts = line.split()
                    if len(parts) == 2 and parts[0] == _era:
                        urls.append(parts[1])
        except FileNotFoundError:
            print(f"File not found: {gd_file_name}")
        except Exception as e:
            print(f"Error reading {gd_file_name}: {e}")
    return urls



def get_url_from_era_def(_era,is_signal,maod_version):

    """ depending on era loop over dir in tamsa and add line to Samples_MC.txt """                                                                                                                                                           
    var_format_val=10

    GD_File='config/GoogleDocLink'+maod_version+'.txt'
    if is_signal:
        GD_File='config/GoogleDocLinkSignal'+maod_version+'.txt'

    GD_File_READ = open (GD_File,"r")

    for line in GD_File_READ:
        if len(line.split()) ==2 :
            if line.split()[0] == _era:
                GD_File_READ.close()
                return line.split()[1]
    print ('Error in assigning GD page from era')
    return '-11111'




def get_array_from_googledoc(era, miniaod_versions, is_signal):
    """
    Retrieves and combines data from multiple Google Doc URLs based on era and signal status.
    """
    full_gd_array = []

    for maod_version in miniaod_versions:
        array_v = get_array_from_googledoc_def(era, is_signal, maod_version)
        full_gd_array.extend(array_v)  # Use extend to add all elements at once

    return full_gd_array


def get_array_from_googledoc_def(era, is_signal, maod_version):
    """
    Retrieves data from a specific Google Doc URL and processes it into two lists:
    arr_doc (status 'Y') and arr_docN (status 'N').
    """
    url = get_url_from_era_def(era, is_signal, maod_version)
    arr_doc = []
    arr_docN = []

    if url == "NULL":
        return arr_doc

    print(f'Reading {url}')
    
    try:
        with urlopen(url) as response:
            cr = csv.reader(response.read().decode('utf-8').splitlines())
            
            for row in cr:
                if row[0] == "END":
                    break
                
                # Directly check and append rows based on condition
                if row[1] and row[2] == "Y" and row[1] != "DS":
                    arr_doc.append(row)
                elif row[1] and row[2] == "N" and row[1] != "DS":
                    arr_docN.append(row)

    except Exception as e:
        print(f"Error reading {url}: {e}")

    # Output the counts of processed samples
    print(f"\nNumber of samples with process status 'Y' in GoogleDoc = {len(arr_doc)}")
    print(f"Number of samples with process status 'N' in GoogleDoc = {len(arr_docN)}")
    for x in arr_docN:
        print(x[0])

    return arr_doc


def n_directories(_dirname):
    """
    Returns the number of subdirectories in the specified directory.
    """
    subdirs_count = 0
    for root, dirs, files in os.walk(_dirname):
        subdirs_count += len(dirs)
    return subdirs_count


def find_googledoc_var_from_alias(_era, _varname, _alias, _urls, _urls_sig):
    """
    Search for a specific variable based on alias, era, and version from a list of URLs.
    """
    for url, url_sig in zip(_urls, _urls_sig):
        _var = find_googledoc_var_from_alias_def(_era, _varname, _alias, url, url_sig)
        
        if _var is not None:
            return _var

    raise ValueError(f"Variable for alias {_alias} not found in the provided URLs.")

def find_googledoc_var_from_alias_def(_era, _varname, _alias, _url, _url_sig):
    """
    Extract a variable from a Google Doc based on the given alias and version from the provided URLs.
    """
    var_index_var = 4 if _varname == "xsec" else 1
    _value = None

    # Process first URL
    try:
        with urlopen(_url) as response:
            cr = csv.reader(response.read().decode('utf-8').splitlines())
            for row in cr:
                if row[0] == "END":
                    break
                if row[1] and row[1] != "DS" and row[0] == _alias:
                    _value = row[var_index_var]
                    break
    except Exception as e:
        print(f"Error processing URL {_url}: {e}")

    # If not found, check the second URL
    if _value is None:
        try:
            with urlopen(_url_sig) as response_sig:
                cr2 = csv.reader(response_sig.read().decode('utf-8').splitlines())
                for row in cr2:
                    if row[0] == "END":
                        break
                    if row[1] and row[1] != "DS" and row[2] == "Y" and row[0] == _alias:
                        _value = row[var_index_var]
                        break
        except Exception as e:
            print(f"Error processing URL {_url_sig}: {e}")

    return _value



def run_skim_from_googledoc(_era,_alias , _skim,_urls):

    for x in _urls:
        
        if not run_skim_from_googledoc_def(_era,_alias , _skim,x) == "NULL":
            return run_skim_from_googledoc_def(_era,_alias , _skim,x)
        

def run_skim_from_googledoc_def(_era, _alias, _skim, _url):
    skim_value = "NULL"
    skim_columns = [10, 11, 12]  # Columns to check for the skim value

    try:
        with urlopen(_url) as response:
            # Decode the byte data to a string and split into lines
            lines = response.read().decode('utf-8').splitlines()
            cr = csv.reader(lines)
            for row in cr:
                if row[0] == "END":
                    break
                if row[1] and row[1] != "DS":
                    if row[0].split()[0] == _alias and row[2] == "Y":
                        for col in skim_columns:
                            if row[col].split()[0] == _skim:
                                skim_value = row[col].split()[0]
                                break
                        if skim_value != "NULL":
                            break
    except Exception as e:
        print(f"Error processing URL {_url}: {e}")

    return skim_value


def CheckMultipleTimeStamps(sk_path):

    from os import listdir
    from os.path import isfile,isdir, join
    arr_tamsadir  = [f for f in listdir(sk_path) if isdir(join(sk_path,f))]

    print(sk_path)
    
    for _dir in arr_tamsadir:
        
        sk_path_mc=sk_path+_dir
        arr_tamsadir_ts  = [f for f in listdir(sk_path_mc) if isdir(join(sk_path_mc,f))]
        
        if len(arr_tamsadir_ts) > 1:
            list_ts=''
            for x in arr_tamsadir_ts:
                list_ts=list_ts+x +', '
            print('Multiple timestamps for ' + sk_path_mc + '  [ ' + list_ts+' ] ')

def GetTimeStamp(sk_path,_dsn):

    from os import listdir
    from os.path import isfile,isdir, join
    arr_tamsadir  = [f for f in listdir(sk_path) if isdir(join(sk_path,f))]

    for _dir in arr_tamsadir:
        if not _dsn == _dir:
            continue
        
        sk_path_mc=sk_path+_dir
        arr_tamsadir_ts  = [f for f in listdir(sk_path_mc) if isdir(join(sk_path_mc,f))]
        
        if len(arr_tamsadir_ts) == 1:
            if arr_tamsadir_ts[0] == "SKFlat_Run2UltraLegacy_v3":
                sk_path_mc=sk_path+_dir+'/SKFlat_Run2UltraLegacy_v3/'
                arr_tamsadir_ts  = [f for f in listdir(sk_path_mc) if isdir(join(sk_path_mc,f))]

        
        if len(arr_tamsadir_ts) == 1:
            return arr_tamsadir_ts[0]

        elif len(arr_tamsadir_ts) > 1:
            return arr_tamsadir_ts[0]


def MissingSamples(array_gd,array_gd_sig,sk_path,_era):

    
    missing = []
    wrong_assign=[]
    dsn_list=[]
    for row in array_gd:
        if row[0] == "END":
            break

        if not row[1] == "":
            dsn_list.append(row[1])

        if not row[1] == "" and row[2] == "Y":
            if not os.path.exists(sk_path+"/"+row[1]):
                wrong_assign.append(row[0])
        else:
            if not os.path.exists(sk_path+"/"+row[1]):
                missing.append(row[0])

    for row in array_gd_sig:
        if row[0] == "END":
            break

        if not row[1] == "":
            dsn_list.append(row[1])

        if not row[1] == "" and row[2] == "Y":
            if not os.path.exists(sk_path+"/"+row[1]):
                print("Wrong " + sk_path+"/"+row[1])
                wrong_assign.append(row[0])
        else:
            if not os.path.exists(sk_path+"/"+row[1]):
                missing.append(row[0])


    if len(wrong_assign)>0:
        print('Samples that are set Y for processed on GD but missing in tamsa include')
    for x in wrong_assign:
        print(x)
    if len(missing) > 0:
        print('Samples that are filled on GD but missing in tamsa include')
    for x in missing:
        print(x)


    from os import listdir
    from os.path import isfile,isdir, join
    arr_tamsadir  = [f for f in listdir(sk_path) if isdir(join(sk_path,f))]
    missing_dsn=[]
    for _dir in arr_tamsadir:
        _dir_tmp=_dir
        _dir_tmp.replace(sk_path+'/','')
        if not _dir_tmp in dsn_list:
            
            skip_dsn_list=open(os.getenv("SKFlat_WD")+"/script/SKFlatSampleList/config/DSN_Skip.txt","r")
            skip=False
            for line in skip_dsn_list:
                if "END" in line:
                    break
                if line.split()[1] == _dir_tmp and line.split()[0] == _era  :
                    skip=True
            skip_dsn_list.close()
            if not skip:
                print('Sample on tamsa missing in googledoc ' + _dir_tmp)
                missing_dsn.append(_dir_tmp)

    if len(missing_dsn)> 0:
        print ("Some samples not on google doc. Add following to config/DSN_Skip.txt OR add to googledoc")
            

    for x in missing_dsn:
        #skip_dsn_list=open(os.getenv("SKFlat_WD")+"/script/SKFlatSampleList/config/DSN_Skip.txt","r")
        #skip=False
        #for line in skip_dsn_list:
        #    if line.split()[1] == x:
        #        skip=True
        #skip_dsn_list.close()
        #if not skip:
        print('echo "'+ _era+ ' ' +  x + '"  >> config/DSN_Skip.txt')
    if len(missing_dsn)> 0:
        var_userinput= input ('If running on SM MC and these are signals you can ignore.... Should we continue [y/n]:')                                                              
        if var_userinput == "y":                                                                                                                        
            return 
        exit()


def AssignTimeStamp(sk_path,array_gd):
    
    array_gd_ts=[]
    for row in array_gd:
        if row[0] == "END":
            array_gd_ts.append(row)
            break
        if not row[1] == "":
            dsn=row[1]
            ts = GetTimeStamp(sk_path,dsn)
            row.append(ts)
            array_gd_ts.append(row)

        else:
            array_gd_ts.append(row)
                        
    return array_gd_ts
    
def CheckDuplicates(array_gd):
    
    alias_list=[]
    dsn_list=[]
    for row in array_gd:
        if row[0] == "END":
            break
        if not row[1] == "":
            if row[0] in alias_list:
                print('Alias ['+row[0]+'] is used twice OR wrong set as processed twice, fix')
                return True

            if row[1] in dsn_list:
                print('DATASET ['+row[1]+'] is used twice OR wrong set as processed twice, fix')
                return True
                                    
            alias_list.append(row[0])
            dsn_list.append(row[1])
    return False

def PrintSampleInfo(array_gd):

    print('Summary for SKFlat TAMSA samples')
    nwidth=254
    print('-'*nwidth)
    print('Alias'+' '*(30) + ' | '  + 'MAOD Version' + ' '*(3)  + ' | ' + 'xsec' + ' '*(11)+ ' | '  + 'DAS Sample [1]=MINIAODSIM' + ' '*(142)+ ' | ' + 'TimeStamp   |') 
    print('-'*nwidth)


    for row in array_gd:
        if row[0] == "END":
            break
        if row[1] == "":
            print('-'*nwidth)
        if len(row) < 14:
            row.append('-')
            
        if not row[1] == "DS" and not row[1] == "" and row[2] == "Y":
            dasname=row[6]
            dasname=dasname.replace('MINIAODSIM','[1]')
            print(row[0] + ' '*(35 - len(row[0])) + ' | ' + row[3] + ' '*(15 - len(row[3]))  + ' | ' + row[4] + ' '*(15 - len(row[4]))+ ' | ' + dasname + ' '*(165 - len(dasname))+ ' | ' + row[6])
    print('-'*nwidth)

def find_googledoc_from_dsn(array_gd,_era,_varname , _dsn):

    _value="NULL"

    for row in array_gd:
        if row[0] == "END":
            break
        if not row[1] == "DS" and not row[1] == "" and row[2] == "Y":
            if row[1] == _dsn:
                _value=row[0]
                
    return _value

def find_googledoc_var_from_dsn(array_gd,_era,_varname , _dsn):
    
    var_index_var=0

    if _varname == "xsec":
        var_index_var=4

    _value="NULL"

    for row in array_gd:
        if row[0] == "END":
            break
        if not row[1] == "DS" and not row[1] == "" and row[2] == "Y":
            if row[1] == _dsn:
                _value=row[var_index_var]
            elif row[1] == _dsn+'/':
                _value=row[var_index_var]
                print(('Fix '/' in DSN in google spreadsheet for ',dsn,' in era',era))

    return _value


def process_url(url):
    processed_dsn = set()
    try:
        with urlopen(url) as response:
            cr = csv.reader(response.read().decode('utf-8').splitlines())
            for row in cr:
                if row[0] == "END":
                    break
                if row[1] and row[1] != "DS":
                    processed_dsn.add(row[1])
    except Exception as e:
        print(f"Error processing URL {url}: {e}")
    return processed_dsn


def check_processed_status(_dsnlist,_era):

    print_message(1,"check_dsn_list_from_googledoc ["+_era+"]")

    _urls=get_url_from_era(_era,False)

    arr_googledoc_dsn=[]
    arr_notgoogledoc_dsn=[]
    for url in _urls:

        with urlopen(url) as response:
            cr = csv.reader(response.read().decode('utf-8').splitlines())
            for row in cr:
                if row[0] == "END":
                    break
                if not row[1] == "DS" and not row[1] == "" and row[2] == "Y":
                    arr_googledoc_dsn.append(row[1])
                if not row[1] == "DS" and not row[1] == "" and row[2] == "N":
                    arr_notgoogledoc_dsn.append(row[1])

    Incorrect=False
    for x in arr_googledoc_dsn:
        if not x in _dsnlist:
            print("check_processed_status: ["+_era+"]   " + x + " googledoc status incorrect")
            Incorrect=True
    for x in arr_notgoogledoc_dsn:
        if x in _dsnlist:
            print("check_processed_status2: ["+_era+"]   " + x + " googledoc status incorrect")
            Incorrect=True


    return Incorrect


def check_processed_status2(_dsnlist,_era):
    
    print_message(1,"check_dsn_list_from_googledoc ["+_era+"]")

    _urls=get_url_from_era(_era,False)

    arr_googledoc_dsn = set()
    arr_notgoogledoc_dsn = set()
    for url in _urls:
        processed_dsn = process_url(url)
        arr_googledoc_dsn.update(processed_dsn)
        arr_notgoogledoc_dsn.update(processed_dsn)

    Incorrect = False
    _dsnset = set(_dsnlist)
    for dsn in arr_googledoc_dsn:
        if dsn not in _dsnset:
            print(f"check_processed_status: [{_era}] {dsn} googledoc status incorrect")
            Incorrect = True
    for dsn in arr_notgoogledoc_dsn:
        if dsn in _dsnset:
            print(f"check_processed_status2: [{_era}] {dsn} googledoc status incorrect")
            Incorrect = True

    return Incorrect
    



def make_common_sampleinfo(array_from_googledoc,_era, _dirlist, _outfiledir,data_skoutput, VERBOSE):

    print_message(1,"make_commonsample_file ["+_era+"]")

    for DSN in _dirlist:
        
        array_gd = array_from_googledoc

        var_alias = find_googledoc_var_from_dsn(array_gd,_era, "alias", DSN)
        var_xsec  = find_googledoc_var_from_dsn(array_gd,_era, "xsec" , DSN)
        
        if var_alias == "NULL":
            if VERBOSE:
                print("skipping " + DSN)
            continue

        if os.path.exists(_outfiledir+"/"+var_alias+".txt"):
            #print "make_common_sampleinfo: "+_outfiledir+"/"+var_alias+".txt already exists..."  
            continue

        print_message(2,"make_commonsample_file: processing  " + _outfiledir+"/"+var_alias+".txt")

        commonfile= open(_outfiledir+"/"+ var_alias+".txt","w")
        commonfile.write("# alias PD xsec nmc sumsign sumw \n")
        commonfile.write(var_alias + "\t" + DSN + "\t" + var_xsec +   "\t" + "1." + "\t" + "1."  + "\t" + "1.\n" )
        commonfile.close()
        print("make_common_sampleinfo: added file " + _outfiledir+"/"+ var_alias+".txt")
        
        # Check If old Eff lumi file is present and delete 

        if os.path.exists(data_skoutput+"/"+ var_alias +".txt"):
            print(('rm ' + data_skoutput+"/"+ var_alias +".txt"))
            os.system('rm ' + data_skoutput+"/"+ var_alias +".txt")



def update_summarymc_file(_era):

    import subprocess
    """
    Executes the Summarize.py script within the specified SKFlat version and era.
    """
    print_message(1, "update_summarymc_file ")

    # Retrieve environment variables
    var_skflat_wd = os.getenv("SKFlat_WD")
    if not var_skflat_wd:
        raise EnvironmentError("SKFlat_WD environment variable is not set.")

    flat_version = ReadConfig("SKFlatVersion")
    if not flat_version:
        raise ValueError("Failed to read SKFlatVersion from configuration.")

    # Construct the working directory path
    work_dir = os.path.join(var_skflat_wd, "data", flat_version, _era, "Sample")
    if not os.path.isdir(work_dir):
        raise FileNotFoundError(f"The directory {work_dir} does not exist.")

    # Define the command to execute
    command = [sys.executable, "Summarize.py"]

    # Execute the command within the specified working directory
    result = subprocess.run(command, cwd=work_dir, capture_output=True, text=True)

    # Check for errors
    if result.returncode != 0:
        print(f"Error executing Summarize.py:\n{result.stderr}")
    else:
        print(f"Summarize.py executed successfully:\n{result.stdout}")

            


def check_ds_in_current_list(_dslist, _varname):

    b_to_update=True

    for l in _dslist:
        _var=l.split()[1]
        if _var == _varname:
            b_to_update=False
    if b_to_update:
        print("check_ds_in_current_list:DSN " + _varname +" not in GD")
        for x in _dslist:
            print(x)
        
    return b_to_update

def check_xsec_in_current_list( _dslist, _dsn, _varname):

    b_to_update=True

    for l in _dslist:
        _var_dsn=l.split()[1]
        _var_xsec=l.split()[2]
        if _dsn == _var_dsn:
            if _var_xsec == _varname:
                b_to_update=False

    if b_to_update: 
        print("check_xsec_in_current_list: xsec not correct")
    

    return b_to_update


def update_mc_samplelist_from_googledoc(array_from_googledoc,_era, commonpath,samplecheck, _dirlist, _path_mc_outfile, _path_sklat_dir,work_dir):
    
    update_list=[]
  
    """ depending on era loop over dir in tamsa and add line to Samples_MC.txt"""
    print_message(1,"update_mc_samplelist_from_googledoc: [Start]")

    """ fill arr_copy_file with lines in local MC summary file"""
    arr_copy_file=[]
   
    ToUpdate=False
    r_mc_outfile=open(_path_mc_outfile,"r")
    for line in r_mc_outfile:
        arr_copy_file.append(line)
    r_mc_outfile.close()
    
    """ setup w_outfile to fill with updated list"""
    _tmp_path_mc_outfile="tmp_summary_"+_era+".txt"
    w_mc_outfile = open(_tmp_path_mc_outfile, "w")
    w_mc_outfile.write('# alias PD xsec nmc sumsign sumw \n')

    """ loop over dataset list on tamsa"""

    for dsn in _dirlist:
 
        #""" get alias xsec from googledoc"""

        array_gd = array_from_googledoc

        var_alias = find_googledoc_var_from_dsn(array_gd,_era,"alias", dsn)
        var_xsec  = find_googledoc_var_from_dsn(array_gd,_era,"xsec" , dsn)

        if var_alias == "NULL" :
            continue

        if n_directories(_path_sklat_dir+_era+"/MC/"+dsn) == 0:
            print("update_mc_samplelist_from_googledoc ["+_era+"] directory "+ dsn + " is missing.") 
            continue
        if var_alias == "NULL":
            print("Dirname " + dsn + " has no match from google doc")
            #print (array_from_googledoc,array_from_googledoc_sig,_era, _dirlist, _path_mc_outfile, _path_sklat_dir,work_dir)
            print(('DSN= ',dsn))
            print(( _path_mc_outfile, _path_sklat_dir,work_dir))  
            
            skip_dsn_list=open(os.getenv("SKFlat_WD")+"/script/SKFlatSampleList/config/DSN_Skip.txt","r")
            skip=False
            for line in skip_dsn_list:
                if line.split()[1] == dsn and line.split()[0] == _era:
                    skip=True
            skip_dsn_list.close()

            if not skip:
                print ('Add sample to condif/DSNSkip.txt OR add to spreadsheet')
                exit()
            else:
                continue

        if var_xsec == "NULL":
            print("xsec not found")
            exit()


        _nevents_no_w="1."
        _nevents_sign="1."
        _nevents_w="1."
        sample_exists=False
        for mcline in arr_copy_file:
            if var_alias == mcline.split()[0]:
                sample_exists=True
                w_mc_outfile.write(mcline)

                _nevents_no_w=mcline.split()[3]
                _nevents_sign=mcline.split()[4]
                _nevents_w=mcline.split()[5]
                mc_xsec = mcline.split()[2]
                if not mc_xsec.split()[0]  == var_xsec.split()[0]:
                    if os.path.exists(commonpath + "/"+var_alias+".txt"):
                        os.system("rm " + commonpath + "/"+var_alias+".txt")
                        print(("rm " + commonpath + "/"+var_alias+".txt"))
                    if os.path.exists(samplecheck + "/"+var_alias+".txt"):
                        os.system("rm " + samplecheck + "/"+var_alias+".txt")
                        print(("rm " + samplecheck + "/"+var_alias+".txt"))

                    print("Change in xsec for " +var_alias + " -->"  + mc_xsec+ ": " + var_xsec.split()[0]+":")
                    ToUpdate=True
                    update_list.append(dsn)
                    
        line_to_file = var_alias + "\t"  + dsn + "\t"+ var_xsec + "\t"+_nevents_no_w+"\t"+_nevents_sign+"\t"+_nevents_w+"\n"
        if not sample_exists:
            w_mc_outfile.write(line_to_file)
            update_list.append(dsn)

            ToUpdate=True

    w_mc_outfile.close()



    os.system("bash script/diff_script_mcfile.sh "+_tmp_path_mc_outfile + " " +_path_mc_outfile+" " + work_dir)

    if  os.path.getsize(work_dir+"/tmp.txt") > 1:
        var_flag= "NULL"
        ToUpdate=True
                    
        #arr_answers = ['y','n']
        #while not  var_flag in arr_answers:
        print('Replace ' + _path_mc_outfile + ' with updated file')
        #var_userinput= raw_input ('Replace ' + _path_mc_outfile + ' with updated file [y/n]:')
            #var_flag=var_userinput
            #print var_userinput
            #if var_userinput == "y":
        os.system('mv ' + _tmp_path_mc_outfile + ' '  + _path_mc_outfile )
        print("Updated...... " +_path_mc_outfile)
            #else:
            #    print "Did not update " + _path_mc_outfile
    os.system("rm "+work_dir+"/tmp.txt")

    if len(update_list) == 0:
        print ('No new samples to process.....')
    print_message(1,"update_mc_samplelist_from_googledoc: [end]")

    return update_list
    

def check_bad_files(badfile_dir,_era, _dir, RunFull):
    
    #if not "mix" in _dir or "WG" in _dir:
    #    return
    if not RunFull:
        return 
    print_message(1,"check_bad_files ["+_era+"] ["+ _dir +"]")
    currentdir = os.getenv("PWD")
    os.chdir(badfile_dir)
    os.system('python  runTAMSA.py -e ' + _era + ' -d ' + _dir)
    os.system("chmod u+x  summary.sh")
    os.system('./summary.sh')
    os.system('rm summary.sh')
    os.chdir(currentdir)
    

def CopyCommonSampleFiles(_era,commonpath, geteff_path, vsummary,gd_list_wo_skipped):
    
    if len(vsummary) < 2:
        exit()

    _path_mc_outfile=vsummary[0]
    _path_type1_outfile=vsummary[1]

    currentdir = os.getenv("PWD")
    var_skflat_wd=os.getenv("SKFlat_WD")
    FLAT_Version= ReadConfig("SKFlatVersion")

    print("Running: python Summarize.py")
    # Change to the correct directory
    target_dir = os.path.join(var_skflat_wd, "data", FLAT_Version, _era, "Sample")

    # Execute the command within the specified working directory                                                                                             
    result = subprocess.run([sys.executable, "Summarize.py"], cwd=target_dir, capture_output=True, text=True)

    # Check for errors                                                                                                                                       
    if result.returncode != 0:
        print(f"Error executing Summarize.py:\n{result.stderr}")
    else:
        print(f"Summarize.py executed successfully:\n{result.stdout}")

    
    # Now cat the output file (assuming _path_mc_outfile is the full path or relative to currentdir)
    try:
        with open(_path_mc_outfile, 'r') as f:
            print(f"Contents of {_path_mc_outfile}:\n{f.read()}")
    except Exception as e:
        print(f"Error reading {_path_mc_outfile}: {e}")
        
    print(('Removing files in ' + geteff_path))

    from os import listdir
    from os.path import isfile,isdir, join

    if len([f for f in listdir(geteff_path) if isfile(join(geteff_path,f))]) > 0:
        os.system('rm ' + geteff_path + '/*.txt')

    alias_copy_file=[]
    _path_mc_outfile_tmp = "tmp.txt"
    w_mc_outfile=open(_path_mc_outfile_tmp,"w")
    r_mc_outfile=open(_path_mc_outfile,"r")
    for line in r_mc_outfile:
        if "#" in line:
            w_mc_outfile.write(line)
        else:
            
            alias=line.split()[0]
            if float(line.split()[3]) == 1. and float(line.split()[4]) == 1. and float(line.split()[5]) == 1. :
                print(("Removing sample " + alias+ " from " + _path_mc_outfile))
            else:
                alias_copy_file.append(alias)
                w_mc_outfile.write(line)
            
    r_mc_outfile.close()
    w_mc_outfile.close()


    _path_type1_outfile_tmp = "tmpType1.txt"
    w_type1_outfile=open(_path_type1_outfile_tmp,"w")
    r_type1_outfile=open(_path_type1_outfile,"r")
    for line in r_type1_outfile:
        if "#" in line:
            w_type1_outfile.write(line)
        elif len(line.split()) < 1:
            continue
        else:

            alias=line.split()[0]
            if float(line.split()[3]) == 1. and float(line.split()[4]) == 1. and float(line.split()[5]) == 1. :
                print(("Removing sample " + alias+ " from " + _path_type1_outfile))
            else:
                alias_copy_file.append(alias)
                w_type1_outfile.write(line)

    r_type1_outfile.close()


    os.system('mv ' + _path_mc_outfile_tmp + ' ' + _path_mc_outfile)
    print(('Updated ' + _path_mc_outfile))
    print('-'*40)


    os.system('mv ' + _path_type1_outfile_tmp + ' ' + _path_type1_outfile)
    print(('Updated ' + _path_type1_outfile))
    print('-'*40)



    forsnupath=commonpath
    forsnupath=forsnupath.replace('CommonSampleInfo','ForSNU')


    common_path  = [f for f in listdir(commonpath) if isfile(join(commonpath,f))]


    get_list  = [f for f in listdir(geteff_path) if isfile(join(geteff_path,f))]
    deleted_alias=[]

    for x in common_path:
        
        if is_data(x):
            continue
        alias_from_file=x
        alias_from_file=alias_from_file.replace('.txt','')

        if not alias_from_file in alias_copy_file:
            print(('Alias ' + alias_from_file + ' is not in ' + _path_mc_outfile))
            print(("removing from " + commonpath))
            print(('rm ' + commonpath + '/'+ x + '\n'))
            print(('rm ' + forsnupath + '/'+ x + '\n'))
            os.system('rm ' + commonpath + '/'+ x)
            os.system('rm ' + forsnupath + '/'+ x)
            deleted_alias.append(x)
            continue

        if not os.path.exists(forsnupath+"/"+x):
            print(('Alias ' + alias_from_file + ' is missing forsnu file'))
            print(('rm ' + commonpath + '/'+ x + '\n'))
            print(('rm ' + forsnupath + '/'+ x + '\n'))
            os.system('rm ' + commonpath + '/'+ x)
            os.system('rm ' + forsnupath + '/'+ x)
            deleted_alias.append(x)

            continue


        tmp_file=False
        open_common = open(commonpath+"/"+x, "r")
        for  line in open_common:
            code_seg="\t" + "1." + "\t" + "1."  + "\t" + "1." 
            if code_seg in line:
                tmp_file=True
        open_common.close()

        if tmp_file:
            print('File ' + commonpath+"/"+x  + ' is set to 1. as default \n')
            os.system('cat ' + commonpath+"/"+x)
            #l_userinput= raw_input ('rm ' + commonpath + '/'+ x +  '[y/n] ')
            #if l_userinput == "y":
            print(('rm ' + commonpath + '/'+ x + '\n'))
            print(('rm ' + forsnupath + '/'+ x + '\n'))
            os.system('rm ' + commonpath + '/'+ x)
            os.system('rm ' + forsnupath + '/'+ x)
            deleted_alias.append(x)
        else:
            
            if not x in get_list:
                print('cp ' + commonpath + '/'+ x  + ' ' + geteff_path)
                os.system('cp ' + commonpath + '/'+ x  + ' ' + geteff_path)


    forsnu_path  = [f for f in listdir(forsnupath) if isfile(join(forsnupath,f))]
    for x in forsnu_path:
        if "Skim" in x:
            continue
        if "skim" in x:
            continue
        if "nfs" in x:
            continue

        if is_data(x):
            continue
        if not os.path.exists(commonpath + '/'+ x ):
            print(("missing | " +commonpath + '/'+ x ))
            print(('rm ' + commonpath + '/'+ x + '\n'))
            print(('rm ' + forsnupath + '/'+ x + '\n'))
            os.system('rm ' + commonpath + '/'+ x)
            os.system('rm ' + forsnupath + '/'+ x)
            deleted_alias.append(x)

            continue

        nfiles=0
        readfile = open(forsnupath + '/'+ x,"r")
        for line in readfile:
            if ".root" in line:
                nfiles=nfiles+1
        readfile.close()
        
        if nfiles == 0:
            print(("Empty file " + forsnupath + '/'+ x))
            os.system("cat " + forsnupath + '/'+ x)
            print(('rm ' + commonpath + '/'+ x + '\n'))
            print(('rm ' + forsnupath + '/'+ x + '\n'))
            os.system('rm ' + commonpath + '/'+ x)
            os.system('rm ' + forsnupath + '/'+ x)
            deleted_alias.append(x)

            continue


            
    print ("Delete removed files from samplelist")
    for dfile in deleted_alias:
        if  os.path.exists(geteff_path+"/"+ dfile):
            os.system('rm ' + geteff_path+"/"+ dfile)

    alias_copy_file=[]
    _path_mc_outfile_tmp = "tmp2.txt"
    w_mc_outfile=open(_path_mc_outfile_tmp,"w")
    r_mc_outfile=open(_path_mc_outfile,"r")
    for line in r_mc_outfile:
        if "#" in line:
            w_mc_outfile.write(line)
        else:
            alias=line.split()[0]+'.txt'
            if not  alias in deleted_alias:
                w_mc_outfile.write(line)
            else:
                print(("Removing sample " + alias+ " from " + _path_mc_outfile))

    r_mc_outfile.close()
    w_mc_outfile.close()
    os.system('mv ' + _path_mc_outfile_tmp + ' ' + _path_mc_outfile)
    print((' '*40))

    print (geteff_path)
    os.system('ls ' + geteff_path)
    get_list2  = [f for f in listdir(geteff_path) if isfile(join(geteff_path,f))]

    print('-'*40)
    print ('List of samples already processed')
    for x in get_list2:
        print(x)
    print(''*40)
    print('-'*40)
    print ('List of samples to be processed')
    for x in deleted_alias:
        print(x)
    print('-'*40)


    print("Running: python Summarize.py")
    
    # Build the working directory path
    work_dir = os.path.join(var_skflat_wd, "data", FLAT_Version, _era, "Sample")
    
    # Run Summarize.py in the target directory
    result = subprocess.run([sys.executable, "Summarize.py"], cwd=work_dir, capture_output=True, text=True)
    
    # Handle the result
    if result.returncode != 0:
        print(f"Error running Summarize.py:\n{result.stderr}")
    else:
        print(f"Summarize.py output:\n{result.stdout}")
    
    # Now cat the output file (assuming _path_mc_outfile is the full path or relative to currentdir)
    try:
        with open(_path_mc_outfile, 'r') as f:
            print(f"Contents of {_path_mc_outfile}:\n{f.read()}")
    except Exception as e:
        print(f"Error reading {_path_mc_outfile}: {e}")



def get_effective_lumi(array_from_googledoc,_era,_skoutput ,data_skoutput, _skdatadir,_dirlist,_summary_path,skim_list, _workdir, RunFull, VERBOSE):

    """  fill file with all alias that has not been ran previously"""

    print_message(1,"get_effective_lumi ["+_era+"]")

    """ loop over dataset list on tamsa"""
    var_url = get_url_from_era(_era,False)

    arr_alias=[]
    arr_alias_torun=[]
    array_gd = array_from_googledoc

    for dsn in _dirlist:
        
        array_gd = array_from_googledoc


        var_alias = find_googledoc_var_from_dsn(array_gd,_era,"alias", dsn)

        if var_alias == "NULL" :
            if VERBOSE:
                print("Skipping NULL [get_effective_lumi] " +dsn)
            continue

        if not os.path.exists(data_skoutput+"/"+ var_alias +".txt"):
            print("get_effective_lumi: adding " + var_alias + " to processing list")
            arr_alias_torun.append(var_alias)

         
        arr_alias.append(var_alias)
    if os.path.exists( _workdir+"/MC"+_era+".txt"):
        os.system("rm " + _workdir +  "/MC"+_era+".txt")

    w_list=open(_workdir+"/MC"+_era+".txt","w")

    for x in arr_alias_torun:
        w_list.write(x.split()[0]+"\n")
    w_list.close()
    
    return_list=[]

    if len(arr_alias_torun) > 0:
        currentdir = os.getenv("PWD")
        print("SKFlat.py -a GetEffLumi -l "+currentdir+"/"+_workdir+"/MC"+_era+".txt -n 50  --nmax 300  -e "+_era)

        os.chdir(os.getenv("SKFlat_WD"))
        os.system("SKFlat.py -a GetEffLumi -l "+currentdir+"/"+_workdir+"/MC"+_era+".txt -n 50  --nmax 300  -e "+_era )
        for x in arr_alias_torun:
            print("SKFlat.py -a GetEffLumi -i "+x.split()[0] +" -n 50  --nmax 300  -e "+_era) 

        for skim in skim_list:
            new_list=open(currentdir+"/"+_workdir+"/MC"+_era+".txt","r")
            new_skimlist=open(currentdir+"/"+_workdir+"/MC_"+skim+"_"+_era+".txt","w")
            runSkim=False
            for l in new_list:
                l = l.split()[0]
                #allowed_inputs=['y','n']
                #l_userinput ='NULL'
                #while not l_userinput in allowed_inputs:
                #    l_userinput= raw_input ('Sample to update ['+l+']: make skim ' + skim + '  [y/n]:')

                
                if run_skim_from_googledoc(_era,l , skim,var_url) == "Y": #l_userinput == "y":
                    print('Sample to update ['+l+']: make skim ' + skim)
                    new_skimlist.write(l+'\n')
                    runSkim=True
                    return_list.append(find_googledoc_var_from_alias(_era, "dsn", l.split()[0],var_url))

            new_list.close()
            new_skimlist.close()
            if runSkim:
                os.system("SKFlat.py -a "+skim+" -l "+currentdir+"/"+_workdir+"/MC_"+skim+"_"+_era+".txt -n 100  --nmax 300  -e "+_era )
                
            
            os.system("rm "+currentdir+"/"+_workdir+"/MC_"+skim+"_"+_era+".txt")
        os.chdir(currentdir)
        
    else:
        print("get_effective_lumi: all samples proccessed previously")

    ''' delete job submittion file '''
    os.system("rm "+ _workdir+ "/MC"+_era+".txt")    

    
    ''' run over ds list at tamsa and fill common samplefile'''

    print ('Fill CommonSampleFiles')

    update_array=[]

    for dsn in _dirlist:
        ''' access alias and xsec fmor google doc'''

        array_gd = array_from_googledoc

        var_alias = find_googledoc_var_from_dsn(array_gd,_era,"alias", dsn)
        if var_alias == "NULL" :
            continue


        if not RunFull:
            if not var_alias in arr_alias_torun:
                print(("skipping " + var_alias + " since not running Full mode"))
                continue
                
        else:
            print('Filling for ' + dsn)

        var_xsec  = find_googledoc_var_from_dsn(array_gd,_era,"xsec" , dsn)

        #''' get nevents from GetEffLumi job'''

        dirpath   = _skoutput + "/GetEffLumi/"+_era + "/"
        _file     = ROOT.TFile(dirpath + "/GetEffLumi_"+ var_alias + ".root")
        hist      = _file.Get("sumW")
        nevents_w = hist.Integral()
        signhist      = _file.Get("sumSign")
        nevents_sign = signhist.Integral()
        _file.Close()
        nevents_no_w=0
        
        orig_xsec=""
        orig_nevent_no_w=""
        orig_nevent_sign=""
        orig_nevent_w=""

        
        print("Reading  : " + dsn)
        print("Reading  " + _skdatadir + _era+ "/Sample/CommonSampleInfo/"+var_alias+".txt")

        orig_common_list = open(_skdatadir + _era+ "/Sample/CommonSampleInfo/"+var_alias+".txt","r")

        for line in orig_common_list:
            if not "#" in line:
                if len(line.split()) < 1:
                    continue
                orig_xsec=line.split()[2]
                orig_nevent_no_w=line.split()[3]
                orig_nevent_sign=line.split()[4]
                orig_nevent_w=line.split()[5]
        orig_common_list.close()

        print ('Filled for original values')
        update_file=False

        if not orig_xsec == var_xsec:
            update_file=True
            print("CommonSampleInfo xsec updated for " + var_alias)

            #if not orig_nevent_no_w==str(nevents_no_w):
            #update_file=True
            #print "CommonSampleInfo xsec updated for nevents_no_w " + str(nevents_no_w)
        if not float(orig_nevent_w)==nevents_w:
            update_file=True
            nevents_no_w=nevents(_era,_skoutput, var_alias,_skdatadir + _era+ "/Sample/ForSNU/"+var_alias + ".txt")
            print("CommonSampleInfo updated for nevents_w " + str(nevents_w))

        elif not float(orig_nevent_sign)==nevents_sign:
            update_file=True
            nevents_no_w=nevents(_era,_skoutput, var_alias,_skdatadir + _era+ "/Sample/ForSNU/"+var_alias + ".txt")
            print("CommonSampleInfo updated for nevents_sign " + str(nevents_sign))
        else: 
            nevents_no_w=orig_nevent_no_w

        if update_file:
            ''' make commonfile for alias'''
            common_list=open(_skdatadir + _era+ "/Sample/CommonSampleInfo/"+var_alias+".txt","w")
            common_list.write("# alias PD xsec nmc sumsign sumw\n")
            common_list.write( var_alias + "\t" + dsn + "\t" + var_xsec + "\t" + str(nevents_no_w) +"\t"+ str(nevents_sign) +"\t"+ str(nevents_w)+" \n")   


            common_list.close()
            os.system("git diff " + _skdatadir + _era+ "/Sample/CommonSampleInfo/"+var_alias+".txt")

            update_array.append([var_alias,var_xsec, nevents_no_w, nevents_sign , nevents_w])


    if len(update_array) > 0:
        update_summarymc_file(_era)

    return return_list

def find_timestamp(_path_to_samplelist , _timestamps):

    list_used = []
    for ts in _timestamps:
        filelist = open(_path_to_samplelist,"r")
        for rline in filelist:
            if ts in rline:
                list_used.append(ts)
                break
        filelist.close()
    return list_used

def CopyGetEff(def_dir, new_dir):
    
    from os import listdir
    from os.path import isfile,isdir, join
    arr_tmp_dir  = [f for f in listdir(def_dir) if isfile(join(def_dir,f))]
    for _dir in arr_tmp_dir:
        if not os.path.exists(new_dir+'/'+_dir):
            os.system('cp ' + def_dir + '/'+_dir + ' ' + new_dir+'/')
            print(('cp ' + def_dir + '/'+_dir + ' ' + new_dir+'/'))

def tmpDir(cdir):
    
    from os import listdir
    from os.path import isfile,isdir, join
    arr_tmp_dir  = [f for f in listdir(cdir) if isfile(join(cdir,f))]
    for _dir in arr_tmp_dir:
        if 'tmp' in _dir:
            return True
    return False

def check_samplefile_diff(orig_dir, updated_dir, geteff_filepath, checksample_path):
    
    from os import listdir
    from os.path import isfile,isdir, join
    arr_orig_dir  = [f for f in listdir(orig_dir) if isfile(join(orig_dir,f))]
    arr_updated_dir  = [f for f in listdir(updated_dir) if isfile(join(updated_dir,f))]

    import filecmp

    for x in arr_updated_dir:
        if not filecmp.cmp(orig_dir+'/'+x, updated_dir+'/'+x):
            alias_changed = x
            alias_changed = alias_changed.replace('.txt','')
            if os.path.exists(checksample_path + "/"+ alias_changed +".txt"):
                #var_userinput= raw_input ('Remove ' + checksample_path + '/GetEffLumi_'+ alias_changed +'.root since ForSNU file changed  [y/n]:')
                print('Remove ' + checksample_path + '/'+ alias_changed +'.txt since ForSNU file changed')
                #if var_userinput == "y" :

                os.system("rm " + geteff_filepath + "/GetEffLumi_"+ alias_changed +".root")
                os.system("rm " + checksample_path + "/"+ alias_changed +".txt")
                print("###############################"*2)
                print("removing " + checksample_path + "/"+ alias_changed +".")
                print("###############################"*2)
                
                
    
def check_file_diff(array_from_googledoc,_era, _dsn,  _path_to_skflat_mc, _path_to_samplefiles,_var_skflat_wd,_skoutput, work_dir, RunFull):
    
    from os import listdir
    from os.path import isfile,isdir, join
    from subprocess import check_output
    print_message(1,"check_file_diff ["+_era+"] [start]")

    """ loop over dataset list on tamsa"""
    #var_url = get_url_from_era(_era,False)
    
    path_mc_era=_path_to_skflat_mc+_era+"/MC/"
    arr_tamsadir  = [f for f in listdir(path_mc_era) if isdir(join(path_mc_era,f))]
    arr_snufiles  = [f for f in listdir(_path_to_samplefiles) if isfile(join(_path_to_samplefiles,f))]

    hasBadFile=False

    for _dir in _dsn:
        
        array_gd = array_from_googledoc

        var_alias = find_googledoc_var_from_dsn(array_gd,_era,"alias", _dir)
        if var_alias == "NULL" :
            continue

        runBadFile=False
        
        if var_alias+".txt" in arr_snufiles:
            path_mc_era_dir=path_mc_era+_dir
            arr_timestamp  = [f for f in listdir(path_mc_era_dir) if isdir(join(path_mc_era_dir,f))]
            timestamp=[]
            if len(arr_timestamp) > 0:
                #timestamp=arr_timestamp[0]
                timestamp=find_timestamp(_path_to_samplefiles+"/"+var_alias+".txt" , arr_timestamp)

            for _ts in timestamp:
                source = path_mc_era_dir+"/"+ _ts
                target = _path_to_samplefiles + var_alias+".txt"
                os.system("chmod u+x  script/diff_script.sh")
                print(("bash script/diff_script.sh " + target + " " + source+" " + var_alias+ " " + work_dir))
                os.system("bash script/diff_script.sh " + target + " " + source+" " + var_alias+ " " + work_dir)


            if  os.path.getsize(work_dir+"/tmp"+var_alias+".txt") > 1:
                print("check_file_diff: "+ var_alias+".txt has changed size... will rerun BadFileChecker and Effective luminosity") 

                runBadFile=True
            os.system("rm "+work_dir+"/tmp"+var_alias+".txt")
        else:
            print("check_file_diff: "+var_alias+".txt is new, will check for broken or empty files...")
            runBadFile=True
            
        
        if runBadFile:
            hasBadFile=True
            if os.path.exists(_skoutput+"/"+ var_alias +".txt"):
                _skoutput_eff_lumi=_skoutput
                _skoutput_eff_lumi=_skoutput_eff_lumi.replace('SampleCheck','GetEffLumi')
                os.system("rm " + _skoutput_eff_lumi+"/GetEffLumi_"+ var_alias +".root")
                
                os.system("rm " + _skoutput+"/"+ var_alias +".txt")
                print("###############################"*2)
                print("removing " + _skoutput+"/"+ var_alias +".txt")
                print("###############################"*2)

            check_bad_files(_var_skflat_wd+"/script/BadFileChecker/",_era, _dir, RunFull)                                                                                                                                                               

    if not hasBadFile:
        print('no bad/corrupt files found....')

    print_message(1,"check_file_diff ["+_era+"] [end]")
    return


def get_alias_perera(_era, dirlist, VERBOSE):

    arr_alias=[]
    var_url = get_url_from_era(_era,False)

    print("List of MC in ["+_era+"]")
    for x in dirlist:
        var_url = get_url_from_era(_era,False)
        if find_googledoc_var_from_dsn(_era,"alias", x, var_url) == "NULL":
            var_url = get_url_from_era(_era,True)

        var_alias = find_googledoc_var_from_dsn(_era,"alias", x, var_url)
        if var_alias == "NULL" :
            if VERBOSE:
                print("Skipping " +var_alias)
            continue

        arr_alias.append(var_alias)
    return arr_alias

def get_missing_alias_perera(_era, dirlist, skimname):

    print("get_missing_alias_perera: era = ["+_era+"]")
    arr_alias=[]
    var_url = get_url_from_era(_era,False)

    skim_dir="/gv0/DATA/SKFlat/Run2UltraLegacy_v3/2018/MC_"+ skimname
    if not os.path.exists(skim_dir):
        exit()


        
    from os import listdir
    from os.path import isfile,isdir, join
    arr_skimsamples  = [f for f in listdir(skim_dir) if isdir(join(skim_dir,f))]

    for _alias in dirlist:
        if not _alias in arr_skimsamples:
            print(_alias)



def print_alias_perera(_era, dirlist):

    var_url = get_url_from_era(_era,False)

    print("List of MC in ["+_era+"]")
    for x in dirlist:
        var_url = get_url_from_era(_era,False)
        if find_googledoc_var_from_dsn(_era,"alias", x, var_url) == "NULL":
            var_url = get_url_from_era(_era,True)

        var_alias = find_googledoc_var_from_dsn(_era,"alias", x, var_url)

        print(var_alias)
