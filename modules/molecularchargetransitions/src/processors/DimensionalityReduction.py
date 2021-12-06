# Name: DimensionalityReduction 

import inviwopy as ivw
import ivwdataframe as df
import numpy as np
from sklearn.manifold import TSNE
from sklearn.manifold import MDS
from sklearn.manifold import Isomap
from sklearn.decomposition import PCA
from sklearn.decomposition import FastICA

class DimensionalityReduction(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = df.DataFrameInport("inport")
        self.addInport(self.inport, owner=False)
        self.outport = df.DataFrameOutport("outport")
        self.addOutport(self.outport)

        self.dimReduction = ivw.properties.OptionPropertyString(
            "dim_reduction", "Dim. red. technique", [ivw.properties.StringOption("tSNE", "t-SNE", "tSNE"), 
                                                     ivw.properties.StringOption("MDS", "MDS", "MDS"),
                                                     ivw.properties.StringOption("isomap", "Isomap", "isomap"),
                                                     ivw.properties.StringOption("pca", "PCA", "pca"),
                                                     ivw.properties.StringOption("ica", "ICA", "ica")])
        self.addProperty(self.dimReduction)
        self.perplexity = ivw.properties.FloatProperty(
            "perplexity", "Perplexity", 5.0, 5.0, 50.0, 1.0)
        self.addProperty(self.perplexity)
        self.learningRate = ivw.properties.FloatProperty(
            "learning_rate", "Learning rate", 200.0, 10.0, 1000.0, 10.0)
        self.addProperty(self.learningRate)
        self.nIter = ivw.properties.IntProperty(
            "n_iter", "Nr of iterations", 1000, 250, 10000, 50)
        self.addProperty(self.nIter)
        self.neighbors = ivw.properties.IntProperty(
            "neighbors", "Neighbors (Isomap)", 5, 3, 40, 1)
        self.addProperty(self.neighbors)
        self.useOscStrength = ivw.properties.BoolProperty(
            "use_osc_strength", "Use oscillator strength", False)
        self.addProperty(self.useOscStrength)
        self.useRotStrength = ivw.properties.BoolProperty(
            "use_rot_strenght", "Use rotatory strength", False)
        self.addProperty(self.useRotStrength)
        self.useEnergy = ivw.properties.BoolProperty(
        "use_energy", "Use energy", False)
        self.addProperty(self.useEnergy)

        self.featureVector = ivw.properties.OptionPropertyString(
            "feature_vector", "Feature vector", [ivw.properties.StringOption("holeAndParticle", "Hole and particle", "holeAndParticle"), 
                                                 ivw.properties.StringOption("difference", "Difference (particle - hole)", "difference"), 
                                                 ivw.properties.StringOption("chargeTransferMatrix", "Charge transfer matrix", "chargeTransferMatrix")])
        self.addProperty(self.featureVector)

        self.dim1name = ivw.properties.StringProperty("dim1_name", "Dim 1 name", "1")
        self.addProperty(self.dim1name)

        self.dim2name = ivw.properties.StringProperty("dim2_name", "Dim 2 name", "2")
        self.addProperty(self.dim2name)


    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.DimensionalityReduction", 
    		displayName = "Dimensionality Reduction",
    		category = "Python",
    		codeState = ivw.CodeState.Stable,
    		tags = ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return DimensionalityReduction.processorInfo()

    def initializeResources(self):
        print("init")

    def process(self):
        inputDataFrame = self.inport.getData()

        # Get data from data frame
        X = []
        for i in range(0, inputDataFrame.rows):
            row = []
            for j in range(0, inputDataFrame.cols):
                header = inputDataFrame.column(j).header.lower()

                # Use hole and particle subgroup charges
                if self.featureVector.value == "holeAndParticle":
                    if ("hole" in header) or ("particle" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Use difference (particle - hole) charges
                elif self.featureVector.value == "difference":
                    if ("diff" in header):
                         row.append(inputDataFrame.column(j).get(i))

                # Use charge transfer matrix
                elif self.featureVector.value == "chargeTransferMatrix":
                    if ("transfer" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Add oscillatory strength to feature vector
                if self.useOscStrength.value == True : 
                    if ("osc" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Add rotatory strength to feature vector
                if self.useRotStrength.value == True :
                    if ("rot" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Add energy to feature vector
                if self.useEnergy.value == True :
                    if ("energy" in header):
                        row.append(inputDataFrame.column(j).get(i))

            X.append(row)

        X = np.array(X)
        print("Dimensions before reduction: " + str(X.shape))

        # Dimensionality reduction
        if (self.dimReduction.value == "tSNE"):
            X_embedded = TSNE(n_components=2, perplexity=self.perplexity.value, 
                          learning_rate=self.learningRate.value, n_iter=self.nIter.value).fit_transform(X)
        elif (self.dimReduction.value == "MDS"):
            embedding = MDS(n_components=2)
            X_embedded = embedding.fit_transform(X)
        elif (self.dimReduction.value == "isomap"):
            embedding = Isomap(n_components=2, n_neighbors=self.neighbors.value)
            X_embedded = embedding.fit_transform(X)
        elif (self.dimReduction.value == "pca"):
            pca = PCA(n_components=2)
            pca.fit(X)
            X_embedded = pca.transform(X)
        elif (self.dimReduction.value == "ica"):
            transformer = FastICA(n_components=2)
            X_embedded = transformer.fit_transform(X)

        X_transposed = np.transpose(X_embedded)

        # Try to make it not "flip" with PCA...
        # Rotate the data
        #clf = PCA(n_components=2)
        #X_transposed = clf.fit_transform(X_transposed)

        dataframe = df.DataFrame()
        dataframe.addFloatColumn(self.dim1name.value, X_transposed[0])
        dataframe.addFloatColumn(self.dim2name.value, X_transposed[1])
        dataframe.updateIndex()

        self.outport.setData(dataframe)
