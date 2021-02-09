#Inviwo Python script 
#import inviwopy
import csv


data_folder = "C:/Users/sigsi52/Development/Inviwo/ElectronDensity/data/tq-pes/"
filenames = ['tq-0', 'tq-15', 'tq-30', 'tq-45', 'tq-60', 'tq-75', 'tq-90', 'tq-105', 'tq-120', 'tq-135', 'tq-150', 'tq-165', 'tq-180']

output = []
for k in range(0, len(filenames)):
    f = open(data_folder + filenames[k] + ".dat", mode='r')
    lines = f.readlines()
    # -1 to not use the 10th state!
    for i in range(0, len(lines)-1):
        output.append([(filenames[k] + '       ' + lines[i]).replace('"', '')])

with open(data_folder + 'allParameters_state1to9.dat', 'w', newline='') as resultsFile:
    writer = csv.writer(resultsFile)
    writer.writerows(output)