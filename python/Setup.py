import os

current_dir=os.getenv("PWD")
if not os.path.exists(current_dir+"/HNDiLeptonWorskspace"):
    print ("git clone git@github.com:jalmond/HNDiLeptonWorskspace.git")
    #os.system("git clone git@github.com:jalmond/HNDiLeptonWorskspace.git")
    
    
    
git_file = open (".gitignore","r")
readfile = git_file.read()
git_file.close()
if not  "HNDiLeptonWorskspace" in readfile:
    print  ("Adding HNDiLeptonWorskspace to git ignore")
    with open(".gitignore","a") as myfile:
        myfile.write("\nHNDiLeptonWorskspace")
        
print ("Setup HNDiLeptonWorskspace")
os.system("bash HNDiLeptonWorskspace/setup.sh")
