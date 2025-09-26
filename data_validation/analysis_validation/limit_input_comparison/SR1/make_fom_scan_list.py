masses = ['400', '500','600','700','800','900','1000','2000']
flavours = ['EE', 'MuMu', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

# Define bin ranges per mass


with open("fom_scan_list.txt", "w") as f:
    for mass in masses:
        for flav in flavours:
            for era in eras:
                for nbin in ['6']:
                    f.write(f"{mass} {flav} {era} {nbin}\n")

print("[INFO] fom_scan_list.txt written with custom bin ranges per mass.")
