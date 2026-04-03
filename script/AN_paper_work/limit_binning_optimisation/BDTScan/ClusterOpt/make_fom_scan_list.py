#masses = ['100','125','150','200', '250','300', '400', '500']
masses = ['300','350','400','450']
flavours = ['EE', 'MuMu', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

# Define bin ranges per mass
bin_ranges = {
    '100': range(10 , 26),
    '125': range(10 , 26),
    '150': range(10 , 26),
    '200': range(10 , 26),
    '250': range(10 , 26),
    '300': range(10 , 26),
    '350': range(10 , 26),
    '400': range(10 , 26),
    '450': range(10 , 26),
    '500': range(10 , 26),
}

with open("fom_scan_list.txt", "w") as f:
    for mass in masses:
        for flav in flavours:
            for era in eras:
                for min_bins in [25]:
                    for n_bins in bin_ranges[mass]:
                        if n_bins > min_bins:
                            continue
                        if n_bins == 15:
                            f.write(f"{mass} {flav} {era} {n_bins} {min_bins}\n")

print("[INFO] fom_scan_list.txt written with custom bin ranges per mass.")
