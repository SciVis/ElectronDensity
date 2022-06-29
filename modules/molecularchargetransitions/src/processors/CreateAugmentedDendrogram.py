# Name: CreateAugmentedDendrogram 

# Need a cloned version of https://github.com/tbmasood/ElectronicTransitionsLOD in a local folder
# with the Dendrogram script already compiled (using 'javac Dendrogram.java')
# Also, java needs to be installed on the computer (using the location of the java.exe as a property input)

# It is possible to change draw settings in the dendrogram_draw.properties file (in ElectronicTransitionsLOD)

# Assumes the input data frame contains feature vector for each member

import inviwopy as ivw
import ivwdataframe as df
from sklearn.cluster import AgglomerativeClustering
import numpy as np
import subprocess
import os

class CreateAugmentedDendrogram(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.dataFrame = df.DataFrameInport("dataFrame")
        self.addInport(self.dataFrame, owner=False)

        self.featureVectorName = ivw.properties.StringProperty("fetureVectorName", "Feature vector name", "TranFV")
        self.addProperty(self.featureVectorName)

        self.linkage = ivw.properties.OptionPropertyString(
            "linkage", "Linkage (Agglomerative)", [ivw.properties.StringOption("ward", "Ward", "ward"), 
                                                   ivw.properties.StringOption("complete", "Complete", "complete"), 
                                                   ivw.properties.StringOption("average", "Average", "average"),
                                                   ivw.properties.StringOption("single", "Single", "single")])
        self.addProperty(self.linkage)

        self.threshold = ivw.properties.FloatProperty("threshold", "Threshold", 5.0, 0.0, 30.0, 0.05)
        self.addProperty(self.threshold)

        self.electronicTransitionsLODLocation = ivw.properties.DirectoryProperty("repo_location", "Folder with ElectronicTransitionsLOD", "/")
        self.addProperty(self.electronicTransitionsLODLocation)

        self.javaLocation = ivw.properties.StringProperty("java_location", "Path to java.exe (put in \"\")", "/")
        self.addProperty(self.javaLocation)

        self.fileLocation = ivw.properties.DirectoryProperty("file_path", "File location (save files)", "/")
        self.addProperty(self.fileLocation)

        self.saveFileCheckbox = ivw.properties.BoolProperty("save_file", "Save augmented dendrogram", False)
        self.addProperty(self.saveFileCheckbox)

        self.resetFileLocation = ivw.properties.BoolProperty("reset_file", "Reset output file location", False)
        self.addProperty(self.resetFileLocation)

        self.resultingFolder = ivw.properties.StringProperty("resulting_folder", "Output file", "/")
        self.addProperty(self.resultingFolder)
        

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.CreateAugmentedDendrogram", 
    		displayName = "Create Augmented Dendrogram",
    		category = "Python",
    		codeState = ivw.CodeState.Stable,
    		tags = ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return CreateAugmentedDendrogram.processorInfo()

    def initializeResources(self):
        print("init")

    def process(self):
        print("process")
        inputDataFrame = self.dataFrame.getData()

        if (self.resetFileLocation.value == True):
            self.resultingFolder.value = '/'

        if (self.saveFileCheckbox.value == True):
            dendrogram_files_name = "augmented_dendrogram"
            job_id = 'job_id'

            # Write information to file (needed to run the Dendrogram script)
            with open(f"{self.fileLocation.value}/{dendrogram_files_name}.txt", "w") as text_file:
                # Number of members
                text_file.write(str(inputDataFrame.rows)+'\n')
                # Get feature vectors from data frame
                featureVector_name = self.featureVectorName.value.lower()
                feature_vectors = []
                for i in range(0, inputDataFrame.rows):
                    fv_row = []
                    for j in range(0, inputDataFrame.cols):
                        header = inputDataFrame.column(j).header.lower()
                        #if ("hole" in header) or ("particle" in header):
                        if featureVector_name in header:
                            value = inputDataFrame.column(j).get(i)
                            fv_row.append(value)
                            text_file.write(f"{value} ")
                    text_file.write("\n")
                
                    feature_vectors.append(fv_row)

                X = np.array(feature_vectors)
                clustering = AgglomerativeClustering(n_clusters=None, linkage=self.linkage.value, distance_threshold=-20.0).fit(X)

                # The result from agglomerative clustering
                for i in range(clustering.n_leaves_ - 1):
                    text_file.write(f"{clustering.children_[i][0]} {clustering.children_[i][1]} {clustering.distances_[i]}\n")
                text_file.close()

            # Execute command to run ElectronicTransitionsLOD
            os.chdir(self.electronicTransitionsLODLocation.value)
            s = self.javaLocation.value + ' Dendrogram ' + self.fileLocation.value + '/' + dendrogram_files_name + '.txt '\
                + self.fileLocation.value + ' ' + job_id + ' ' + str(self.threshold.value)
            res = subprocess.run(s, shell=True, capture_output=True) 
            print(f"returncode: {res.returncode}, stderr: {res.stderr}, stdout: {res.stdout}, executed: {s}")

            # The resulting png file (if the script runs sucessfully)
            self.resultingFolder.value = f"{self.fileLocation.value}/{job_id}/dendrogram.png"

