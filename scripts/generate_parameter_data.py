#Inviwo Python script 
#import inviwopy
import csv


data_folder = "C:/Users/sigsi52/Development/Inviwo/ElectronDensity/data/silver-complexes/"
#filenames = ['tq-0', 'tq-15', 'tq-30', 'tq-45', 'tq-60', 'tq-75', 'tq-90', 'tq-105', 'tq-120', 'tq-135', 'tq-150', 'tq-165', 'tq-180']
#filenames = ['A2B2-td', 'A3B-td', 'A4-td', 'ABAB-td']
#filenames = ['ag-phe-ipr-td', 'ag-phe-pheme-td', 'ag-phe-pheome-td', 'ag-phe-phephe-td', 'ag-phe-xant-td', 'ag-phe2-td',
#             'au-phe-ipr-td', 'au-phe-pheme-td', 'au-phe-pheome-td', 'au-phe-phephe-td', 'au-phe-xant-td', 'au-phe2-td',
#             'cu-phe-ipr-td', 'cu-phe-pheme-td', 'cu-phe-pheome-td', 'cu-phe-phephe-td', 'cu-phe-xant-td', 'cu-phe2-td']
#filenames = ['cu-h-me3-p', 'cu-h-me3', 'cu-h2-me2-p', 'cu-h2-me2', 'cu-h3-me-p', 'cu-h3-me', 'cu-h4', 'cu-me4-p', 'cu-me4']
#filenames = ['A2B2', 'A3B', 'AAAA', 'AB3', 'ABAB', 'BBBB']
# OBS! This order is not as in the file structure, but as the metadata-file
filenames = ['0/td-0', 'Minimum1_linked/ag-ipr-dpa-linked-td', '15/td-15', \
             '30/td-30', '45/td-45', '60/td-60', '75/td-75', '90/td-90', \
             '105/td-105', '120/td-120', 'Minimum2_unlinked/ag-ipr-dpa-unlinked-td', \
             '135/td-135', '150/td-150', '165/td-165', '180/td-180','195/td-195', \
             '210/td-210', 'unlinked2/ag-ipr-dpa-unlinked2-td', '225/td-225', \
             '240/td-240', '255/td-255', '270/td-270', '285/td-285', '300/td-300', \
             '315/td-315', '330/td-330', '345/td-345', '360/td-360']

output = []
for k in range(0, len(filenames)):
    f = open(data_folder + filenames[k] + ".dat", mode='r')
    lines = f.readlines()
    # -1 to not use the 10th (if last) state!
    for i in range(0, len(lines)):
        output.append([(filenames[k] + '       ' + lines[i]).replace('"', '')])

with open(data_folder + 'allParameters.dat', 'w', newline='') as resultsFile:
    writer = csv.writer(resultsFile)
    writer.writerows(output)