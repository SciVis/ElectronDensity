#Inviwo Python script 
#import inviwopy
import csv


data_folder = "C:/Users/sigsi52/Development/Inviwo/ElectronDensity/data/serie/"
#filenames = ['tq-0', 'tq-15', 'tq-30', 'tq-45', 'tq-60', 'tq-75', 'tq-90', 'tq-105', 'tq-120', 'tq-135', 'tq-150', 'tq-165', 'tq-180']
#filenames = ['A2B2-td', 'A3B-td', 'A4-td', 'ABAB-td']
filenames = ['ag-phe-ipr-td', 'ag-phe-pheme-td', 'ag-phe-pheome-td', 'ag-phe-phephe-td', 'ag-phe-xant-td', 'ag-phe2-td',
             'au-phe-ipr-td', 'au-phe-pheme-td', 'au-phe-pheome-td', 'au-phe-phephe-td', 'au-phe-xant-td', 'au-phe2-td',
             'cu-phe-ipr-td', 'cu-phe-pheme-td', 'cu-phe-pheome-td', 'cu-phe-phephe-td', 'cu-phe-xant-td', 'cu-phe2-td']

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