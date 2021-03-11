# Name: CreateDendrogram 

import inviwopy as ivw
import numpy as np

from matplotlib import pyplot as plt
from scipy.cluster.hierarchy import dendrogram
from sklearn.cluster import AgglomerativeClustering

class CreateDendrogram(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.dataFrame = df.DataFrameInport("dataFrame")
        self.addInport(self.dataFrame, owner=False)
        self.outport = df.DataFrameOutport("outport")
        self.addOutport(self.outport)

        self.featureVector = ivw.properties.OptionPropertyString(
            "feature_vector", "Feature vector", [ivw.properties.StringOption("reducedDims", "Reduced dimensions", "reducedDims"), 
                                                 ivw.properties.StringOption("holeAndParticle", "Hole and particle", "holeAndParticle"), 
                                                 ivw.properties.StringOption("chargeTransferMatrix", "Charge transfer matrix", "chargeTransferMatrix")])
        self.addProperty(self.featureVector)

        self.linkage = ivw.properties.OptionPropertyString(
            "linkage", "Linkage (Agglomerative)", [ivw.properties.StringOption("ward", "Ward", "ward"), 
                                                   ivw.properties.StringOption("complete", "Complete", "complete"), 
                                                   ivw.properties.StringOption("average", "Average", "average"),
                                                   ivw.properties.StringOption("single", "Single", "single")])
        self.addProperty(self.linkage)

        self.fileFormat = ivw.properties.OptionPropertyString(
            "fileFormat", "File format", [ivw.properties.StringOption(".png", ".png", ".png"), 
                                          ivw.properties.StringOption(".svg", ".svg", ".svg")])
        self.addProperty(self.fileFormat)
        
        self.threshold = ivw.properties.FloatProperty("threshold", "Threshold", 1.0, 0.0, 10.0, 0.05)
        self.addProperty(self.threshold)

        self.saveFileCheckbox = ivw.properties.BoolProperty("save_file", "Save file", False)
        self.addProperty(self.saveFileCheckbox)

        self.columnName = ivw.properties.StringProperty("column_name", "Column name", "Cluster")
        self.addProperty(self.columnName)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.CreateDendrogram", 
    		displayName = "CreateDendrogram",
    		category = "Python",
    		codeState = ivw.CodeState.Stable,
    		tags = ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return CreateDendrogram.processorInfo()

    def initializeResources(self):
        print("init")

    @staticmethod
    def plot_dendrogram(model, **kwargs):
        # Create linkage matrix and then plot the dendrogram

        # create the counts of samples under each node
        counts = np.zeros(model.children_.shape[0])
        n_samples = len(model.labels_)
        for i, merge in enumerate(model.children_):
            current_count = 0
            for child_idx in merge:
                if child_idx < n_samples:
                    current_count += 1  # leaf node
                else:
                    current_count += counts[child_idx - n_samples]
            counts[i] = current_count

        linkage_matrix = np.column_stack([model.children_, model.distances_,
                                      counts]).astype(float)
        
        # Plot the corresponding dendrogram
        dendrogram(linkage_matrix, **kwargs)

    def process(self):
        print("process")

        inputDataFrame = self.dataFrame.getData()

        
        # Get data from data frame
        X = []
        for i in range(0, inputDataFrame.rows):
            row = []
            for j in range(0, inputDataFrame.cols):
                header = inputDataFrame.column(j).header.lower()
                
                # Use reduced dimensions
                if self.featureVector.value == "reducedDims":
                    if (header=="1" or header=="2"):
                        row.append(inputDataFrame.column(j).get(i))

                # Use hole and particle subgroup charges
                elif self.featureVector.value == "holeAndParticle":
                    if ("hole" in header) or ("particle" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Use charge transfer matrix
                elif self.featureVector.value == "chargeTransferMatrix":
                    if ("transfer" in header):
                        row.append(inputDataFrame.column(j).get(i))

            X.append(row)

        X = np.array(X)
        print("Dimensions of X: " + str(X.shape))

        # setting distance_threshold=0 ensures we compute the full tree.
        model = AgglomerativeClustering(distance_threshold=self.threshold.value, n_clusters=None, linkage=self.linkage.value)
        
        model = model.fit(X)

        dataframe = df.DataFrame()
        dataframe.addFloatColumn(self.columnName.value, model.labels_)
        dataframe.updateIndex()

        self.outport.setData(dataframe)

        if (self.saveFileCheckbox.value == True):
            plt.clf()
            plt.title('Hierarchical Clustering Dendrogram')

            # plot the top p levels of the dendrogram
            #self.plot_dendrogram(model, truncate_mode='level', p=5)
            # plot the whole dendrogram
            self.plot_dendrogram(model, color_threshold=self.threshold.value)

            plt.tight_layout()
            plt.xlabel("Number of points in node (or index of point if no parenthesis).")


            plt.savefig("C:/Users/sigsi52/Development/Inviwo/ElectronDensity/data/results/plots/dendrogram_" + self.featureVector.value + "_" + self.linkage.value + self.fileFormat.value, bbox_inches='tight')
        
