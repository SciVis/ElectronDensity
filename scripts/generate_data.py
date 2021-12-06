#Inviwo Python script 
import inviwopy
import inviwopy.qt
import ivw.utils as inviwo_utils
import time
import csv

t0 = time.time()

app = inviwopy.app
network = app.network
data_folder = "C:/Users/sigsi52/Development/Inviwo/ElectronDensity/data/cpp-new/"

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
    # If should skip state...
    #if file[0] == "State 10":
    #    continue

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
    cubeLoader2 = network.CubeSource3 # CubeSource2
    cubeLoader2.cube.value = data_folder + file[2]

    # unlock
    app.network.unlock()
     
    #inviwo_utils.update() # Needed for canvas to update

    # To make sure processor (voronoi) is finished
    inviwopy.qt.update()    
    #app.waitForPool()
    while app.network.runningBackgroundJobs > 0:
        inviwopy.qt.update()

    # Result
    chargeTransferProcessor = network.ComputeChargeTransfer

    holeAndParticleCharges = chargeTransferProcessor.outports[2].getData()
    chargeDifference = chargeTransferProcessor.outports[0].getData()
    chargeTransfer = chargeTransferProcessor.outports[1].getData()

    if holeAndParticleCharges == None or chargeDifference == None : 
        print("Error, no data in outport(s) (None), " + file[4] + ", " + file[0])
        exit()
    
    nrSubgroups = chargeDifference[1].size 

    row = []
    # Type
    row.append(file[4])
    # State
    row.append(file[0])

    # Hole and particle charges
    for i in range(0, 2*nrSubgroups):
        row.append(holeAndParticleCharges[1].get(i)) 
    
    # Charge difference
    for j in range(0, nrSubgroups):
        row.append(chargeDifference[1].get(j))        
    
    # Charge transfer matrix (row-wise)
    for k in range(0, nrSubgroups):
        for l in range(1, nrSubgroups+1):
            row.append(chargeTransfer[l].get(k))
    
    dataResult.append(row)

holeNames = []
particleNames = []
diffNames = []
chargeTransferNames = []
for m in range(1, nrSubgroups+1): 
    holeNames.append("Hole sg" + str(m))
    particleNames.append("Particle sg" + str(m))
    diffNames.append("Delta q sg" + str(m))
    for ind1, ind2 in zip([m]*nrSubgroups, range(1,nrSubgroups+1)):
        chargeTransferNames.append("Charge transfer " + str(ind1) + str(ind2))

header = []
header.append("Name")
header.append("State")
header.extend(holeNames)
header.extend(particleNames)
header.extend(diffNames)
header.extend(chargeTransferNames)

with open(data_folder + 'results3.csv', 'w', newline='') as resultsFile:
    writer = csv.writer(resultsFile)
    writer.writerow(header)
    writer.writerows(dataResult)

t1 = time.time()

print("Time:")
print(t1-t0)
