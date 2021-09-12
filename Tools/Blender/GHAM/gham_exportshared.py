if "bpy" not in locals():
    import bpy
if "gham_structs" not in locals():
    from . import gham_structs
if "struct" not in locals():
    import struct

class ghamExportSettings:
    def __init__(self):
        self.exportText = True
        self.transformVerts = True
        self.yUp = True
        self.exportUV = True
        self.exportRefPose = True
        self.generateExtraBones = True

# info about vertex groups for a specific vert
class ghamVertexGroupEntry:
    def __init__(self, groupIdx, weight):
        self.groupIdx = groupIdx
        self.weight = weight
class ghamVertexGroupInfo:
    def __init__(self):
        self.groups = []

class ghamAnimInfo:
    def __init__(self):
        self.name = "noname"
        self.firstFrame = 0
        self.numFrames = 0
class ghamAnimList:
    def __init__(self):
        self.anims = []
