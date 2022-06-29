# Name: CreateMemberTransitionDiagram 

# Need a cloned version of https://github.com/tbmasood/ElectronicTransitionsLOD in a local folder
# with the TransitionDiagram script already compiled (using 'javac TransitionDiagram.java')
# Also, java needs to be installed on the computer (using the location of the java.exe as a property input)

# It is possible to change draw settings in the draw.properties file (in ElectronicTransitionsLOD)

# Assumes the input data frame contains name, state and feature vector for each member

import inviwopy as ivw
import ivwdataframe as df
import subprocess
import os

class CreateMemberTransitionDiagram(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.dataFrame = df.DataFrameInport("dataFrame")
        self.addInport(self.dataFrame, owner=False)

        self.name = ivw.properties.StringProperty("name", "Name", "-")
        self.addProperty(self.name)

        self.state = ivw.properties.StringProperty("state", "State", "-")
        self.addProperty(self.state)

        self.electronicTransitionsLODLocation = ivw.properties.DirectoryProperty("repo_location", "Folder with ElectronicTransitionsLOD", "/")
        self.addProperty(self.electronicTransitionsLODLocation)

        self.javaLocation = ivw.properties.StringProperty("java_location", "Path to java.exe (put in \"\")", "/")
        self.addProperty(self.javaLocation)

        self.fileLocation = ivw.properties.DirectoryProperty("file_path", "File location (save files)", "/")
        self.addProperty(self.fileLocation)

        self.featureVectorName = ivw.properties.StringProperty("fetureVectorName", "Feature vector name", "TranFV")
        self.addProperty(self.featureVectorName)

        self.nameColumnName = ivw.properties.StringProperty("name_column_name", "Name column name", "Name")
        self.addProperty(self.nameColumnName)

        self.stateColumnName = ivw.properties.StringProperty("state_column_name", "State column name", "State")
        self.addProperty(self.stateColumnName)

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

        self.saveFileCheckbox = ivw.properties.BoolProperty("save_files", "Save member transition diagram", False)
        self.addProperty(self.saveFileCheckbox)

        self.resetFileLocation = ivw.properties.BoolProperty("reset_file", "Reset output file location", False)
        self.addProperty(self.resetFileLocation)

        self.resultingFolder = ivw.properties.StringProperty("resulting_folder", "Output file", "/")
        self.addProperty(self.resultingFolder)


    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.CreateMemberTransitionDiagram", 
    		displayName = "Create Member Transition Diagram",
    		category = "Python",
    		codeState = ivw.CodeState.Stable,
    		tags = ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return CreateMemberTransitionDiagram.processorInfo()

    def initializeResources(self):
        print("init")

    def process(self):
        print("process")
        inputDataFrame = self.dataFrame.getData()

        if (self.resetFileLocation.value == True):
            self.resultingFolder.value = '/'

        if (self.saveFileCheckbox.value == True):
            features_file_name = 'member_diagram_feature'
            labels_file_name = 'member_diagram_labels'
            job_id = 'job_id'
            file_id = str(self.name.value) + '_' + str(self.state.value).replace(" ", "")
            name_column_name = self.nameColumnName.value.lower()
            state_column_name = self.stateColumnName.value.lower()

            # Get the column indices for name and state
            name_col_index = -1
            state_col_index = -1
            for j in range(0, inputDataFrame.cols):
                header = inputDataFrame.column(j).header.lower()
                if (header == name_column_name):
                    name_col_index = j
                if (header == state_column_name):
                    state_col_index = j

            if(name_col_index == -1 or state_col_index == -1):
                print("CreateMemberTransitionDiagram: Could not find name column and/or state column")
                return

            # Save file with feature vector
            featureVector_name = self.featureVectorName.value.lower()
            fv_row = []
            with open(f"{self.fileLocation.value}/{features_file_name}.txt", "w") as text_file:
                for i in range(0, inputDataFrame.rows):
                    if (self.name.value in str(inputDataFrame.column(name_col_index).get(i)) and self.state.value in str(inputDataFrame.column(state_col_index).get(i))):
                        for j in range(0, inputDataFrame.cols):
                            header = inputDataFrame.column(j).header.lower()
                            if featureVector_name in header:
                            #if ("hole" in header) or ("particle" in header):
                                value = inputDataFrame.column(j).get(i)
                                fv_row.append(value)
                                text_file.write(f"{value} ")
                        text_file.write("\n")
                text_file.close()
            
            if (len(fv_row) == 0):
                print(f"CreateMemberTransitionDiagram: Could not find the feature vector for the given name and state: {self.name.value}, {self.state.value}")
                return

            # Save file with labels
            with open(f"{self.fileLocation.value}/{labels_file_name}.txt", "w") as text_file:
                # TODO: solve this in a better way (dynamically name the groups)
                text_file.write(f"{self.name.value}, {self.state.value}\n")
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
            
            # Execute command to run ElectronicTransitionsLOD
            os.chdir(self.electronicTransitionsLODLocation.value)
            s = self.javaLocation.value + ' TransitionDiagram ' + self.fileLocation.value + '/' + features_file_name + '.txt '\
                + self.fileLocation.value + '/' + labels_file_name + ".txt " + self.fileLocation.value + ' ' + job_id + ' ' + file_id
            res = subprocess.run(s, shell=True, capture_output=True) 
            print(f"returncode: {res.returncode}, stderr: {res.stderr}, stdout: {res.stdout}, executed: {s}")

            # The resulting png file (if the script runs sucessfully)
            self.resultingFolder.value = f"{self.fileLocation.value}/{job_id}/{file_id}_MD.png"
