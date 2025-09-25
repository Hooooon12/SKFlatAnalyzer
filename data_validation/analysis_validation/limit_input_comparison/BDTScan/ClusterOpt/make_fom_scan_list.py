masses = ['100','125','150','200', '250','300', '400', '500']
flavours = ['EE', 'MuMu', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']


# Define bin ranges per mass
bin_ranges = {
    '100': range(1 , 6),
    '125': range(1 , 6),
    '150': range(1 , 6),
    '200': range(1 , 6),
    '250': range(1 , 6),
    '300': range(1 , 6),
    '400': range(1 , 6),
    '500': range(1 , 6),
}

with open("fom_scan_list.txt", "w") as f:
    for mass in masses:
        for flav in flavours:
            for era in eras:
                for min_bins in [5]:
                    for n_bins in bin_ranges[mass]:
                        if n_bins > min_bins:
                            continue
                        f.write(f"{mass} {flav} {era} {n_bins} {min_bins}\n")

print("[INFO] fom_scan_list.txt written with custom bin ranges per mass.")
