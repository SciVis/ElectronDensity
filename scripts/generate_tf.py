# Inviwo Python script 
import inviwopy
def color(x):
   r = x
   g = 1-x
   return vec3(r,g,0)
numseg = 97
delta = 1.0/(numseg)
width = delta*0.2
#inviwo.clearTransferfunction("VolumeRaycaster.transferFunction")
#inviwo.addPointToTransferFunction("VolumeRaycaster.transferFunction",(0.0,0.0),(0,0,0)) 

tf = inviwopy.app.network.AtlasVolumeRaycaster5.atlasTF
tf.clear()
#tf.add(0,vec4(0,0,0,0)) 
points = []
points.append(inviwopy.data.TFPrimitiveData(0,vec4(0,0,0,0)))
for i in range(0,numseg,1):
   x = i * delta + 0.5*delta
   points.append(inviwopy.data.TFPrimitiveData(x-width,vec4(0,0,0,0)))
   points.append(inviwopy.data.TFPrimitiveData(x,vec4(color(x),0.5)))
   points.append(inviwopy.data.TFPrimitiveData(x+width,vec4(0,0,0,0)))
   #tf.add(x-width,vec4(0,0,0,0)) 
   #tf.add(x,vec4(color(x),0.5)) 
   #tf.add(x+width,vec4(0,0,0,0)) 
points.append(inviwopy.data.TFPrimitiveData(1,vec4(0,0,0,0)))
#tf.add(1,vec4(0,0,0,0)) 
tf.add(points)
   #inviwopy.addPointToTransferFunction("AtlasVolumeRaycaster5.atlasTF",(x, 0.24),color(x)) 
   #inviwopy.addPointToTransferFunction("AtlasVolumeRaycaster5.atlasTF",(x+delta, 0),(0,0,0)) 
# inviwo.addPointToTransferFunction("VolumeRaycaster.transferFunction",(1.0,0.0),(0,0,0)) 