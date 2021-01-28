# Name: ParameterSource 

import inviwopy as ivw
import ivwdataframe as df

class ParameterSource(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.outport = df.DataFrameOutport("outport")
        self.addOutport(self.outport)

        self.file = ivw.properties.FileProperty("file", "File")
        self.addProperty(self.file, owner=False)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.ParameterSource", 
    		displayName = "Parameter Source",
    		category = "Python",
    		codeState = ivw.CodeState.Stable,
    		tags = ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return ParameterSource.processorInfo()

    def initializeResources(self):
        print("init")

    def process(self):
        if not self.file.value:
            self.outport.setData(df.DataFrame())
            return

        f = open(self.file.value, mode='r')
        lines = f.readlines()

        name = []
        energyEv = []
        vawelengthNm = []
        oscillatoryStrength = []
        rotatoryStrength = []
        for i in range(0, len(lines)):
            splitted = lines[i].strip().split(" ")
            splitted[:] = [x for x in splitted if x]

            name.append(splitted[0].strip())
            energyEv.append(float(splitted[1].strip()))
            vawelengthNm.append(float(splitted[2].strip()))
            oscillatoryStrength.append(float(splitted[3].strip()))
            rotatoryStrength.append(float(splitted[4].strip()))

        dataframe = df.DataFrame()
        dataframe.addCategoricalColumn("Name", name)
        dataframe.addFloatColumn("Energy (eV)", energyEv)
        dataframe.addFloatColumn("Wavelength (nm)", vawelengthNm)
        dataframe.addFloatColumn("Oscillatory strength", oscillatoryStrength)
        dataframe.addFloatColumn("Rotatory strength", rotatoryStrength)

        self.outport.setData(dataframe)
