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
        # Show contextual editing UI without blocking Export section
        if obj is None or obj.type != 'MESH':
            box.label(text="Select a mesh object.", icon='ERROR')
        elif obj.mode != 'EDIT':
            box.label(text="Switch to Edit Mode.", icon='ERROR')
        else:
            bm = bmesh.from_edit_mesh(obj.data)
            attr_layer = bm.faces.layers.int.get("engine_brightness")
            if not attr_layer:
                box.label(text="Run Initialization (no engine_brightness).", icon='ERROR')
            else:
                selected_faces = [f for f in bm.faces if f.select]
                if not selected_faces:
                    box.label(text="Select at least one face.", icon='ERROR')
                else:
                    # When exactly one face is selected the IntProperty getter shows the face value.
                    # Changing the value auto-applies to selected faces.
                    box.prop(context.scene, "fa_attr_value")
                # Spreadsheet button still useful even without selection if layer exists
                box.operator("mesh.open_face_attribute_spreadsheet", text="Open Spreadsheet", icon='SPREADSHEET')

        # Export section (always shown)
        export_box = layout.box()
        export_box.label(text="Export", icon='EXPORT')
        export_box.operator("mesh.export_with_metadata", text="Export OBJ + Metadata", icon='FILE_TICK')


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

class MESH_OT_open_spreadsheet(bpy.types.Operator):
    bl_idname = "mesh.open_face_attribute_spreadsheet"
    bl_label = "Open Spreadsheet"
    bl_description = "Open Spreadsheet in a separate popup window showing face engine_brightness"

    def execute(self, context):
        obj = context.active_object
        if not obj or obj.type != 'MESH':
            self.report({'ERROR'}, "Select a mesh object.")
            return {'CANCELLED'}

        wm = context.window_manager
        before = {w.as_pointer() for w in wm.windows}
        try:
            bpy.ops.wm.window_new()
        except Exception as e:
            self.report({'ERROR'}, f"Cannot create window: {e}")
            return {'CANCELLED'}

        new_window = None
        for w in wm.windows:
            if w.as_pointer() not in before:
                new_window = w
                break
        if not new_window:
            self.report({'ERROR'}, "Failed to detect new window.")
            return {'CANCELLED'}

        # Find first suitable area and turn it into a Spreadsheet
        target_area = None
        for a in new_window.screen.areas:
            if a.type in {'VIEW_3D', 'OUTLINER', 'PROPERTIES', 'IMAGE_EDITOR'}:
                target_area = a
                break
        if not target_area:
            # fallback: just take first
            target_area = new_window.screen.areas[0]
        target_area.type = 'SPREADSHEET'

        # Configure spreadsheet in new window
        try:
            with context.temp_override(window=new_window, screen=new_window.screen, area=target_area):
                space = target_area.spaces.active
                space.object = obj
                space.display_context = 'GEOMETRY'
                space.geometry_component_type = 'MESH'
                # Show face data
                try:
                    space.mesh_component_show_fdata = True
                except Exception:
                    pass
        except Exception:
            self.report({'WARNING'}, "Spreadsheet opened, configuration partial.")
            return {'FINISHED'}

        self.report({'INFO'}, "Spreadsheet opened in new window.")
        return {'FINISHED'}

class MESH_OT_export_with_metadata(bpy.types.Operator):
    bl_idname = "mesh.export_with_metadata"
    bl_label = "Export OBJ + Metadata"
    bl_description = "Export OBJ/MTL and JSON metadata (engine_scale + engine_brightness per face)"
    bl_options = {'REGISTER'}

    filepath: bpy.props.StringProperty(name="Filepath", subtype='FILE_PATH')
    use_selection: bpy.props.BoolProperty(default=True, name="Selection Only")

    def invoke(self, context, event):
        if not self.filepath:
            blend_name = bpy.path.display_name_from_filepath(bpy.data.filepath) or 'model'
            self.filepath = bpy.path.abspath(f"//{blend_name}.obj")
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}

    def execute(self, context):
        obj = context.active_object
        if not obj or obj.type != 'MESH':
            self.report({'ERROR'}, "Select a mesh object.")
            return {'CANCELLED'}

        base_path = bpy.path.abspath(self.filepath)
        if not base_path.lower().endswith('.obj'):
            base_path += '.obj'
        json_path = base_path[:-4] + '.json'

        # Preserve mode & selection state
        prev_mode = obj.mode if obj.mode != 'OBJECT' else 'OBJECT'
        prev_selection = {o: o.select_get() for o in context.view_layer.objects}
        # Ensure we are in OBJECT mode for export (some exporters ignore edit-mode data)
        try:
            if prev_mode != 'OBJECT':
                bpy.ops.object.mode_set(mode='OBJECT')
        except Exception:
            pass
        # Ensure object is selected (for selection-only export) and others deselected
        for o in context.view_layer.objects:
            o.select_set(False)
        obj.select_set(True)
        context.view_layer.objects.active = obj

        # Perform OBJ export (handle Blender API differences across versions)
        export_ok = False
        export_errors = []

        # Helper to call operator with flexible keyword mapping
        def _attempt_op(op_callable, base_kwargs, sel_flag):
            props = set()
            try:
                props = set(op_callable.get_rna_type().properties.keys())
            except Exception:
                pass
            kwargs = dict(base_kwargs)
            # Selection flag variants
            if sel_flag is not None:
                if 'use_selection' in props:
                    kwargs['use_selection'] = sel_flag
                elif 'export_selected_objects' in props:
                    kwargs['export_selected_objects'] = sel_flag
                elif 'export_selected' in props:
                    kwargs['export_selected'] = sel_flag
            # Normals flag variants
            if 'export_normals' in props:
                kwargs['export_normals'] = True
            elif 'use_normals' in props:
                kwargs['use_normals'] = True
            # UV flag variants
            if 'export_uv' in props:
                kwargs['export_uv'] = True
            elif 'use_uvs' in props:
                kwargs['use_uvs'] = True
            # Materials flag variants
            if 'export_materials' in props:
                kwargs['export_materials'] = True
            elif 'use_materials' in props:
                kwargs['use_materials'] = True
            return op_callable(**kwargs)

        base_kwargs = {'filepath': base_path}

        # 1. Try new unified exporter (wm.obj_export)
        if hasattr(bpy.ops.wm, 'obj_export'):
            try:
                result = _attempt_op(bpy.ops.wm.obj_export, base_kwargs, self.use_selection)
                if result == {'FINISHED'}:
                    export_ok = True
            except Exception as e:
                export_errors.append(f"wm.obj_export: {e}")

        # 2. Fallback to legacy addon exporter (export_scene.obj)
        if not export_ok and hasattr(bpy.ops, 'export_scene') and hasattr(bpy.ops.export_scene, 'obj'):
            try:
                result = _attempt_op(bpy.ops.export_scene.obj, base_kwargs, self.use_selection)
                if result == {'FINISHED'}:
                    export_ok = True
            except Exception as e:
                export_errors.append(f"export_scene.obj: {e}")

        # If export succeeded but produced an empty OBJ (no 'v ' lines), try one fallback without selection restriction
        if export_ok:
            try:
                with open(base_path, 'r', encoding='utf-8') as testf:
                    contents = testf.read()
                if '\nv ' not in contents and not contents.startswith('v '):
                    # Retry once with selection flag disabled if previously enabled
                    export_ok = False
                    if hasattr(bpy.ops.wm, 'obj_export'):
                        try:
                            _attempt_op(bpy.ops.wm.obj_export, base_kwargs, None)
                            export_ok = True
                        except Exception as e:
                            export_errors.append(f"retry wm.obj_export: {e}")
                    if not export_ok and hasattr(bpy.ops, 'export_scene') and hasattr(bpy.ops.export_scene, 'obj'):
                        try:
                            _attempt_op(bpy.ops.export_scene.obj, base_kwargs, None)
                            export_ok = True
                        except Exception as e:
                            export_errors.append(f"retry export_scene.obj: {e}")
            except Exception:
                pass

        if not export_ok:
            # Restore state before exit
            for o, sel in prev_selection.items():
                try:
                    o.select_set(sel)
                except Exception:
                    pass
            try:
                if prev_mode != 'OBJECT':
                    bpy.ops.object.mode_set(mode=prev_mode)
            except Exception:
                pass
            self.report({'ERROR'}, "OBJ export failed: " + "; ".join(export_errors) if export_errors else "Unknown exporter issue")
            return {'CANCELLED'}

        # Gather metadata
        metadata = {}
        metadata['engine_scale'] = int(obj.get('engine_scale', 10))
        brightness = []
        if obj.mode == 'EDIT':
            bm = bmesh.from_edit_mesh(obj.data)
            layer = bm.faces.layers.int.get('engine_brightness')
            if layer:
                for f in bm.faces:
                    brightness.append(int(f[layer]))
        else:
            bm = bmesh.new()
            bm.from_mesh(obj.data)
            layer = bm.faces.layers.int.get('engine_brightness')
            if layer:
                for f in bm.faces:
                    brightness.append(int(f[layer]))
            bm.free()
        metadata['engine_brightness'] = brightness

        import json
        try:
            with open(json_path, 'w', encoding='utf-8') as jf:
                json.dump(metadata, jf, indent=2)
        except Exception as e:
            self.report({'ERROR'}, f"JSON write failed: {e}")
            return {'CANCELLED'}

        # Restore original selection & mode
        for o, sel in prev_selection.items():
            try:
                o.select_set(sel)
            except Exception:
                pass
        try:
            if prev_mode != 'OBJECT':
                bpy.ops.object.mode_set(mode=prev_mode)
        except Exception:
            pass

        self.report({'INFO'}, f"Exported OBJ and metadata JSON: {json_path}")
        return {'FINISHED'}

def register():
    bpy.utils.register_class(MESH_PT_face_attribute_setter)
    bpy.utils.register_class(MESH_OT_initialize_file)
    bpy.utils.register_class(MESH_OT_open_spreadsheet)
    bpy.utils.register_class(MESH_OT_export_with_metadata)
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
    bpy.utils.unregister_class(MESH_OT_export_with_metadata)
    bpy.utils.unregister_class(MESH_OT_open_spreadsheet)
    bpy.utils.unregister_class(MESH_OT_initialize_file)
    del bpy.types.Scene.fa_attr_value
    del bpy.types.Scene.fa_attr_value_storage_internal
    del bpy.types.Scene.fa_attr_value_manual_override
    del bpy.types.Scene.fa_attr_last_face_index

if __name__ == "__main__":
    register()