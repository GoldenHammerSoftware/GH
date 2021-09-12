if "bpy" not in locals():
    import bpy
if "gham_structs" not in locals():
    from . import gham_structs
if "struct" not in locals():
    import struct
if "gham_exportshared" not in locals():
    from . import gham_exportshared
if "gham_skeletonexport" not in locals():
    from . import gham_skeletonexport

import mathutils
import math

class ghamExporter:
    def export(self, binFile, textFile, animFile, exportSettings, reporter):
        print("gham_export.py export called")
        bpy.context.view_layer.update()
        
        header = gham_structs.ghamHeader()
        header.numMeshes = self.countMeshes()

        # make a copy of selected objects, since we will need to select individual.
        selectedObjects = []
        for obj in bpy.context.selected_objects:
            selectedObjects.append(obj)

		# grab any empty objects so we can turn those into bones.
        selectedEmptyObjects = []
        for obj in selectedObjects:
            if obj.type == 'EMPTY':
                selectedEmptyObjects.append(obj)
        print("found " + str(len(selectedEmptyObjects)) + " empty objects found")
    
        ghamSkelExport = gham_skeletonexport.ghamSkeletonExporter()
        armObj = ghamSkelExport.findSkeleton()
        if armObj != 0:
            header.numSkeletonNodes = ghamSkelExport.countSkeletonNodes(armObj, exportSettings)
        else:
            header.numSkeletonNodes = len(selectedEmptyObjects) + 1;

        header.saveBin(binFile)
        if exportSettings.exportText:
            header.saveText(textFile)
        print("exported gham header")
        
        if armObj != 0:
            ghamSkelExport.exportSkeleton(armObj, binFile, textFile, animFile, exportSettings)
            print("exported skeleton")
            if exportSettings.exportRefPose == True:
                ghamSkelExport.resetPoseBones(armObj)
        else:
            ghamSkelExport.exportEmptyObjectsAsSkeleton(selectedEmptyObjects, binFile, textFile, exportSettings);
			                 
        for obj in selectedObjects:
            if obj.type == 'MESH':
                self.exportMesh(obj, binFile, textFile, exportSettings, reporter)

        if exportSettings.exportRefPose == True and armObj != 0:
            ghamSkelExport.unresetPoseBones(armObj)

    def exportMesh(self, obj, binFile, textFile, exportSettings, reporter):
        print("exporting mesh " + obj.name)
        depsgraph = bpy.context.evaluated_depsgraph_get()
        mesh = obj.evaluated_get(depsgraph).to_mesh()
    
        numGroupsPerVert = 4
        indices = []
        vertIds = []
        uvs = []
        uv2s = []
        self.collectVerts(mesh, vertIds, uvs, uv2s, indices, reporter, exportSettings)
        
        if len(indices) < 3:
            reporter.report({"ERROR"}, "No triangles found in mesh! " + obj.name)
            if exportSettings.exportText:
                textFile.write("No triangles found in mesh! " + obj.name + "\n")
        print("  collected verts(" + str(len(vertIds)) +") uvs("+ str(len(uvs)) + ") indices("+ str(len(indices)) + ")")
             
        meshHeader = gham_structs.ghamMeshHeader()
        meshHeader.numStreams = 1 # 1 stream unless skinned
        meshHeader.numVerts = len(vertIds)
        meshHeader.numIndices = len(indices)
        meshHeader.name = obj.name
        meshHeader.isSkinned = 0
        for constraint in obj.constraints:
            if constraint.type == 'COPY_TRANSFORMS':
                meshHeader.parentName = constraint.subtarget
                break
 
        vertexGroupsPerVert = []
        self.collectVertexGroupsPerVert(obj, mesh, vertIds, vertexGroupsPerVert, numGroupsPerVert)
        if len(vertexGroupsPerVert) > 0:
            meshHeader.isSkinned = 1
            meshHeader.numStreams += 1 # second stream for bone weights
            vertexGroupsUsed = []
            numUsedGroups = self.collectUsedVertexGroups(obj, vertexGroupsPerVert, vertexGroupsUsed)
            print("  collected " + str(numUsedGroups) + " vertex groups")

        meshHeader.saveBin(binFile)
        if exportSettings.exportText:
            meshHeader.saveText(textFile)
        #print("  exported meshHeader")
    
        if meshHeader.isSkinned == 1:
            self.exportMeshVertexGroupHeader(obj, numUsedGroups, vertexGroupsUsed, binFile, textFile)
        
        #todo: if vertex anim, set up multiple streams.
        streamDef = gham_structs.ghamStreamDef()
        self.createStaticStreamDef(mesh, streamDef, len(uvs))
        streamDef.saveBin(binFile)
        if exportSettings.exportText:
            streamDef.saveText(textFile)
    
        if meshHeader.isSkinned == 1:
            skinStreamDef = gham_structs.ghamStreamDef()
            self.createSkinStreamDef(skinStreamDef, numGroupsPerVert)
            skinStreamDef.saveBin(binFile)
            if exportSettings.exportText:
                skinStreamDef.saveText(textFile)

        minPoint = gham_structs.ghamVec3()
        maxPoint = gham_structs.ghamVec3()
        self.exportVerts(obj, mesh, vertIds, uvs, uv2s, indices, minPoint, maxPoint, binFile, textFile, exportSettings, meshHeader)
        if meshHeader.isSkinned == 1:
            self.exportSkinVerts(vertexGroupsPerVert, vertexGroupsUsed, numGroupsPerVert, binFile)
                
        self.exportIndices(indices, binFile, textFile, exportSettings)

        minPoint.saveBin(binFile)
        maxPoint.saveBin(binFile)
        if exportSettings.exportText:
            textFile.write("Bounds:\n")
            minPoint.saveText(textFile)
            textFile.write("\n")
            maxPoint.saveText(textFile)
            textFile.write("\n\n")

        #print("  exported buffers")
            
    def countMeshes(self):
        numMeshes = 0;
        for obj in bpy.context.selected_objects:
            if obj.type == 'MESH':
                numMeshes += 1
        return numMeshes

    def collectVerts(self, mesh, vertIds, uvs, uv2s, indices, reporter, exportSettings):
        uv0 = [0,0]
        uv1 = [0,0]
        uv2 = [0,0]
        uv3 = [0,0]
        uv20 = [0,0]
        uv21 = [0,0]
        uv22 = [0,0]
        uv23 = [0,0]
        hasUV = False

        if exportSettings.exportUV == True:
            if len(mesh.uv_layers) > 0:
                hasUV = True
        #if hasUV == False:
        #    print("model has no UV")
        
        # blender can store a different uv per vert based on face
        # we need to interpret this as different verts
        for poly in mesh.polygons:
            if len(poly.vertices) == 3:
                for polyVertIdx in range(len(poly.vertices)):
                    if hasUV == True:
                        uv0 = self.createGHUV(mesh.uv_layers[0].data[ poly.loop_indices[polyVertIdx] ].uv.to_tuple())
                        if len(mesh.uv_layers) > 1:
                            uv20 = self.createGHUV(mesh.uv_layers[1].data[ poly.loop_indices[polyVertIdx] ].uv.to_tuple())
                    self.storeVert(poly.vertices[polyVertIdx], uv0, uv20, vertIds, uvs, uv2s, indices, hasUV)
            elif len(poly.vertices) == 4:
                # we found a quad, do an arbitrary triangulation.
                if hasUV == True:
                    uv0 = self.createGHUV(mesh.uv_layers[0].data[ poly.loop_indices[0] ].uv.to_tuple())
                    uv1 = self.createGHUV(mesh.uv_layers[0].data[ poly.loop_indices[1] ].uv.to_tuple())
                    uv2 = self.createGHUV(mesh.uv_layers[0].data[ poly.loop_indices[2] ].uv.to_tuple())
                    uv3 = self.createGHUV(mesh.uv_layers[0].data[ poly.loop_indices[3] ].uv.to_tuple())
                    if len(mesh.uv_layers) > 1:
                        uv20 = self.createGHUV(mesh.uv_layers[1].data[ poly.loop_indices[0] ].uv.to_tuple())
                        uv21 = self.createGHUV(mesh.uv_layers[1].data[ poly.loop_indices[1] ].uv.to_tuple())
                        uv22 = self.createGHUV(mesh.uv_layers[1].data[ poly.loop_indices[2] ].uv.to_tuple())
                        uv23 = self.createGHUV(mesh.uv_layers[1].data[ poly.loop_indices[3] ].uv.to_tuple())

                # we know the second time we add the same vert that it is going to be a dup
                # so we store out the result and the second time just add an index
                v0Idx = self.storeVert(poly.vertices[0], uv0, uv20, vertIds, uvs, uv2s, indices, hasUV)
                self.storeVert(poly.vertices[1], uv1, uv21, vertIds, uvs, uv2s, indices, hasUV)
                v2Idx = self.storeVert(poly.vertices[2], uv2, uv22, vertIds, uvs, uv2s, indices, hasUV)
                indices.append(v0Idx)
                indices.append(v2Idx)
                self.storeVert(poly.vertices[3], uv3, uv23, vertIds, uvs, uv2s, indices, hasUV)
            else:
                reporter.report({"ERROR"}, "Found a polygon of length " + str(len(poly.vertices)))


    def createGHUV(self, srcUV):
        destUV = srcUV
        destUV = [ round( x, 10 ) for x in destUV ]
        # blender uvs are yflipped
        destUV[1] = (-1.0 * destUV[1]) + 1.0
        return destUV

    def storeVert(self, vertId, uv, uv2, vertIds, uvs, uv2s, indices, hasUV):
        # first check for a dup vertId/uv in existing list
        dupId = -1
        for idx in range(len(vertIds)):
            if vertId == vertIds[idx]:
                uvMatch = True
                if hasUV == True:
                    if uv != uvs[idx]:
                        uvMatch = False
                    if uv2 != uv2s[idx]:
                        uvMatch = False
                if uvMatch == True:
                    dupId = idx
                    break
        if dupId != -1:
            # if we found a dup, then use that instead
            indices.append(dupId)
            return dupId
        else:
            # otherwise store new vals in the list
            vertIds.append(vertId)
            if hasUV == True:
                uvs.append(uv)
                uv2s.append(uv2)
            vertIdx = len(vertIds)-1
            indices.append(vertIdx)
            return vertIdx

    def createStaticStreamDef(self, mesh, streamDef, uvLen):
        # assume all streams have pos
        posDef = gham_structs.ghamComponentDef()
        posDef.dataCount = 3
        posDef.dataType = "CT_FLOAT"
        posDef.shaderId = "SI_POS"
        streamDef.componentDefs.append(posDef)
        # all blender models have normals
        # todo: support a checkbox for normals or no
        normDef = gham_structs.ghamComponentDef()
        normDef.dataCount = 3
        normDef.dataType = "CT_FLOAT"
        normDef.shaderId = "SI_NORMAL"
        streamDef.componentDefs.append(normDef)
        # todo? vert colors
        if uvLen > 0:
            if len(mesh.uv_layers) > 0:
                uv1Def = gham_structs.ghamComponentDef()
                uv1Def.dataCount = 2
                uv1Def.dataType = "CT_FLOAT"
                uv1Def.shaderId = "SI_UV1"
                streamDef.componentDefs.append(uv1Def)
            if len(mesh.uv_layers) > 1:
                uv2Def = gham_structs.ghamComponentDef()
                uv2Def.dataCount = 2
                uv2Def.dataType = "CT_FLOAT"
                uv2Def.shaderId = "SI_UV2"
                streamDef.componentDefs.append(uv2Def)
            # todo? more than two uvs

    def createSkinStreamDef(self, streamDef, numBonesPerVert):
        idxDef = gham_structs.ghamComponentDef()
        idxDef.dataCount = numBonesPerVert
        idxDef.dataType = "CT_UBYTE"
        idxDef.shaderId = "SI_BONEIDX"
        streamDef.componentDefs.append(idxDef)
        weightDef = gham_structs.ghamComponentDef()
        weightDef.dataCount = numBonesPerVert
        weightDef.dataType = "CT_FLOAT"
        weightDef.shaderId = "SI_BONEWEIGHT"
        streamDef.componentDefs.append(weightDef)
    
    def collectVertexGroupsPerVert(self, obj, mesh, vertIds, vertexGroupsPerVert, maxGroupsPerVert):
        # early out if this mesh doesn't use groups.
        if len(obj.vertex_groups) == 0:
            return
             
        # collect the top maxGroupsPerVert groups for each vert
        for vertIdx in range(len(vertIds)):
            vgInfo = gham_exportshared.ghamVertexGroupInfo()
            
            if maxGroupsPerVert <= len(mesh.vertices[vertIds[vertIdx]].groups):
                # we don't need to prune groups
                for element in mesh.vertices[vertIds[vertIdx]].groups:
                    vgInfo.groups.append(gham_exportshared.ghamVertexGroupEntry(element.group, element.weight))
            else:
                # pick the top maxGroupsPerVert vertex groups
                for element in mesh.vertices[vertIds[vertIdx]].groups:
                    # first push them in until we have maxGroupsPerVert
                    if len(vgInfo.groups) < maxGroupsPerVert:
                        vgInfo.groups.append(gham_exportshared.ghamVertexGroupEntry(element.group,element.weight))
                    else:
                        # if this weight is higher than any existing weights, add it and remove the lowest.
                        lowestIdx = -1
                        for testIdx in range(len(vertex)):
                            if element.weight > vgInfo.groups[testIdx].weight:
                                if lowestIdx == -1:
                                    lowestIdx = testIdx
                                else:
                                    if vgInfo.groups[testIdx].weight < vgInfo.groups[lowestIdx].weight:
                                        lowestIdx = testIdx
                        if lowestIdx != -1:
                            del vgInfo.groups[lowestIdx]
                            vgInfo.groups.append(gham_exportshared.ghamVertexGroupEntry(element.group, element.weight))
        
            vertexGroupsPerVert.append(vgInfo)

    def collectUsedVertexGroups(self, obj, vertexGroupsPerVert, vertexGroupsUsed):
        # first figure out what groups we are using
        isGroupUsed = []
        for group in obj.vertex_groups:
            isGroupUsed.append(False)
            vertexGroupsUsed.append(-1)
        for vgpv in vertexGroupsPerVert:
            for group in vgpv.groups:
                if group.weight > 0:
                    isGroupUsed[group.groupIdx] = True
        # then make a list of blender index -> exported index
        numUsedGroups = 0
        for idx in range(len(isGroupUsed)):
            if isGroupUsed[idx] == True:
                vertexGroupsUsed[idx] = numUsedGroups
                numUsedGroups += 1
                #print("group " + str(idx) + " " + str(obj.vertex_groups[idx].index) + " " + obj.vertex_groups[idx].name)
        return numUsedGroups

    def exportMeshVertexGroupHeader(self, obj, numUsedGroups, vertexGroupsUsed, binFile, textFile):
        textFile.write("Num Vertex Groups: " + str(numUsedGroups) + "\n")
        data = struct.pack("<1i", numUsedGroups)
        binFile.write(data)
        for groupIdx in range(len(vertexGroupsUsed)):
            if vertexGroupsUsed[groupIdx] != -1:
                textFile.write(obj.vertex_groups[groupIdx].name + " ")
                data = struct.pack("<256s", obj.vertex_groups[groupIdx].name.encode('utf-8'))
                binFile.write(data)
        textFile.write("\n")
            
    def exportVerts(self, obj, mesh, vertIds, uvs, uv2s, indices, minPoint, maxPoint, binFile, textFile, exportSettings, meshHeader):
        vec3 = gham_structs.ghamVec3()
        vec2 = gham_structs.ghamVec2()

        vertIdx = 0
        
        if exportSettings.exportText:
            textFile.write("VERT DATA\n")
        # this is very odd: if we don't multiply vert.co by something it is corrupt
        # so we multiply it by identity if we don't want transformed verts.
        worldMatrix = obj.matrix_world.copy()

        #print("Exporting mesh with obj matrix_world")
        #print(worldMatrix)

        rotation = mathutils.Matrix.Rotation(math.radians(-90), 4, 'X')
        if exportSettings.transformVerts == False or meshHeader.parentName != "":
            worldMatrix.identity()
        if exportSettings.yUp == True and meshHeader.parentName == "":
            worldMatrix = rotation @ worldMatrix

        worldPos, worldRot, worldScale = worldMatrix.decompose();

        for vertIdx in range(len(vertIds)):
            vec3.xyz = worldMatrix @ mesh.vertices[vertIds[vertIdx]].co
            vec3.saveBin(binFile)
            if exportSettings.exportText:
                textFile.write("pos: ")
                vec3.saveText(textFile)
                textFile.write("\n")
    
            # keep track of bounds as we iterate verts.
            if vertIdx == 0:
                minPoint.xyz = vec3.xyz.copy()
                maxPoint.xyz = vec3.xyz.copy()
            else:
                for vertPos in range(3):
                    if vec3.xyz[vertPos] < minPoint.xyz[vertPos]:
                        minPoint.xyz[vertPos] = vec3.xyz[vertPos]
                    if vec3.xyz[vertPos] > maxPoint.xyz[vertPos]:
                        maxPoint.xyz[vertPos] = vec3.xyz[vertPos]

            vec3.xyz = mesh.vertices[vertIds[vertIdx]].normal
            if meshHeader.parentName == "":
                vec3.xyz = worldRot @ vec3.xyz;
                #vec3.xyz = rotation @ mesh.vertices[vertIds[vertIdx]].normal
                #vec3.xyz *= -1
            vec3.saveBin(binFile)
            if exportSettings.exportText:
                textFile.write("norm: ")
                vec3.saveText(textFile)
                textFile.write("\n")

            if len(uvs) > 0:
                vec2.xy = uvs[vertIdx]
                vec2.saveBin(binFile)
                if exportSettings.exportText:
                    textFile.write("uv1: ")
                    vec2.saveText(textFile)
                    textFile.write("\n")
                if len(mesh.uv_layers) > 1:
                    vec2.xy = uv2s[vertIdx]
                    vec2.saveBin(binFile)
                    if exportSettings.exportText:
                        textFile.write("uv2: ")
                        vec2.saveText(textFile)
                        textFile.write("\n")

            vertIdx += 1
        if exportSettings.exportText:
            textFile.write("\n\n")

    def exportSkinVerts(self, vertexGroupsPerVert, vertexGroupsUsed, numGroupsPerVert, binFile):
        for vgpv in vertexGroupsPerVert:
            for vgpvIdx in range(numGroupsPerVert):
                if vgpvIdx < len(vgpv.groups):
                    data = struct.pack("<1B", vertexGroupsUsed[vgpv.groups[vgpvIdx].groupIdx])
                    binFile.write(data)
                else:
                    data = struct.pack("<1B", 0)
                    binFile.write(data)
            for vgpvIdx in range(numGroupsPerVert):
                if vgpvIdx < len(vgpv.groups):
                    data = struct.pack("<1f", vgpv.groups[vgpvIdx].weight)
                    binFile.write(data)
                else:
                    data = struct.pack("<1f", 0)
                    binFile.write(data)

    def exportIndices(self, indices, binFile, textFile, exportSettings):
        for idx in indices:
            data = struct.pack("<1H", idx)
            binFile.write(data)

        # add an extra short if there's an odd number.
        # otherwise everything after this won't be 4 byte aligned.
        # this was added for gham version 1
        if (len(indices) % 2) != 0:
            data = struct.pack("<1H", 0)
            binFile.write(data)

        if exportSettings.exportText:
            textFile.write("INDEX BUFFER\n")
            for idx in indices:
                textFile.write(str(idx) + " ")
            textFile.write("\n")

