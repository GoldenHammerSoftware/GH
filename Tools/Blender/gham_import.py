if "bpy" not in locals():
    import bpy
if "gham_structs" not in locals():
    from . import gham_structs
if "struct" not in locals():
    import struct
import mathutils
import math
import bmesh

class ghamImportSettings:
    def __init__(self):
        self.yUp = True
        self.preserveBoneRot = False
        self.ghamVersion = 1

# all the info that could be associated with a vert, extracted from the vb
class ghamBlenderVert:
    def __init__(self):
        self.pos = [0,0,0]
        self.uv = [0,0]
        self.uv1 = [0,0]
        self.normal = [0,0,0]
        self.groupIds = []
        self.groupWeights = []

class ghamImporter:
    # import is a keyword, so doImport
    def doImport(self, binFile, reporter, importSettings):
        print("gham_import.py import called")

        header = gham_structs.ghamHeader()
        header.loadBin(binFile)
        if header.ident != 1296123975:
            reporter.report({"ERROR"}, "File not recognized as GHAM")
        header.output()

        importSettings.ghamVersion = header.version

        nodeHeaders = []
        armObj = self.loadSkeleton(binFile, reporter, header, importSettings, nodeHeaders)

        for i in range(header.numMeshes):
            self.loadMesh(binFile, reporter, importSettings, armObj, nodeHeaders)

        bpy.context.view_layer.update()
    
    def loadSkeleton(self, binFile, reporter, ghamHeader, importSettings, nodeHeaders):
        for i in range(ghamHeader.numSkeletonNodes):
            self.loadSkeletonNodeHeader(binFile, reporter, nodeHeaders, importSettings)
        print("")
                
        for nodeHeader in nodeHeaders:
            for i in range(nodeHeader.numAnimFrames):
                nodeHeader.transforms[i+1].loadBin(binFile)

        # the root node is rotated, unrotate it.
        for nodeHeader in nodeHeaders:
            if importSettings.yUp == True and nodeHeader.parentName == "":
                for i in range(nodeHeader.numAnimFrames+1):
                    nodeHeader.transforms[i].rotateToZUp()

        armObj = self.createBlenderBones(nodeHeaders, importSettings)
        return armObj
            
    def loadSkeletonNodeHeader(self, binFile, reporter, nodeHeaders, importSettings):
        header = gham_structs.ghamSkeletonNode()
        header.loadBin(binFile)
        header.output()
        nodeHeaders.append(header)

    def vec_roll_to_mat3(self, vec, roll):
        target = mathutils.Vector((0,1,0))
        nor = vec.normalized()
        axis = target.cross(nor)
        if axis.dot(axis) > 0.000001:
            axis.normalize()
            theta = target.angle(nor)
            bMatrix = mathutils.Matrix.Rotation(theta, 3, axis)
        else:
            updown = 1 if target.dot(nor) > 0 else -1
            bMatrix = mathutils.Matrix.Scale(updown, 3)
        rMatrix = mathutils.Matrix.Rotation(roll, 3, nor)
        mat = rMatrix @ bMatrix
        return mat

    def mat3_to_vec_roll(self, mat):
        vec = mat.col[1]
        vecmat = self.vec_roll_to_mat3(mat.col[1], 0)
        vecmatinv = vecmat.inverted()
        rollmat = vecmatinv @ mat
        roll = math.atan2(rollmat[0][2], rollmat[2][2])
        return vec, roll
    
    def createBlenderBones(self, nodeHeaders, importSettings):
        if (len(nodeHeaders) == 0):
            return 0
        
        bpy.ops.object.armature_add()
        ob = bpy.context.view_layer.objects.active
        ob.name = "skeleton"
        arm = ob.data
        
        bpy.ops.object.mode_set(mode='EDIT')
        # armature comes with 1 bone by default
        for i in range(len(nodeHeaders)-1):
            bpy.ops.armature.bone_primitive_add()

        # transforms are in local space, convert to world space
        worldMats = []
        for node in nodeHeaders:
            worldMats.append(self.createWorldNode(node, nodeHeaders, 0))

        # create bones with appropriate names
        for i in range(len(nodeHeaders)):
            eb = arm.edit_bones[i]
            eb.name = nodeHeaders[i].name
            eb.use_inherit_rotation = True
            eb.use_inherit_scale = True
            eb.use_local_location = True
            eb.use_connect = True
    
        for i in range(len(nodeHeaders)):
            eb = arm.edit_bones[i]

            axis, roll = self.mat3_to_vec_roll(worldMats[i].to_3x3())
            print("bone axis " + str(axis))
            eb.roll = roll

            axis *= 0.1
            
            eb.tail = worldMats[i].to_translation()
            if nodeHeaders[i].parentName == "":
                eb.head = eb.tail;
                eb.head -= axis
    
            if importSettings.preserveBoneRot == False or nodeHeaders[i].parentName == "":
                self.applyBoneParent(arm, eb, nodeHeaders[i].parentName)
            else:
                # create offset bones so we can preserve the incoming rotation.
                bpy.ops.armature.bone_primitive_add()
                offsetBone = arm.edit_bones[len(arm.edit_bones)-1]
                self.applyBoneParent(arm, offsetBone, nodeHeaders[i].parentName);
                offsetBone.name = nodeHeaders[i].name + "offset"
                offsetBone.use_inherit_rotation = True
                offsetBone.use_inherit_scale = True
                offsetBone.use_local_location = True
                offsetBone.use_connect = True
                offsetBone.tail = worldMats[i].to_translation()
                offsetBone.tail -= axis
                eb.parent = offsetBone

        # load in anim frames
        #self.applyPoseBones(nodeHeaders, ob)
        bpy.ops.object.mode_set(mode='OBJECT')
        return ob

    def applyBoneParent(self, arm, editBone, parentName):
        for testBone in arm.edit_bones:
            if testBone.name == parentName:
                editBone.parent = testBone
                break
        
    def applyPoseBones(self, nodeHeaders, ob):
        # this function doesn't work.
        bpy.ops.object.mode_set(mode='POSE')
        for nodeId in range(len(nodeHeaders)):
            poseBone = ob.pose.bones[0]
            foundBone = False
            for testBone in ob.pose.bones:
                if testBone.name == nodeHeaders[nodeId].name:
                    poseBone = testBone
                    foundBone = True
                    break
            if foundBone == False:
                print("failed to find pose bone for " + nodeHeaders[nodeId].name)
                continue
            
            for frameId in range(nodeHeaders[nodeId].numAnimFrames):
                #print("applying frame " + str(frameId))
                worldMat = self.createWorldNode(nodeHeaders[nodeId], nodeHeaders, 0)
                baseMat = self.createWorldNode(nodeHeaders[nodeId], nodeHeaders, frameId)
                baseMat.invert()
                worldMat = baseMat @ worldMat
                #worldMat = nodeHeaders[nodeId].transforms[frameId+1].toBlenderMatrix()
                bpy.context.scene.frame_set(frameId+1)
                poseBone.rotation_quaternion = worldMat.to_quaternion()
                poseBone.location = worldMat.to_translation()
                #poseBone.matrix = worldMat
                #poseBone.matrix_basis = worldMat
                
                poseBone.keyframe_insert(data_path='location')
                poseBone.keyframe_insert(data_path='rotation_quaternion')
                poseBone.keyframe_insert(data_path='scale')
        bpy.context.scene.frame_set(1)

    # recurse the tree to make a world matrix
    def createWorldNode(self, node, nodeHeaders, frameIndex):
        #print("createWorldNode " + node.name)
        nodeMat = node.transforms[frameIndex].toBlenderMatrix()
        for parent in nodeHeaders:
            if parent.name == node.parentName:
                parentMat = self.createWorldNode(parent, nodeHeaders, frameIndex)
                nodeMat = parentMat @ nodeMat
                break
        return nodeMat
            
    def loadMesh(self, binFile, reporter, importSettings, armObj, nodeHeaders):
        meshHeader = gham_structs.ghamMeshHeader()
        meshHeader.loadBin(binFile)
        meshHeader.output()

        vertexGroups = []
        if meshHeader.isSkinned == 1:
            tup = struct.unpack("<1i", binFile.read(4))
            numGroups = tup[0]
            print("loading " + str(numGroups) + " vertex groups")
            stringUtil = gham_structs.ghamStringUtil()
            for i in range(numGroups):
                nameBuf = binFile.read(256)
                vertexGroups.append(stringUtil.decodeLoadedString(nameBuf))
        
        # create a list of vert infos used to extract info out of the VBs
        vertInfos = []
        for i in range(meshHeader.numVerts):
            vertInfos.append(ghamBlenderVert())

        self.loadStreams(binFile, meshHeader, vertInfos)
                
        indexBuffer = []
        for i in range(meshHeader.numIndices):
            indexBuffer.append(int(struct.unpack("<1H", binFile.read(2))[0]))

        # if we are gham version 1+ and there is an odd number of indices then we need to read the dummy short.
        # this value was inserted to ensure 4 byte alignment.
        if importSettings.ghamVersion > 0:
            if (meshHeader.numIndices % 2) != 0:
                binFile.read(2)

        if meshHeader.numAnimFrames > 0:
            animStreamDef = gham_structs.ghamStreamDef()
            animStreamDef.loadBin(binFile)
            animStreamDef.output()

            animVertSize = animStreamDef.calcVertSizeInBytes()
            for i in range(meshHeader.numAnimFrames):
                # hacky: grab the pos from the first frame.
                if i == 0:
                    self.extractVertInfo(binFile, animStreamDef, vertInfos)
                else:
                    animBuffer = binFile.read(animVertSize * meshHeader.numVerts)
                    # todo: do something with anim frames.

        # throw away bounds
        binFile.read(6*4);
                            
        self.createBlenderMesh(meshHeader, vertInfos, indexBuffer, importSettings, armObj, nodeHeaders, vertexGroups)

    def loadStreams(self, binFile, meshHeader, vertInfos):
        streamDefs = []
        for i in range(meshHeader.numStreams):
            streamDef = gham_structs.ghamStreamDef()
            streamDef.loadBin(binFile)
            streamDef.output()
            streamDefs.append(streamDef)
            
        for i in range(meshHeader.numStreams):
            self.extractVertInfo(binFile, streamDefs[i], vertInfos)

    # grab any vert info out of the file and interpret it into our vert structure
    def extractVertInfo(self, binFile, streamDef, vertInfos):
        for vertId in range(len(vertInfos)):
            boneIds = []
            boneWeights = []
            for compId in range(len(streamDef.componentDefs)):
                if streamDef.componentDefs[compId].shaderId == "SI_POS":
                    # todo? handle other pos formats
                    vertInfos[vertId].pos[0], vertInfos[vertId].pos[1], vertInfos[vertId].pos[2] = struct.unpack("<3f", binFile.read(3*4))
                elif streamDef.componentDefs[compId].shaderId == "SI_NORMAL":
                    # todo? handle other normal formats
                    vertInfos[vertId].normal[0], vertInfos[vertId].normal[1], vertInfos[vertId].normal[2] = struct.unpack("<3f", binFile.read(3*4))
                elif streamDef.componentDefs[compId].shaderId == "SI_UV1":
                    vertInfos[vertId].uv[0], vertInfos[vertId].uv[1] = struct.unpack("<2f", binFile.read(2*4))
                    # blender uvs are vflipped
                    vertInfos[vertId].uv[1] = (-1.0 * vertInfos[vertId].uv[1]) + 1.0
                elif streamDef.componentDefs[compId].shaderId == "SI_UV2":
                    vertInfos[vertId].uv1[0], vertInfos[vertId].uv1[1] = struct.unpack("<2f", binFile.read(2*4))
                    # blender uvs are vflipped
                    vertInfos[vertId].uv1[1] = (-1.0 * vertInfos[vertId].uv1[1]) + 1.0
                elif streamDef.componentDefs[compId].shaderId == "SI_BONEIDX":
                    tup = struct.unpack("<4B", binFile.read(4))
                    for i in range(4):
                        boneIds.append(tup[i])
                elif streamDef.componentDefs[compId].shaderId == "SI_BONEWEIGHT":
                    tup = struct.unpack("<4f", binFile.read(4*4))
                    for i in range(4):
                        boneWeights.append(tup[i])
                else:
                    binFile.read(streamDef.componentDefs[compId].calcSizeInBytes())
                    # todo: store other types of info from the file
                    # just skipping them here if they are not in the above list.
            for boneIdx in range(len(boneIds)):
                if boneWeights[boneIdx] > 0.01:
                    vertInfos[vertId].groupIds.append(boneIds[boneIdx])
                    vertInfos[vertId].groupWeights.append(boneWeights[boneIdx])
                    
    def createBlenderMesh(self, meshHeader, vertInfos, indexBuffer, importSettings, armObj, nodeHeaders, vertexGroups):
        mesh = bpy.data.meshes.new(meshHeader.name)
        bm = bmesh.new()
        bm.from_mesh(mesh)

        for vertId in range(len(vertInfos)):
            pos = vertInfos[vertId].pos
            normal = vertInfos[vertId].normal
            if importSettings.yUp and meshHeader.parentName == "":
                tempZ = pos[2]
                pos[2] = pos[1]
                pos[1] = tempZ
            pos[1] *= -1
            bmvert = bm.verts.new( pos )
            bmvert.normal = normal

        bm.verts.index_update()
        bm.verts.ensure_lookup_table()

        numTris = int(len(indexBuffer)/3)
        indexStart = 0
        for indexId in range(numTris):
            bm.faces.new( (bm.verts[int(indexBuffer[indexStart])], bm.verts[int(indexBuffer[indexStart+1])], bm.verts[int(indexBuffer[indexStart+2])]) )
            indexStart += 3

        bm.to_mesh(mesh)

        mesh.update()
        mesh.validate()
        obj = bpy.data.objects.new(meshHeader.name, mesh)
        bpy.context.collection.objects.link(obj) # remember scene.update() later!
        bpy.context.view_layer.objects.active = obj
        bpy.context.view_layer.update()

        uvtex = bpy.ops.mesh.uv_texture_add()
        uvtex1 = bpy.ops.mesh.uv_texture_add()
        uv_layer = mesh.uv_layers[0]
        uv_layer1 = mesh.uv_layers[1]
        for ibIdx in range(len(indexBuffer)):
            uv_layer.data[ibIdx].uv = vertInfos[indexBuffer[ibIdx]].uv
            uv_layer1.data[ibIdx].uv = vertInfos[indexBuffer[ibIdx]].uv1
        
        if meshHeader.parentName != "" and armObj != 0:
            for nodeHeader in nodeHeaders:
                if nodeHeader.name == meshHeader.parentName:
                    constraint = obj.constraints.new('COPY_TRANSFORMS')
                    constraint.head_tail = 1.0
                    constraint.target = armObj
                    constraint.subtarget = meshHeader.parentName
                    break

        if meshHeader.isSkinned == 1 and armObj != 0:
            self.createMeshVertexGroups(obj, mesh, armObj, vertexGroups, vertInfos)

    def createMeshVertexGroups(self, obj, mesh, armObj, vertexGroups, vertInfos):
        # parent the new mesh to the armature
        obj.select_set( state = True, view_layer = None)
        armObj.select_set( state = True, view_layer = None)
        bpy.context.view_layer.objects.active = armObj
        bpy.ops.object.parent_set(type='ARMATURE', xmirror=False, keep_transform=False)
        # create the vertex groups
        bpy.context.view_layer.objects.active = obj
        for groupIdx in range(len(vertexGroups)):
            bpy.ops.object.vertex_group_add()
            obj.vertex_groups[groupIdx].name = vertexGroups[groupIdx]
        # add the verts to the appropriate groups
        #bpy.ops.object.mode_set(mode='EDIT')
        #bpy.ops.mesh.select_all(action='DESELECT')
        for vertIdx in range(len(vertInfos)):
            if len(vertInfos[vertIdx].groupIds) > 0:
                #mesh.vertices[vertIdx].select = True
                vertList = []
                vertList.append(vertIdx)
                for groupIdx in range(len(vertInfos[vertIdx].groupIds)):
                    obj.vertex_groups[vertInfos[vertIdx].groupIds[groupIdx]].add(index=vertList, weight=vertInfos[vertIdx].groupWeights[groupIdx], type='ADD')
                #mesh.vertices[vertIdx].select = False
        bpy.ops.object.mode_set(mode='OBJECT')
        bpy.ops.object.select_all(action='DESELECT')

