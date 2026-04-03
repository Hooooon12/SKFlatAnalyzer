masses = ['100','125','150','200', '250','300', '400', '500']
flavours = ['EE', 'MuMu', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']
n_maxiter_scan = 1500000

# Define bin ranges per mass
bin_ranges = {
    '100': range(8, 20),
    '125': range(8, 20),
    '150': range(8, 20),
    '200': range(8, 20),
    '250': range(8, 20),
    '300': range(6, 18),
    '400': range(6, 15),
    '500': range(6, 15),
}

with open("fom_scan_list.txt", "w") as f:
    for mass in masses:
        for flav in flavours:
            for era in eras:
                for n_bins in bin_ranges[mass]:
                    f.write(f"{mass} {flav} {era} {n_bins} {n_maxiter_scan}\n")

print("[INFO] fom_scan_list.txt written with custom bin ranges per mass.")
