if "bpy" not in locals():
    import bpy
if "struct" not in locals():
    import struct
import mathutils
import math

class ghamHeader:
    binaryFormat="<16i" # little-endian(<), 16ints

    def __init__(self):
        self.ident = 1296123975 # GHAM in int form
        self.version = 1 # file format version
        self.numMeshes = 0
        self.numSkeletonNodes = 0

    def saveBin(self, binFile):
        data = struct.pack(self.binaryFormat,
                           self.ident,
                           self.version,
                           self.numMeshes,
                           self.numSkeletonNodes,
                           0,0,0,0,0,0,0,0,0,0,0,0
                           )
        binFile.write(data)

    def saveText(self, textFile):
        textFile.write("HEADER\n")
        textFile.write("version " + str(self.version) + "\n")
        textFile.write("numMeshes " + str(self.numMeshes) + "\n")
        textFile.write("numSkeletonNodes " + str(self.numSkeletonNodes) + "\n\n")

    def loadBin(self, binFile):
        unpacked = struct.unpack(self.binaryFormat, binFile.read(16*4))
        self.ident = unpacked[0]
        self.version = unpacked[1]
        self.numMeshes = unpacked[2]
        self.numSkeletonNodes = unpacked[3]

    def output(self):
        print("GHAM HEADER")
        print("ident: " + str(self.ident))
        print("version: " + str(self.version))
        print("numMeshes: " + str(self.numMeshes))
        print("numSkeletonNodes: " + str(self.numSkeletonNodes) + "\n")

class ghamMeshHeader:
    binaryFormat="<4i256s256s" # 4 ints, 256 len string, 8 ints

    def __init__(self):
        self.numStreams = 0
        self.numVerts = 0
        self.numIndices = 0
        self.numAnimFrames = 0
        self.name = ""
        self.parentName = ""
        self.isSkinned = 0

    def saveBin(self, binFile):
        data = struct.pack(self.binaryFormat,
                           self.numStreams,
                           self.numVerts,
                           self.numIndices,
                           self.numAnimFrames,
                           self.name.encode('utf-8'),
                           self.parentName.encode('utf-8'))
        binFile.write(data)
        data2 = struct.pack("<8i", self.isSkinned, 0,0,0,0,0,0,0)
        binFile.write(data2)
    
    def saveText(self, textFile):
        textFile.write("MESH HEADER\n")
        textFile.write("numStreams " + str(self.numStreams) + "\n")
        textFile.write("numVerts " + str(self.numVerts) + "\n")
        textFile.write("numIndices " + str(self.numIndices) + "\n")
        textFile.write("numAnimFrames " + str(self.numAnimFrames) + "\n")
        textFile.write("name " + self.name + "\n")
        textFile.write("parentName " + self.parentName + "\n")
        textFile.write("isSkinned " + str(self.isSkinned) + "\n\n")
    
    def loadBin(self, binFile):
        self.numStreams, self.numVerts, self.numIndices, self.numAnimFrames = struct.unpack("<4i", binFile.read(4*4))
        stringUtil = ghamStringUtil()
        nameBuf = binFile.read(256)
        self.name = stringUtil.decodeLoadedString(nameBuf)
        nameBuf = binFile.read(256)
        self.parentName = stringUtil.decodeLoadedString(nameBuf)
        tup = struct.unpack("<1i", binFile.read(4))
        self.isSkinned = tup[0]
        binFile.read(4*7)
    
    def output(self):
        print("MESH HEADER")
        print("numStreams: " + str(self.numStreams))
        print("numVerts: " + str(self.numVerts))
        print("numIndices: " + str(self.numIndices))
        print("numAnimFrames: " + str(self.numAnimFrames))
        print("name: " + str(self.name))
        print("parentName: " + str(self.parentName))
        print("isSkinned: " + str(self.isSkinned) + "\n")

class ghamComponentDef:
    binaryFormat="<1i256s256s" # 1 int, 2 strings

    def __init__(self):
        self.dataCount = 0
        self.dataType = ""
        self.shaderId = ""

    def saveBin(self, binFile):
        data = struct.pack(self.binaryFormat,
                           self.dataCount,
                           self.dataType.encode('utf-8'),
                           self.shaderId.encode('utf-8')
                           )
        binFile.write(data)

    def saveText(self, textFile):
        textFile.write("count: " + str(self.dataCount))
        textFile.write(" type: " + self.dataType)
        textFile.write(" shaderId: " + self.shaderId + "\n")

    def loadBin(self, binFile):
        tempType = ""
        tempId = ""
        self.dataCount, tempType, tempId = struct.unpack(self.binaryFormat, binFile.read(256*2 + 4))
        stringUtil = ghamStringUtil()
        self.dataType = stringUtil.decodeLoadedString(tempType)
        self.shaderId = stringUtil.decodeLoadedString(tempId)
    
    def output(self):
        print(self.dataType + " " + str(self.dataCount) + " " + self.shaderId)

    def calcSizeInBytes(self):
        compSize = 4
        if self.dataType == "CT_BYTE":
            compSize = 1
        elif self.dataType == "CT_UBYTE":
            compSize = 1
        elif self.dataType == "CT_SHORT":
            compSize = 2
        retSize = compSize * self.dataCount
        retSize += (retSize % 4)
        return retSize

class ghamStreamDef:
    binaryFormat="<1i" # 1 int (num vert components)

    def __init__(self):
        self.componentDefs = []

    def saveBin(self, binFile):
        data = struct.pack(self.binaryFormat,
                           len(self.componentDefs)
                           )
        binFile.write(data)

        for componentDef in self.componentDefs:
            componentDef.saveBin(binFile)

    def saveText(self, textFile):
        textFile.write("STREAM COMPONENTS\n")
        for componentDef in self.componentDefs:
            componentDef.saveText(textFile)
        textFile.write("\n")

    def loadBin(self, binFile):
        numComps = struct.unpack(self.binaryFormat, binFile.read(4))
        for i in range(numComps[0]):
            compDef = ghamComponentDef()
            compDef.loadBin(binFile)
            self.componentDefs.append(compDef)

    def output(self):
        print("STREAM COMPONENTS: NumComps " + str(len(self.componentDefs)))
        for comp in self.componentDefs:
            comp.output()
        print("")

    def calcVertSizeInBytes(self):
        retVal = 0
        for componentDef in self.componentDefs:
            retVal += componentDef.calcSizeInBytes()
        return retVal

class ghamSkeletonNode:

    def __init__(self):
        self.name = ""
        self.editBoneName = ""
        self.parentName = ""
        self.transforms = []
        self.numAnimFrames = 0
        self.useBoneHead = False # if this is a bone we create for export then use head instead of tail.

    def setNumFrames(self, numFrames):
        #print("setNumFrames " + str(numFrames))
        self.numAnimFrames = numFrames
        self.transforms = []
        self.transforms.append(ghamTransform())
        for i in range(self.numAnimFrames):
            self.transforms.append(ghamTransform())

    def saveBin(self, binFile):
        data = struct.pack("<256s256s",
                           self.name.encode('utf-8'),
                           self.parentName.encode('utf-8'))
        binFile.write(data)
        self.transforms[0].saveBin(binFile)
        data = struct.pack("<1i", self.numAnimFrames)
        binFile.write(data)

    def saveText(self, textFile):
        textFile.write("NODE " + str(self.name) + " parent(" + str(self.parentName) + ") anims(" + str(self.numAnimFrames) + ")\n")
        for i in self.transforms[0].vals:
            textFile.write(str(i) + " ")
        textFile.write("\n")

    def loadBin(self, binFile):
        stringUtil = ghamStringUtil()
        self.name = stringUtil.decodeLoadedString(binFile.read(256))
        self.parentName = stringUtil.decodeLoadedString(binFile.read(256))
        rootTrans = ghamTransform()
        for i in range(16):
            tup = struct.unpack("<1f", binFile.read(4))
            rootTrans.vals[i] = tup[0]
        numAnimFrames = struct.unpack("<1i", binFile.read(4))
        self.setNumFrames(numAnimFrames[0])
        for i in range(16):
            self.transforms[0].vals[i] = rootTrans.vals[i]
    
    def output(self):
        print("Node " + str(self.name) + " parent(" + str(self.parentName) + ") anims(" + str(self.numAnimFrames) + ")")
        #self.transforms[0].output()

class ghamVec3:
    binaryFormat="<3f"

    def __init__(self):
        self.xyz = [0,0,0]

    def saveBin(self, binFile):
        data = struct.pack(self.binaryFormat,
                           self.xyz[0],
                           self.xyz[1],
                           self.xyz[2])
        binFile.write(data)

    def saveText(self, textFile):
        textFile.write(str(self.xyz[0]) + " " + str(self.xyz[1]) + " " + str(self.xyz[2]) + " ")

class ghamVec2:
    binaryFormat="<2f"
                       
    def __init__(self):
        self.xy = [0,0]
                       
    def saveBin(self, binFile):
        data = struct.pack(self.binaryFormat,
                    self.xy[0],
                    self.xy[1])
        binFile.write(data)
                       
    def saveText(self, textFile):
        textFile.write(str(self.xy[0]) + " " + str(self.xy[1]) + " ")

# list of 16 floats
class ghamTransform:
    def __init__(self):
        self.vals = [1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1]

    def saveBin(self, binFile):
        for i in self.vals:
            data = struct.pack("<1f", i)
            binFile.write(data)

    def saveText(self, textFile):
        for i in self.vals:
            textFile.write(str(i) + " ")
        textFile.write("\n")

    def loadBin(self, binFile):
        for i in range(16):
            val = struct.unpack("<1f", binFile.read(4))
            self.vals[i] = val[0]

    def output(self):
        outStr = ""
        for i in range(16):
            outStr += str(self.vals[i]) + " "
            if i == 3 or i == 7 or i == 11:
                print(outStr)
                outStr = ""
        print(outStr)

    def rotateToYUp(self):
        # blender is rotated 90 degrees along the x axis
        rotation = mathutils.Matrix.Rotation(math.radians(-90), 4, 'X')
        myBlenderMat = self.toBlenderMatrix()
        myBlenderMat = rotation @ myBlenderMat
        self.fromBlenderMatrix(myBlenderMat)
    
    def rotateToZUp(self):
        # blender is rotated 90 degrees along the x axis
        rotation = mathutils.Matrix.Rotation(math.radians(90), 4, 'X')
        myBlenderMat = self.toBlenderMatrix()
        myBlenderMat = rotation @ myBlenderMat
        self.fromBlenderMatrix(myBlenderMat)
    
    def toBlenderMatrix(self):
        ret = mathutils.Matrix()
        idx = 0
        for col in range(4):
            for row in range(4):
                ret[row][col] = self.vals[idx]
                idx += 1
        return ret

    def fromBlenderMatrix(self, matrix):
        idx = 0
        for col in range(4):
            for row in range(4):
                self.vals[idx] = matrix[row][col]
                idx += 1


# we load in garbage from the file, so turn it into a palatable string
class ghamStringUtil:
    def decodeLoadedString(self, srcBuf):
        destString = ""
        srcString = str(srcBuf)
        strLen = len(srcString)
        
        charIdx = 0
        skipTwo = False
        if strLen > 2 and srcString[0] == 'b' and srcString[1] == "'":
            skipTwo = True
        
        for c in srcString:
            if skipTwo == False or charIdx >= 2:
                if (c == '\\' and srcString[charIdx+1] == 'x' and srcString[charIdx+2] == '0'):
                    break
                destString = destString + str(c)
            charIdx += 1
    
        return destString


