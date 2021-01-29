#Inviwo Python script 
import inviwopy
import inviwopy.qt
import ivw.utils as inviwo_utils
import time
import csv

t0 = time.time()

app = inviwopy.app
network = app.network
data_folder = "C:/Users/sigsi52/Development/Inviwo/ElectronDensity/data/tq/"

# Read metadata file
f = open(data_folder + "metadata.csv", mode='r')
lines = f.readlines()
fileNames = []
for i in range(1, len(lines)):
    splitted = lines[i].strip().split(",")
    # State,Hole cube file,Particle cube file,Subgroups file,Type
    fileNames.append((splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]))

dataResult = []
nrSubgroups = 0
for file in fileNames:
    # lock
    app.network.lock()  

    # Subgroups
    sumChargeProcessor1 = network.SumChargeInSegmentedRegions
    sumChargeProcessor1.fileLocation.value = data_folder + file[3]
    sumChargeProcessor2 = network.SumChargeInSegmentedRegions2
    sumChargeProcessor2.fileLocation.value = data_folder + file[3]

    # Hole
    cubeLoader1 = network.CubeSource
    cubeLoader1.cube.value = data_folder + file[1]
    
    # Particle
    cubeLoader2 = network.CubeSource2
    cubeLoader2.cube.value = data_folder + file[2]

    # unlock
    app.network.unlock()

    # wait?
    
    inviwo_utils.update() # Needed for canvas to update

    # Result
    chargeTransferProcessor = network.ComputeChargeTransfer
        
    holeAndParticleCharges = chargeTransferProcessor.outports[2].getData()
    chargeDifference = chargeTransferProcessor.outports[0].getData()
    
    if holeAndParticleCharges == None or chargeDifference == None : 
        print("Error, no data in outport(s) (None), " + file[4] + ", " + file[0])
        exit()
    
    nrSubgroups = chargeDifference[1].size 

    row = []
    # Type
    row.append(file[4])
    # State
    row.append(file[0])
    for i in range(0, 2*nrSubgroups):
        row.append(holeAndParticleCharges[1].get(i)) 

    for j in range(0, nrSubgroups):
        row.append(chargeDifference[1].get(j))        

    dataResult.append(row)

holeNames = []
particleNames = []
diffNames = []
for k in range(0, nrSubgroups): 
    holeNames.append("Hole sg" + str(k+1))
    particleNames.append("Particle sg" + str(k+1))
    diffNames.append("Diff sg" + str(k+1))

header = []
header.append("Name")
header.append("State")
header.extend(holeNames)
header.extend(particleNames)
header.extend(diffNames)

with open(data_folder + 'results_withDifference2.csv', 'w', newline='') as resultsFile:
    writer = csv.writer(resultsFile)
    writer.writerow(header)
    writer.writerows(dataResult)

t1 = time.time()

print("Time:")
print(t1-t0)
