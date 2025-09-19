bl_info = {
    "name": "GBA Model Metadata Helper",
    "author": "GitHub Copilot",
    "version": (1, 1),
    "blender": (4, 5, 0),
    "location": "View3D > Sidebar > GBA Model Metadata Helper",
    "description": "(safe for Blender 4.5+)",
    "category": "Mesh",
}

import bpy
import bmesh

class MESH_PT_face_attribute_setter(bpy.types.Panel):
    bl_label = "GBA Model Metadata Helper"
    bl_idname = "MESH_PT_face_attribute_setter"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "GBA Model Metadata Helper"

    def draw(self, context):
        layout = self.layout

        # Initialization section (appears above editing tools)
        init_box = layout.box()
        init_box.label(text="Initialization", icon='FILE')
        init_box.operator("mesh.initialize_face_brightness_file", text="Initialize File")

        # Model Scale section (between initialization and face brightness)
        scale_box = layout.box()
        scale_box.label(text="Model Scale", icon='FULLSCREEN_ENTER')
        obj = context.active_object
        if obj is None or obj.type != 'MESH':
            scale_box.label(text="Select a mesh object.", icon='ERROR')
        else:
            if 'engine_scale' in obj:
                # Directly expose the ID property; UI metadata already set during initialization
                try:
                    scale_box.prop(obj, '["engine_scale"]', text="Engine Scale")
                except Exception:
                    scale_box.label(text="engine_scale property inaccessible.", icon='ERROR')
            else:
                scale_box.label(text="Run Initialization first.", icon='ERROR')

        # Face brightness editing section
        box = layout.box()
        box.label(text="Face Brightness", icon='LIGHT')

        obj = context.active_object
        # Only allow editing in Edit Mode and when a mesh is selected
        if obj is None or obj.type != 'MESH' or bpy.context.object.mode != 'EDIT':
            box.label(text="Switch to Edit Mode.", icon='ERROR')
            return
        bm = bmesh.from_edit_mesh(obj.data)
        # Check if attribute exists first
        attr_layer = bm.faces.layers.int.get("engine_brightness")
        if not attr_layer:
            box.label(text="Run Initialization first.", icon='ERROR')
            return

        selected_faces = [f for f in bm.faces if f.select]
        if not selected_faces:
            box.label(text="Select at least one face.", icon='ERROR')
            return
        # When exactly one face is selected the IntProperty getter shows the face value.
        # Changing the value now auto-applies to selected faces.
        box.prop(context.scene, "fa_attr_value")


class MESH_OT_initialize_file(bpy.types.Operator):
    bl_idname = "mesh.initialize_face_brightness_file"
    bl_label = "Initialize Face Brightness"
    bl_description = "Ensure 'engine_brightness' face integer attribute layer exists on the active mesh"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        obj = context.active_object
        if obj is None or obj.type != 'MESH':
            self.report({'ERROR'}, "Select a mesh object.")
            return {'CANCELLED'}

        attr_name = "engine_brightness"
        # Add / normalize custom object property engine_scale (int 1-99, default 10)
        added_engine_scale = False
        if 'engine_scale' not in obj:
            obj['engine_scale'] = 10
            added_engine_scale = True
        else:
            try:
                obj['engine_scale'] = max(1, min(99, int(obj['engine_scale'])))
            except Exception:
                obj['engine_scale'] = 10
        # Apply UI metadata (Blender 3.4+ id_properties_ui API)
        try:
            ui = obj.id_properties_ui("engine_scale")
            ui.update(min=1, max=99, default=10)
        except Exception:
            pass

        if obj.mode == 'EDIT':
            bm = bmesh.from_edit_mesh(obj.data)
            layer = bm.faces.layers.int.get(attr_name)
            if layer:
                msg = []
                if added_engine_scale:
                    msg.append("Added engine_scale")
                msg.append(f"'{attr_name}' layer already existed")
                self.report({'INFO'}, "; ".join(msg))
                return {'CANCELLED'}
            layer = bm.faces.layers.int.new(attr_name)
            # Assign value 7 to every face
            for f in bm.faces:
                f[layer] = 7
            bmesh.update_edit_mesh(obj.data)
        else:
            bm = bmesh.new()
            bm.from_mesh(obj.data)
            layer = bm.faces.layers.int.get(attr_name)
            if layer:
                bm.free()
                msg = []
                if added_engine_scale:
                    msg.append("Added engine_scale")
                msg.append(f"'{attr_name}' layer already existed")
                self.report({'INFO'}, "; ".join(msg))
                return {'CANCELLED'}
            layer = bm.faces.layers.int.new(attr_name)
            for f in bm.faces:
                f[layer] = 7
            bm.to_mesh(obj.data)
            bm.free()

        extra = " and added engine_scale" if added_engine_scale else ""
        self.report({'INFO'}, f"Initialized '{attr_name}' layer (faces=7){extra}.")
        return {'FINISHED'}

def register():
    bpy.utils.register_class(MESH_PT_face_attribute_setter)
    bpy.utils.register_class(MESH_OT_initialize_file)
    # Hidden storage for the last user-entered value (used when multiple faces selected)
    bpy.types.Scene.fa_attr_value_storage_internal = bpy.props.IntProperty(
        default=1, options={'HIDDEN'})
    # Tracks if user manually edited after last selection sync
    bpy.types.Scene.fa_attr_value_manual_override = bpy.props.BoolProperty(
        default=False, options={'HIDDEN'})
    # Stores last single-face index used to seed the value
    bpy.types.Scene.fa_attr_last_face_index = bpy.props.IntProperty(
        default=-1, options={'HIDDEN'})

    # Dynamic getter / setter so the UI value mirrors a single selected face's attribute
    def _get_fa_attr_value(self):
        obj = bpy.context.active_object
        if obj and obj.type == 'MESH' and obj.mode == 'EDIT':
            try:
                bm = bmesh.from_edit_mesh(obj.data)
            except Exception:
                return self.fa_attr_value_storage_internal
            selected_faces = [f for f in bm.faces if f.select]
            if len(selected_faces) == 1:
                face = selected_faces[0]
                # If selection changed, seed from face attribute and clear manual override
                if face.index != self.fa_attr_last_face_index:
                    layer = bm.faces.layers.int.get("engine_brightness")
                    if layer:
                        self.fa_attr_value_storage_internal = face[layer]
                    self.fa_attr_last_face_index = face.index
                    self.fa_attr_value_manual_override = False
                # If user has manually overridden, just return stored value
                return self.fa_attr_value_storage_internal
            else:
                # Reset last face index when not exactly one selected so next single selection resyncs
                self.fa_attr_last_face_index = -1
        return self.fa_attr_value_storage_internal

    def _set_fa_attr_value(self, value):
        new_val = max(0, min(7, int(value)))
        self.fa_attr_value_storage_internal = new_val
        self.fa_attr_value_manual_override = True
        # Live-apply to selected faces if possible
        obj = bpy.context.active_object
        if obj and obj.type == 'MESH' and obj.mode == 'EDIT':
            try:
                bm = bmesh.from_edit_mesh(obj.data)
            except Exception:
                return
            layer = bm.faces.layers.int.get("engine_brightness")
            if not layer:
                return
            changed = 0
            for f in bm.faces:
                if f.select:
                    f[layer] = new_val
                    changed += 1
            if changed:
                bmesh.update_edit_mesh(obj.data, loop_triangles=False, destructive=False)

    bpy.types.Scene.fa_attr_value = bpy.props.IntProperty(
        name="Engine Brightness",
        description="Engine brightness value (shows selected face attribute if exactly one face is selected)",
        min=0, max=7,
        get=_get_fa_attr_value,
        set=_set_fa_attr_value
    )

def unregister():
    bpy.utils.unregister_class(MESH_PT_face_attribute_setter)
    bpy.utils.unregister_class(MESH_OT_initialize_file)
    del bpy.types.Scene.fa_attr_value
    del bpy.types.Scene.fa_attr_value_storage_internal
    del bpy.types.Scene.fa_attr_value_manual_override
    del bpy.types.Scene.fa_attr_last_face_index

if __name__ == "__main__":
    register()