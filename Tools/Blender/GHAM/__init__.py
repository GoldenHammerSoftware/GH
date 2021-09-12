bl_info = {
    "name": "GHAwesomeModel",
    "author": "Golden Hammer Software",
    "version": (1,0),
    "blender": (2, 80, 0),
    "location": "File > Import-Export",
    "description": "Export Golden Hammer Awesome Model",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "http://goldenhammersoftware.com",
    "category": "Import-Export"}

import imp
import os
import sys
import bpy
from bpy_extras.io_utils import ExportHelper
from bpy.props import *

if "gham_export" in locals():
    imp.reload( gham_export )
else:
    from . import gham_export
if "gham_import" in locals():
    imp.reload( gham_import )
else:
    from . import gham_import
if "gham_exportshared" not in locals():
    from . import gham_exportshared
else:
    from . import gham_exportshared
if "gham_lightmap" in locals():
    imp.reload( gham_lightmap )
else:
    from . import gham_lightmap

class ExportGHAM(bpy.types.Operator, ExportHelper):
    bl_idname       = "export_gham.gham";
    bl_label        = "GH Awesome Model Export";
    bl_options      = {'PRESET'};
    
    filename_ext    = ".gham";
    filepath : bpy.props.StringProperty(
        name="File Path",
        description="GHAM File Path",
        maxlen= 1024, default= "")
    exportText : BoolProperty( name = "Text Export", description = "Export a .txt gham description", default = True )
    transformVerts : BoolProperty( name = "Transform Unparented Verts", description = "Multiply vert coords by world transform", default = True )
    yUp : BoolProperty( name = "Y is up", description = "swap y and z", default = True )
    exportUV : BoolProperty( name = "Export UV", description = "export UV", default = True )
    exportRefPose : BoolProperty( name = "Export Mesh RefPose", description = "Export mesh in bone rest position", default = True)
    generateExtraBones : BoolProperty( name = "Add extra bones", description = "Generate head bones for detached bones", default = True)

    def execute(self, context):
        print("GHAM export execute " + self.properties.filepath)
        binFile = open(self.properties.filepath, "wb");
        if self.exportText:
            textFile = open(self.properties.filepath+".txt", "w");
        else:
            textFile = 0
        animFile = open(self.properties.filepath+"anims.xml", "w");
		        
        exportSettings = gham_exportshared.ghamExportSettings()
        exportSettings.exportText = self.exportText
        exportSettings.transformVerts = self.transformVerts
        exportSettings.yUp = self.yUp
        exportSettings.exportUV = self.exportUV
        exportSettings.exportRefPose = self.exportRefPose
        exportSettings.generateExtraBones = self.generateExtraBones
        
        ghamExporter = gham_export.ghamExporter()
        ghamExporter.export(binFile, textFile, animFile, exportSettings, self)
        binFile.close()
        if self.exportText:
            textFile.close()
        animFile.close()
        return {'FINISHED'};

    def invoke( self, context, event ):
        wm = context.window_manager
        wm.fileselect_add( self )
        return { 'RUNNING_MODAL' }

def export_menu_func(self, context):
    self.layout.operator(ExportGHAM.bl_idname, text="GH Awesome Model(.gham)");

class ImportGHAM(bpy.types.Operator):
    bl_idname       = "import_gham.gham"
    bl_label        = "GH Awesome Model Import"

    filepath : bpy.props.StringProperty(
        name="File Path",
        description="GHAM File Path",
        maxlen= 1024, default= "")
    yUp : BoolProperty( name = "y is up", description = "swap y and z", default = True )
    preserveBoneRot : BoolProperty( name = "Preserve Bone Rot", description = "Add extra rot nodes", default = False)

    def execute(self, context):
        print("Importing GHAM " + self.properties.filepath)
        importSettings = gham_import.ghamImportSettings()
        importSettings.yUp = self.yUp
        importSettings.preserveBoneRot = self.preserveBoneRot
        
        file = open(self.properties.filepath, "rb");
        ghamImporter = gham_import.ghamImporter()
        ghamImporter.doImport(file, self, importSettings)
        file.close()
        return {'FINISHED'}

    def invoke( self, context, event ):
        wm = context.window_manager
        wm.fileselect_add( self )
        return { 'RUNNING_MODAL' }

def import_menu_func(self, context):
    self.layout.operator(ImportGHAM.bl_idname, text="GH Awesome Model(.gham)");

class SelectLightmapOperator(bpy.types.Operator):
    bl_idname       = "selectlightmap.gham"
    bl_label        = "Select Lightmap Settings"

    def execute(self, context):
        lightmapper = gham_lightmap.ghamLightmapper()
        lightmapper.doSelectLightmap()
        return {"FINISHED"}

def select_lightmap_func(self, context):
    self.layout.operator(SelectLightmapOperator.bl_idname, text="GH Activate Lightmap");

class UnselectLightmapOperator(bpy.types.Operator):
    bl_idname       = "unselectlightmap.gham"
    bl_label        = "Unselect Lightmap Settings"

    def execute(self, context):
        lightmapper = gham_lightmap.ghamLightmapper()
        lightmapper.doUnselectLightmap()
        return {"FINISHED"}

def unselect_lightmap_func(self, context):
    self.layout.operator(UnselectLightmapOperator.bl_idname, text="GH Deactivate Lightmap");

class CreateLightmapMaterialOperator(bpy.types.Operator):
    bl_idname       = "createlightmapmat.gham"
    bl_label        = "Create Lightmap Material"

    def execute(self, context):
        lightmapper = gham_lightmap.ghamLightmapper()
        lightmapper.doCreateLightmapMaterial()
        return {"FINISHED"}

def create_lightmapmaterial_func(self, context):
    self.layout.operator(CreateLightmapMaterialOperator.bl_idname, text="GH Create Lightmap material");

classes = (
    ExportGHAM,
    ImportGHAM,
    SelectLightmapOperator,
    UnselectLightmapOperator,
    CreateLightmapMaterialOperator,
)

def register():
    from bpy.utils import register_class
    for cls in classes:
        register_class(cls)

    bpy.types.TOPBAR_MT_file_export.append(export_menu_func)
    bpy.types.TOPBAR_MT_file_import.append(import_menu_func)
    bpy.types.TOPBAR_MT_edit.append(select_lightmap_func)
    bpy.types.TOPBAR_MT_edit.append(unselect_lightmap_func)
    bpy.types.TOPBAR_MT_edit.append(create_lightmapmaterial_func)

def unregister():
    from bpy.utils import unregister_class
    for cls in reversed(classes):
        unregister_class(cls)

    bpy.types.TOPBAR_MT_file_export.remove(export_menu_func)
    bpy.types.TOPBAR_MT_file_import.remove(import_menu_func)
    bpy.types.TOPBAR_MT_edit.remove(select_lightmap_func)
    bpy.types.TOPBAR_MT_edit.remove(unselect_lightmap_func)
    bpy.types.TOPBAR_MT_edit.remove(create_lightmapmaterial_func)

if __name__ == "__main__":
    register()

