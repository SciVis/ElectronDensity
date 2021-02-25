# Name: CreateAverageChargeTransferDiagram 

import inviwopy as ivw
import ivwdataframe as df
from electrans import transition_analysis_utils as tau
import types

class CreateAverageChargeTransferDiagram(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.dataFrame = df.DataFrameInport("dataFrame")
        self.addInport(self.dataFrame, owner=False)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.CreateAverageChargeTransferDiagram", 
    		displayName = "CreateAverageChargeTransferDiagram",
    		category = "Python",
    		codeState = ivw.CodeState.Stable,
    		tags = ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return CreateAverageChargeTransferDiagram.processorInfo()

    def initializeResources(self):
        print("init")

    def process(self):
        print("process")
        inputDataFrame = self.dataFrame.getData()

        # Get data from data frame
        holeCharges = []
        particleCharges = []
        clusters = []
        for i in range(0, inputDataFrame.rows):
            meanHole = []
            meanParticle = []
            for j in range(0, inputDataFrame.cols):
                header = inputDataFrame.column(j).header.lower()
                if ("mean hole" in header):
                    meanHole.append(inputDataFrame.column(j).get(i))
                if ("mean particle" in header):
                    meanParticle.append(inputDataFrame.column(j).get(i))
                if (header == "cluster"):
                    clusters.append(inputDataFrame.column(j).get(i))
            holeCharges.append(meanHole)
            particleCharges.append(meanParticle)

        #print("len holeCharges", len(holeCharges))
        #print("len particleCharges", len(particleCharges))

        for k in range(0, len(holeCharges)):
            #print("len holeCharges[k]", len(holeCharges[k]))
            subgroup_names = range(1, len(holeCharges[k])+1)

            atom_subgroup_map = range(len(holeCharges[k]))
            subgroup_info = tau.SubgroupInfo()
            subgroup_info.set_subgroups(subgroup_names, atom_subgroup_map)

            transition = types.SimpleNamespace()
            transition.hole_charges =  holeCharges[k]
            transition.particle_charges = particleCharges[k]

            tau.compute_subgroup_charges(transition, subgroup_info)

            tau.create_diagram(subgroup_info, title="Mean, cluster " + str(clusters[k]), show_plot=False, save_plot=True, file_name="C:/Users/sigsi52/Development/Inviwo/ElectronDensity/data/results/plots/meanChargeTransferPlot" + str(clusters[k]) + ".png")
            print(subgroup_info)
