#Inviwo Python script 
import inviwopy
import inviwopy.qt
import ivw.utils as inviwo_utils
import time
import csv

t0 = time.time()

app = inviwopy.app
network = app.network
data_folder = "C:/Users/sigsi52/Development/Inviwo/ElectronDensity/data/cpp/cpp_ABAB/"

# Read metadata file
f = open(data_folder + "metadata.csv", mode='r')
lines = f.readlines()
fileNames = []
for i in range(1, len(lines)):
    splitted = lines[i].strip().split(",")
    fileNames.append((splitted[0], splitted[1], splitted[2]))

# Subgroup information
sumChargeProcessor1 = network.SumChargeInSegmentedRegions
sumChargeProcessor1.fileLocation.value = data_folder + "subgroups.json"
sumChargeProcessor2 = network.SumChargeInSegmentedRegions2
sumChargeProcessor2.fileLocation.value = data_folder + "subgroups.json"

dataResult = []
for file in fileNames:
    #print("File: " + file[0])
    
    # Hole
    cubeLoader1 = network.CubeSource
    cubeLoader1.cube.value = data_folder + file[1]
    
    # Particle
    cubeLoader2 = network.CubeSource2
    cubeLoader2.cube.value = data_folder + file[2]

    inviwo_utils.update() # Needed for canvas to update
    
    # Result
    chargeTransferProcessor = network.ComputeChargeTransfer
    holeAndParticleCharges = chargeTransferProcessor.outports[2].getData()
    
    row = []
    for it in range(0, holeAndParticleCharges[1].size):
        row.append(holeAndParticleCharges[1].get(it)) 

    dataResult.append(row)

#help('inviwopy')

with open(data_folder + 'results.csv', 'w', newline='') as resultsFile:
    writer = csv.writer(resultsFile)
    writer.writerows(dataResult)

t1 = time.time()

print("Time:")
print(t1-t0)
