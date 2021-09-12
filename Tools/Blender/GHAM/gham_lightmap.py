if "bpy" not in locals():
    import bpy

class ghamLightmapper:

    def activateUV(self, obj, index):
        if len(obj.data.uv_layers) > index:
            #print("changing active layer")
            destlayer = obj.data.uv_layers[index]
            obj.data.uv_layers.active = destlayer
            obj.data.uv_layers.active_index = index
            destlayer.active = True
            destlayer.active_render = True

    def doSelectLightmap(self):
        print("gham_lightmap.py select called")

        for obj in bpy.context.selected_objects:
            if obj.type == 'MESH':
                self.activateUV(obj, 1)

                if len(obj.data.materials) > 1:
                    obj.active_material_index = 1

                    bpy.context.view_layer.objects.active = obj
                    bpy.ops.object.mode_set(mode = 'EDIT')
                    bpy.ops.mesh.select_all(action='SELECT')
                    bpy.ops.object.material_slot_assign()
                    bpy.ops.object.mode_set(mode = 'OBJECT')

        print("finished")

    def doUnselectLightmap(self):
        print("gham_lightmap.py unselect called")

        for obj in bpy.context.selected_objects:
            if obj.type == 'MESH':
                self.activateUV(obj, 0)

                if len(obj.data.materials) > 0:
                    obj.active_material_index = 0

                    bpy.context.view_layer.objects.active = obj
                    bpy.ops.object.mode_set(mode = 'EDIT')
                    bpy.ops.mesh.select_all(action='SELECT')
                    bpy.ops.object.material_slot_assign()
                    bpy.ops.object.mode_set(mode = 'OBJECT')

        print("finished")

    def doCreateLightmapMaterial(self):
        print("doCreateLightmapMaterial called")
        mat = bpy.data.materials.new("Lightmap")

        for obj in bpy.context.selected_objects:
            if obj.type == 'MESH':
                if (len(obj.data.materials) < 2):
                    obj.data.materials.append(mat)
                else:
                    obj.data.materials[1] = mat

        print("finished")
