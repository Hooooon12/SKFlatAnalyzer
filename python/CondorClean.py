import argparse
import os,subprocess

os.system("condor_q  " + os.getenv("USER") + " > log")


parser = argparse.ArgumentParser(description='CR plot commands')
parser.add_argument('-f', dest='Flag', type=str, default='NULL')
parser.add_argument('--Run',action='store_true')

args = parser.parse_args()

user = os.getenv("USER")

slist = open("log","r")

for line in slist:
    if args.Flag in line:
        JobID= line.split()[8]
        JobID=JobID.replace('.',' ')
        JobID= JobID.split()[0]
        if not args.Run:
              print line.split()[1]+ " --> condor_rm "+user+" "+JobID
        else:
            os.system("condor_rm  " +  JobID)
            
slist.close()


os.system("rm log")
