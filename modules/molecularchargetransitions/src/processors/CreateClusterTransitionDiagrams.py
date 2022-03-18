# Name: CreateClusterTransitionDiagrams 

# Need a cloned version of https://github.com/tbmasood/ElectronicTransitionsLOD in a local folder
# with the ClusterDiagram script already compiled (using 'javac ClusterDiagram.java')
# Also, java needs to be installed on the computer (using the location of the java.exe as a property input)

# It is possible to change draw settings in the draw.properties file (in ElectronicTransitionsLOD)

# Currently, it assumes the 'hole' and 'particle' charges are used as feature vectors

import inviwopy as ivw
import ivwdataframe as df
import subprocess
import os

class CreateClusterTransitionDiagrams(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.dataFrame = df.DataFrameInport("dataFrame")
        self.addInport(self.dataFrame, owner=False)

        self.electronicTransitionsLODLocation = ivw.properties.DirectoryProperty("repo_location", "Folder with ElectronicTransitionsLOD", "/")
        self.addProperty(self.electronicTransitionsLODLocation)

        self.javaLocation = ivw.properties.StringProperty("java_location", "Path to java.exe (put in \"\")", "/")
        self.addProperty(self.javaLocation)

        self.fileLocation = ivw.properties.DirectoryProperty("file_path", "File location (save files)", "/")
        self.addProperty(self.fileLocation)

        self.clusterColumnName = ivw.properties.StringProperty("cluster_column_name", "Cluster column name", "Cluster")
        self.addProperty(self.clusterColumnName)

        self.nrSubgroups = ivw.properties.IntProperty("nr_subgroups", "Number of subgroups", 3, 1, 5)
        self.addProperty(self.nrSubgroups)

        self.sg1 = ivw.properties.StringProperty("sg_name_1", "Subgroup 1", "1")
        self.addProperty(self.sg1)
        self.sg2 = ivw.properties.StringProperty("sg_name_2", "Subgroup 2", "2")
        self.addProperty(self.sg2)
        self.sg3 = ivw.properties.StringProperty("sg_name_3", "Subgroup 3", "3")
        self.addProperty(self.sg3)
        self.sg4 = ivw.properties.StringProperty("sg_name_4", "Subgroup 4", "4")
        self.addProperty(self.sg4)
        self.sg5 = ivw.properties.StringProperty("sg_name_5", "Subgroup 5", "5")
        self.addProperty(self.sg5)

        self.saveFileCheckbox = ivw.properties.BoolProperty("save_files", "Save cluster transition diagrams", False)
        self.addProperty(self.saveFileCheckbox)

        self.resetFileLocation = ivw.properties.BoolProperty("reset_file", "Reset output file location", False)
        self.addProperty(self.resetFileLocation)

        self.resultingFolder = ivw.properties.StringProperty("resulting_folder", "Output file", "/")
        self.addProperty(self.resultingFolder)


    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.CreateClusterTransitionDiagrams", 
    		displayName = "CreateClusterTransitionDiagrams",
    		category = "Python",
    		codeState = ivw.CodeState.Stable,
    		tags = ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return CreateClusterTransitionDiagrams.processorInfo()

    def initializeResources(self):
        print("init")

    def process(self):
        print("process")
        inputDataFrame = self.dataFrame.getData()
        
        if (self.resetFileLocation.value == True):
            self.resultingFolder.value = '/'

        if (self.saveFileCheckbox.value == True):
            cluster_file_name = 'cluster_diagram_cluster'
            labels_file_name = 'cluster_diagram_labels'
            job_id = 'job_id'
            cluster_name = self.clusterColumnName.value.lower()

            # Get the range for the cluster indices
            cluster_range = []
            cluster_col_nr = -1
            for k in range(0, inputDataFrame.cols):
                header = inputDataFrame.column(k).header.lower()
                if (header == cluster_name):
                    cluster_range = inputDataFrame.column(k).range
                    cluster_col_nr = k

            if(cluster_col_nr == -1):
                print("CreateClusterTransitionDiagrams: Could not find cluster column")
                return

            # For each cluster, save two files:
            # 1) all feature vectors
            # 2) labels (title, subgroups...)
            for c in range(int(cluster_range[0]), int(cluster_range[1] + 1)):
                with open(f"{self.fileLocation.value}/{cluster_file_name}{int(c)}.txt", "w") as text_file:
                    feature_vectors = []
                    for i in range(0, inputDataFrame.rows):
                        if (inputDataFrame.column(cluster_col_nr).get(i) == c):
                            fv = []
                            for j in range(0, inputDataFrame.cols):
                                header = inputDataFrame.column(j).header.lower()
                                if ("hole" in header) or ("particle" in header):
                                    value = inputDataFrame.column(j).get(i)
                                    fv.append(value)
                                    text_file.write(f"{value} ")
                            text_file.write("\n")
                        feature_vectors.append(fv)
                    text_file.close()

                with open(f"{self.fileLocation.value}/{labels_file_name}{int(c)}.txt", "w") as text_file:
                    # TODO: solve this in a better way (dynamically name the groups)
                    text_file.write(f"Cluster {int(c)}\n")
                    text_file.write(f"{self.nrSubgroups.value}\n") 
                    text_file.write(f"{self.sg1.value}\n")
                    if (self.nrSubgroups.value > 1):
                        text_file.write(f"{self.sg2.value}\n")
                    if (self.nrSubgroups.value > 2):
                        text_file.write(f"{self.sg3.value}\n")
                    if (self.nrSubgroups.value > 3):
                        text_file.write(f"{self.sg4.value}\n")
                    if (self.nrSubgroups.value > 4):
                        text_file.write(f"{self.sg5.value}\n")
                    text_file.close()

            # Execute command to run ElectronicTransitionsLOD, for each cluster
            os.chdir(self.electronicTransitionsLODLocation.value)
            for c in range(int(cluster_range[0]), int(cluster_range[1] + 1)):
                s = self.javaLocation.value + ' ClusterDiagram ' + self.fileLocation.value + '/' + cluster_file_name + str(int(c)) + '.txt '\
                    + self.fileLocation.value + '/' + labels_file_name + str(int(c)) + ".txt " + self.fileLocation.value + ' ' + ' ' + job_id + ' ' + str(int(c))
                res = subprocess.run(s, shell=True, capture_output=True)
                print(f"returncode: {res.returncode}, stderr: {res.stderr}, stdout: {res.stdout}, executed: {s}")

            # The resulting png files (if the script runs sucessfully)
            self.resultingFolder.value = f"{self.fileLocation.value}/{job_id}/*CD.png"


            