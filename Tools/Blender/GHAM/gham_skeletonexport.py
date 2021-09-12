if "bpy" not in locals():
    import bpy
if "gham_structs" not in locals():
    from . import gham_structs
if "struct" not in locals():
    import struct
if "gham_exportshared" not in locals():
    from . import gham_exportshared

# util functions for exporting skeletons.
class ghamSkeletonExporter:
    # for all pose bones in armObj, set them back to the edit bone position
    # we do this to make sure meshes are in edit bone stance,
    # which is necessary for skinning.
    def resetPoseBones(self, armObj):
        bpy.context.view_layer.objects.active = armObj
        bpy.ops.object.mode_set(mode='POSE')
        # disable any constraints
        for poseBone in armObj.pose.bones:
            for constraint in poseBone.constraints:
                constraint.mute = True
        bpy.ops.pose.select_all(action='SELECT')
        bpy.ops.pose.transforms_clear()
        print("cleared pose transforms")

    def unresetPoseBones(self, armObj):
        bpy.ops.object.mode_set(mode='POSE')
        # disable any constraints
        for poseBone in armObj.pose.bones:
            for constraint in poseBone.constraints:
                constraint.mute = False
        # we changed the bones back to default pose
        # toggle to a different frame and back to 1 to undo that change.
        bpy.context.scene.frame_set(1)

    def findSkeleton(self):
        for obj in bpy.context.selected_objects:
            if obj.type == 'ARMATURE':
                print("found skeleton")
                return obj
        return 0

    def countSkeletonNodes(self, armObj, exportSettings):
        bpy.context.view_layer.objects.active = armObj
        bpy.ops.object.mode_set(mode='EDIT')
        arm = armObj.data
        numNodes = 0
        for bone in arm.edit_bones:
            parentName = ""
            try:
                parentName = bone.parent.name
            except AttributeError:
                parentName = ""
            
            if exportSettings.generateExtraBones == True and bone.use_connect == False and parentName != "":
                numNodes += 2
            else:
                numNodes += 1
        print("skeleton node count: " + str(numNodes))
        bpy.ops.object.mode_set(mode='OBJECT')
        return numNodes

    def collectActionInfo(self, armObj):
        animList = gham_exportshared.ghamAnimList()
        bpy.context.view_layer.objects.active = armObj

        for action in bpy.data.actions:
            if bpy.context.object.animation_data.action is not None:
                armObj.animation_data.action = action
                if bpy.context.object.animation_data.action.fcurves.items():
                    animInfo = gham_exportshared.ghamAnimInfo()
                    animInfo.name = action.name
                    animInfo.firstFrame = int(round(action.frame_range[0]))
                    animInfo.numFrames = int(round(action.frame_range[1])) - animInfo.firstFrame
                    animInfo.action = action
                    animList.anims.append(animInfo)

        return animList

    def exportAnimList(self, animList, animFile):
        animFile.write("<animSet>\n")
        frameStart = 0
        for anim in animList.anims:
            animFile.write("<Seq name=" + anim.name + " start=" + str(frameStart) + " numFrames=" + str(anim.numFrames) + " loopFrames=" + str(anim.numFrames) + " fps=1/>\n")
            frameStart += anim.numFrames
        animFile.write("</animSet>\n")

    def exportSkeleton(self, armObj, binFile, textFile, animFile, exportSettings):
        animList = self.collectActionInfo(armObj)
        self.exportAnimList(animList, animFile)

        bpy.context.view_layer.objects.active = armObj
        keyframeCount = 0
        for anim in animList.anims:
            keyframeCount += anim.numFrames

        #keyframeCount = bpy.context.view_layer.frame_end - bpy.context.scene.frame_start
        bpy.context.scene.frame_set(bpy.context.scene.frame_start)
        # test hack: no animations
        #keyframeCount = 0
        print("Skeleton anim frames: " + str(keyframeCount))

        ghamNodes = []
        self.collectEditBones(ghamNodes, armObj, keyframeCount, exportSettings)
        if exportSettings.exportRefPose == False:
            # we don't export the pose for default node trans.
            # otherwise we have nothing to skin against.
            self.collectPoseTransforms(ghamNodes, armObj, 0)

        # our transforms are currently world transforms
        # convert them into local transforms and save
        self.convertSkeletonToLocalTransforms(ghamNodes, 0, exportSettings)
        for ghamBone in ghamNodes:
            ghamBone.saveBin(binFile)
            ghamBone.saveText(textFile)

        # grab the anim frames
        if keyframeCount > 0:
            currAnimStart = 1
            for anim in animList.anims:
                armObj.animation_data.action = anim.action
                for i in range(anim.numFrames):
                    bpy.context.scene.frame_set(anim.firstFrame + i)
                    self.collectPoseTransforms(ghamNodes, armObj, i+currAnimStart)	
                    self.convertSkeletonToLocalTransforms(ghamNodes, i+currAnimStart, exportSettings)
                currAnimStart += anim.numFrames

			# old style before we looked at actions.
            #for i in range(keyframeCount):
            #    bpy.context.scene.frame_set(bpy.context.scene.frame_start+i)
            #    self.collectPoseTransforms(ghamNodes, armObj, i+1)
            #    self.convertSkeletonToLocalTransforms(ghamNodes, i+1, exportSettings)
                
        # export anim frames
        for ghamBone in ghamNodes:
            for i in range(ghamBone.numAnimFrames):
                ghamBone.transforms[i+1].saveBin(binFile)
                #ghamBone.transforms[i+1].saveText(textFile)

    def collectEditBones(self, ghamNodes, armObj, numAnimFrames, exportSettings):
        bpy.ops.object.mode_set(mode='EDIT')
        arm = armObj.data
        for bone in arm.edit_bones:
            ghamNode = gham_structs.ghamSkeletonNode()
            ghamNode.setNumFrames(numAnimFrames)
            ghamNode.name = bone.name
            ghamNode.editBoneName = bone.name
            try:
                ghamNode.parentName = bone.parent.name
            except AttributeError:
                ghamNode.parentName = ""
            
            if exportSettings.generateExtraBones == False and bone.use_connect == False:
                # if we're not generating extra head nodes then use the head instead of tail.
                ghamNode.useBoneHead = True
                self.createBoneTransform(bone, bone.head, ghamNode, 0, armObj.matrix_world)
                ghamNodes.append(ghamNode)
            else:
                if bone.use_connect == False and ghamNode.parentName != "":
                    # create a node that doesn't exist in blender to hold the head position
                    ghamHeadNode = gham_structs.ghamSkeletonNode()
                    ghamHeadNode.setNumFrames(numAnimFrames)
                    ghamHeadNode.useBoneHead = True
                    ghamHeadNode.name = bone.name + "head"
                    ghamHeadNode.editBoneName = bone.name
                    ghamHeadNode.parentName = ghamNode.parentName
                    ghamNode.parentName = ghamHeadNode.name
                    self.createBoneTransform(bone, bone.head, ghamHeadNode, 0, armObj.matrix_world)
                    ghamNodes.append(ghamHeadNode)
            
                self.createBoneTransform(bone, bone.tail, ghamNode, 0, armObj.matrix_world)
                ghamNodes.append(ghamNode)
        bpy.ops.object.mode_set(mode='OBJECT')

    def collectPoseTransforms(self, ghamNodes, armObj, frameId):
        bpy.ops.object.mode_set(mode='POSE')
        for ghamBone in ghamNodes:
            poseBone = armObj.pose.bones[ghamBone.editBoneName]
            
            if ghamBone.useBoneHead == True:
                # generated bone means use head, and next bone is the tail of same pose bone
                self.createBoneTransform(poseBone, poseBone.head, ghamBone, frameId, armObj.matrix_world)
            else:
                self.createBoneTransform(poseBone, poseBone.tail, ghamBone, frameId, armObj.matrix_world)
        bpy.ops.object.mode_set(mode='OBJECT')

    def convertSkeletonToLocalTransforms(self, ghamNodes, frameId, exportSettings):
        worldMats = []
        for ghamBone in ghamNodes:
            worldMats.append(ghamBone.transforms[frameId].toBlenderMatrix())
        for i in range(len(ghamNodes)):
            nodeMat = worldMats[i].copy()
            try:
                for j in range(len(ghamNodes)):
                    if (ghamNodes[j].name == ghamNodes[i].parentName):
                        parentMat = worldMats[j].copy()
                        parentMat.invert()
                        nodeMat = parentMat @ nodeMat
                        break
            except AttributeError:
                print("no parent found")
            ghamNodes[i].transforms[frameId].fromBlenderMatrix(nodeMat)

            if exportSettings.yUp == True and ghamNodes[i].parentName == "":
                # rotate the top bone to match y up.
                ghamNodes[i].transforms[frameId].rotateToYUp()

    def createBoneTransform(self, bone, wsPos, ghamNode, frameId, worldMatrix):
        bonemat = worldMatrix @ bone.matrix;
        ghamNode.transforms[frameId].fromBlenderMatrix(bonemat)

    def exportEmptyObjectsAsSkeleton(self, emptyObjects, binFile, textFile, exportSettings):
        print("exporting empty objects as skeleton");
        
        # create an empty top node to hold the skeleton
        topNode = gham_structs.ghamSkeletonNode()
        topNode.setNumFrames(0)
        topNode.useBoneHead = True
        topNode.name = "ModelTop"
        topNode.editBoneName = topNode.name
        topNode.parentName = ""
        topNode.saveBin(binFile)
        topNode.saveText(textFile)

        # export the other empty objects as children.
        for obj in emptyObjects:
            childNode = gham_structs.ghamSkeletonNode()
            childNode.setNumFrames(0)
            childNode.useBoneHead = True
            childNode.name = obj.name
            childNode.editBoneName = childNode.name
            childNode.parentName = topNode.name
            childNode.transforms[0].fromBlenderMatrix(obj.matrix_world)
            if exportSettings.yUp == True:
                # rotate the top bone to match y up.
                childNode.transforms[0].rotateToYUp()  
            childNode.saveBin(binFile)
            childNode.saveText(textFile)

