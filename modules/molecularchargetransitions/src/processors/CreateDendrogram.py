# Name: CreateDendrogram 
# Requires scipy>=1.6.0

import inviwopy as ivw
import ivwdataframe as df
from inviwopy.glm import vec2,vec3,vec4

import numpy as np
from matplotlib import pyplot as plt
from matplotlib import colors as col
from scipy.cluster.hierarchy import dendrogram
from sklearn.cluster import AgglomerativeClustering

class CreateDendrogram(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.dataFrame = df.DataFrameInport("dataFrame")
        self.addInport(self.dataFrame, owner=False)
        self.outport = df.DataFrameOutport("outport")
        self.addOutport(self.outport)

        self.featureVectorName = ivw.properties.StringProperty("fetureVectorName", "Feature vector name", "TranFV")
        self.addProperty(self.featureVectorName)

        self.linkage = ivw.properties.OptionPropertyString(
            "linkage", "Linkage (Agglomerative)", [ivw.properties.StringOption("ward", "Ward", "ward"), 
                                                   ivw.properties.StringOption("complete", "Complete", "complete"), 
                                                   ivw.properties.StringOption("average", "Average", "average"),
                                                   ivw.properties.StringOption("single", "Single", "single")])
        self.addProperty(self.linkage)

        self.distanceMetric = ivw.properties.OptionPropertyString(
            "distance_metric", "Distance metric", [ivw.properties.StringOption("euclidean", "euclidean", "euclidean"), 
                                                   ivw.properties.StringOption("l1", "l1", "l1"), 
                                                   ivw.properties.StringOption("l2", "l2", "l2"),
                                                   ivw.properties.StringOption("manhattan", "manhattan", "manhattan"),
                                                   ivw.properties.StringOption("cosine", "cosine", "cosine")])
        self.addProperty(self.distanceMetric)

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

        self.fileLocation = ivw.properties.DirectoryProperty("file_path", "File location", "/")
        self.addProperty(self.fileLocation)

        self.fileName = ivw.properties.StringProperty("file_name", "File name", "dendrogram")
        self.addProperty(self.fileName)

        self.inputTf = ivw.properties.TransferFunctionProperty("inputTf", "Input transfer function", ivw.data.TransferFunction())
        self.addProperty(self.inputTf, owner=False)

        self.outputTf = ivw.properties.TransferFunctionProperty("outputTf", "Output transfer function", ivw.data.TransferFunction())
        self.addProperty(self.outputTf, owner=False)

        self.highlightCluster = ivw.properties.BoolProperty("highlight_cluster", "Highlight cluster", False)
        self.addProperty(self.highlightCluster)

        self.clusterToHighlight = ivw.properties.IntProperty("cluster_to_highlight", "Cluster to highlight", 1, 0, 10, 1)
        self.addProperty(self.clusterToHighlight)

        self.reloadButton = ivw.properties.ButtonProperty("reload_button", "Reset")
        self.addProperty(self.reloadButton)


    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.CreateDendrogram", 
    		displayName = "Create Dendrogram",
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
        d = dendrogram(linkage_matrix, **kwargs)
        return d

    def process(self):
        print("process")
        inputDataFrame = self.dataFrame.getData()
        
        # This seem to help if get error: 'NoneType' object has no attribute 'rows' ??
        print(inputDataFrame.rows)

        # Get feature vector from data frame
        featureVector_name = self.featureVectorName.value.lower()
        X = []
        labels = []
        for i in range(0, inputDataFrame.rows):
            fv_row = []
            label = ""
            for j in range(0, inputDataFrame.cols):
                header = inputDataFrame.column(j).header.lower()
                
                # Labels
                #if (header=="name" or header=="state"):
                #    label = label + inputDataFrame.column(j).get(i)

                if featureVector_name in header:
                    fv_row.append(inputDataFrame.column(j).get(i))

            X.append(fv_row)
            labels.append(label)

        X = np.array(X)
        print("CreateDendrogram, dimensions of X: " + str(X.shape))

        # setting distance_threshold=0 ensures we compute the full tree.
        model = AgglomerativeClustering(distance_threshold=self.threshold.value, n_clusters=None, linkage=self.linkage.value, metric=self.distanceMetric.value)
        
        model = model.fit(X)

        dataframe = df.DataFrame()
        dataframe.addFloatColumn(self.columnName.value, model.labels_ + np.ones(len(model.labels_)))

        # Sample colors from input transfer function (between 0 and 1)
        delta = 1.0/model.n_clusters_
        x = delta/2
        colors = []
        for i in range(0,model.n_clusters_):
            colors.append(col.to_hex(np.array(self.inputTf.value.sample(x))))
            x = x + delta
        
        # Set color palette for dendrogram
        from scipy.cluster import hierarchy
        hierarchy.set_link_color_palette(colors)

        plt.clf()

        # plot the top p levels of the dendrogram
        #self.plot_dendrogram(model, truncate_mode='level', p=5)
        # plot the whole dendrogram
        #d = self.plot_dendrogram(model, color_threshold=self.threshold.value, no_labels=False, get_leaves=True, above_threshold_color='grey', leaf_rotation=90., leaf_font_size=3.)
        d = self.plot_dendrogram(model, color_threshold=self.threshold.value, no_labels=True, get_leaves=True, above_threshold_color='grey')

        dendrogramColors = d['leaves_color_list']
        leaves = d['leaves']
        z = zip(leaves, dendrogramColors)
        
        # sort these together to get the leave indices with corresponding color
        z_sorted = sorted(z, key = lambda t: t[0])
        z_unzipped = list(zip(*z_sorted))

        dataframe.addCategoricalColumn("colors", z_unzipped[1])
        dataframe.addIntColumn("leaves", z_unzipped[0])

        # set output transfer function
        outputTf = self.outputTf.value
        outputTf.clear()

        if (self.highlightCluster.value == True):
            x = 0
            for i in range(0,model.n_clusters_):
                if (i == self.clusterToHighlight.value):
                    outputTf.add(x, ivw.glm.vec4(col.to_rgba('#3f3f3f')))
                    outputTf.add(x+delta, ivw.glm.vec4(col.to_rgba('#3f3f3f')))
                else:
                    outputTf.add(x, ivw.glm.vec4(col.to_rgba('#bebebe')))
                    outputTf.add(x+delta, ivw.glm.vec4(col.to_rgba('#bebebe')))
                x = x + delta
        else:
            x = 0
            for i in range(0,model.n_clusters_):
                ind = np.where(model.labels_==i)
                outputTf.add(x, ivw.glm.vec4(col.to_rgba(z_unzipped[1][ind[0][0]])))
                outputTf.add(x+delta, ivw.glm.vec4(col.to_rgba(z_unzipped[1][ind[0][0]])))
                x = x + delta

        # plot line at threshold
        plt.axhline(y=self.threshold.value, c='grey', lw=1, linestyle='dashed')
        ax = plt.gca()
        ax.axes.yaxis.set_visible(False)

        plt.tight_layout()
        # plt.xlabel("Number of points in node (or index of point if no parenthesis).")
        if (self.saveFileCheckbox.value == True):
            if (self.fileLocation.value == "/"):
                print('CreateDendrogram: specify a valid file location!')
            else:
                print('Saving dendrogram figure')
                print('location: ' + str(self.fileLocation.value + '/' + self.fileName.value + self.fileFormat.value))
                plt.savefig(self.fileLocation.value + '/' + self.fileName.value + self.fileFormat.value, bbox_inches='tight')
                #self.reloadButton.press()

        dataframe.updateIndex()

        self.outport.setData(dataframe)
        
