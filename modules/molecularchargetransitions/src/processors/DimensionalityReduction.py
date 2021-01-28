# Name: DimensionalityReduction 

import inviwopy as ivw
import ivwdataframe as df
import numpy as np
from sklearn.manifold import TSNE
from sklearn.manifold import MDS

class DimensionalityReduction(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = df.DataFrameInport("inport")
        self.addInport(self.inport, owner=False)
        self.outport = df.DataFrameOutport("outport")
        self.addOutport(self.outport)
        self.perplexity = ivw.properties.FloatProperty(
            "perplexity", "Perplexity", 5.0, 5.0, 50.0, 1.0)
        self.addProperty(self.perplexity)
        self.learningRate = ivw.properties.FloatProperty(
            "learning_rate", "Learning rate", 200.0, 10.0, 1000.0, 10.0)
        self.addProperty(self.learningRate)
        self.nIter = ivw.properties.IntProperty(
            "n_iter", "Nr of iterations", 1000, 250, 10000, 50)
        self.addProperty(self.nIter)
        self.useDifference = ivw.properties.BoolProperty(
            "use_difference", "Use subgroup difference", False)
        self.addProperty(self.useDifference)

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

        # Get data from dataFrame
        X = []
        for i in range(0, inputDataFrame.rows):
            row = []
            for j in range(0, inputDataFrame.cols):
                header = inputDataFrame.column(j).header.lower()

                # Use hole and particle subgroup charges
                if self.useDifference.value == False :
                    if ("hole" in header) or ("particle" in header):
                        row.append(inputDataFrame.column(j).get(i))

                # Use difference (particle - hole) charges
                if self.useDifference.value == True :
                    if ("diff" in header):
                        row.append(inputDataFrame.column(j).get(i))
            X.append(row)

        X = np.array(X)
        print("Dimensions before reduction: " + str(X.shape))

        # Dimensionality reduction
        X_embedded = TSNE(n_components=2, perplexity=self.perplexity.value, 
                          learning_rate=self.learningRate.value, n_iter=self.nIter.value).fit_transform(X)
        X_transposed = np.transpose(X_embedded)
        #embedding = MDS(n_components=2)
        #X_embedded = embedding.fit_transform(X)
        X_transposed = np.transpose(X_embedded)
        
        dataframe = df.DataFrame()
        dataframe.addFloatColumn("1", X_transposed[0])
        dataframe.addFloatColumn("2", X_transposed[1])
        dataframe.updateIndex()

        self.outport.setData(dataframe)
