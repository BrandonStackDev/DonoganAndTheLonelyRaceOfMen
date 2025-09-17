# clean_export_raylib.py
# 1) File > Import > glTF 2.0 (.glb) — import your original animated file
# 2) Scripting tab > New/Text Editor > paste this > Run Script
# 3) Exports next to your .blend: //darrel_clean.glb (and //darrel_clean.iqm if add-on)
# this script is for taking meshy glb's and cleaning them up to target raylib, designed for simple NPC's that dont do much, but I wanted animation...

import bpy

# --------------------------
# CONFIG
# --------------------------
ARMATURE_NAME = None      # e.g. "Armature"; None = auto-pick selected or first
EXPORT_BASENAME = "darrel_clean"
DO_TRIANGULATE  = False   # set True if you want forced triangulation
MAX_WEIGHTS     = 4       # raylib expects <= 4 weights per vertex
CLEAN_EPS       = 1e-4
DO_GLB          = True
DO_IQM          = False    # requires IQM exporter add-on (export_scene.iqm)

# --------------------------
# HELPERS
# --------------------------
def _ensure_object_mode():
    try:
        if bpy.ops.object.mode_set.poll():
            bpy.ops.object.mode_set(mode='OBJECT')
    except Exception:
        pass

def _ops_try(op, **kwargs):
    try:
        return op(**kwargs)
    except Exception as e:
        # Blender operator wrappers don't have __name__
        try:
            name = op.idname() if hasattr(op, "idname") else str(op)
        except Exception:
            name = str(op)
        print(f"[WARN] {name} failed: {e}")
        return None


def find_primary_armature():
    if ARMATURE_NAME:
        arm = bpy.data.objects.get(ARMATURE_NAME)
        if arm and arm.type == 'ARMATURE':
            return arm
    arms = [o for o in bpy.data.objects if o.type == 'ARMATURE']
    if not arms:
        return None
    sel = [a for a in arms if a.select_get()]
    return sel[0] if sel else arms[0]

def find_meshes_for_armature(arm):
    res = []
    for o in bpy.data.objects:
        if o.type != 'MESH':
            continue
        for m in o.modifiers:
            if m.type == 'ARMATURE' and m.object == arm:
                res.append(o); break
    return res

def select_only(objs):
    _ensure_object_mode()
    bpy.ops.object.select_all(action='DESELECT')
    for o in objs:
        o.select_set(True)
    if objs:
        bpy.context.view_layer.objects.active = objs[0]

def apply_rot_scale(objs):
    _ensure_object_mode()
    for o in objs:
        bpy.context.view_layer.objects.active = o
        _ops_try(bpy.ops.object.transform_apply, location=False, rotation=True, scale=True)

def normalize_and_limit_weights(obj, max_weights=4, clean_eps=1e-4):
    if obj.type != 'MESH':
        return
    if not obj.vertex_groups:
        print(f"[weights] '{obj.name}' has no vertex groups; skip")
        return
    _ensure_object_mode()
    bpy.context.view_layer.objects.active = obj
    _ops_try(bpy.ops.object.vertex_group_limit_total, group_select_mode='ALL', limit=max_weights)
    _ops_try(bpy.ops.object.vertex_group_clean,        group_select_mode='ALL', limit=clean_eps)
    _ops_try(bpy.ops.object.vertex_group_normalize_all, lock_active=False)

def triangulate_apply(obj):
    if obj.type != 'MESH':
        return
    _ensure_object_mode()
    bpy.context.view_layer.objects.active = obj
    mod = obj.modifiers.new("TriangulateForRaylib", 'TRIANGULATE')
    _ops_try(bpy.ops.object.modifier_apply, modifier=mod.name)

def export_glb_selected(filepath_glb):
    print("[export] GLB ->", filepath_glb)
    # Try with explicit flags first
    if not _ops_try(
        bpy.ops.export_scene.gltf,
        filepath=filepath_glb,
        export_format='GLB',
        use_selection=True,
        export_apply=False,   # don't bake modifiers/armature
        export_animations=True
    ):
        # Fallback minimal call (different Blender versions)
        _ops_try(
            bpy.ops.export_scene.gltf,
            filepath=filepath_glb,
            export_format='GLB',
            use_selection=True
        )

def export_iqm_selected(filepath_iqm):
    if not hasattr(bpy.ops.export_scene, "iqm"):
        print("[export] IQM exporter not installed; skipping")
        return
    print("[export] IQM ->", filepath_iqm)
    _ops_try(
        bpy.ops.export_scene.iqm,
        filepath=filepath_iqm,
        export_selected=True,
        export_animations=True
    )

def summarize(arm, meshes, prefix="[summary]"):
    print(f"\n{prefix} Armature:", arm.name if arm else "(none)")
    if arm and arm.data:
        print(f"{prefix} Bones:", len(arm.data.bones))
    for m in meshes:
        print(f"{prefix} Mesh: {m.name}  verts={len(m.data.vertices)}  vgroups={len(m.vertex_groups)}")
    print()


def remove_root_motion(arm):
    """Bake out translation from the root bone so the model stays in place."""
    if not arm or arm.type != 'ARMATURE':
        return
    root_name = "Hips"  # adjust if your root bone has a different name
    if root_name not in arm.data.bones:
        print(f"[root motion] Bone '{root_name}' not found; skipping")
        return

    for action in bpy.data.actions:
        for fcurve in action.fcurves:
            data_path = fcurve.data_path
            if root_name in data_path and "location" in data_path:
                # Zero out location keyframes
                for kp in fcurve.keyframe_points:
                    kp.co[1] = 0.0
                    kp.handle_left[1] = 0.0
                    kp.handle_right[1] = 0.0
                fcurve.update()
    print(f"[root motion] Stripped translation from '{root_name}' in all actions")

# --------------------------
# MAIN
# --------------------------
def clean_for_raylib():
    arm = find_primary_armature()
    if not arm:
        print("ERROR: No armature found. Import your GLB first, or set ARMATURE_NAME.")
        return
    meshes = find_meshes_for_armature(arm)
    if not meshes:
        print(f"ERROR: No meshes bound to armature '{arm.name}'. Check Armature modifiers.")
        return

    summarize(arm, meshes, "[before]")

    # Select export set
    select_only([arm] + meshes)

    # Apply rotation & scale (keeps location)
    print("[clean] Apply rotation & scale (armature + meshes)")
    apply_rot_scale([arm] + meshes)

    # Weight cleanup
    print(f"[clean] Limit to {MAX_WEIGHTS} weights/vertex, clean {CLEAN_EPS}, normalize")
    for m in meshes:
        normalize_and_limit_weights(m, MAX_WEIGHTS, CLEAN_EPS)

    # Optional triangulation
    if DO_TRIANGULATE:
        print("[clean] Triangulate meshes")
        for m in meshes:
            triangulate_apply(m)

    # Reselect before export
    select_only([arm] + meshes)
    remove_root_motion(arm)
    
    glb_path = bpy.path.abspath(f"//{EXPORT_BASENAME}.glb")
    iqm_path = bpy.path.abspath(f"//{EXPORT_BASENAME}.iqm")
    
    # Export
    if DO_GLB:
        export_glb_selected(glb_path)
    if DO_IQM:
        export_iqm_selected(iqm_path)

    summarize(arm, meshes, "[after]")
    print("[done] Exports written:",
          ("GLB " + glb_path) if DO_GLB else "",
          ("IQM " + iqm_path) if (DO_IQM and hasattr(bpy.ops.export_scene, "iqm")) else "")

clean_for_raylib()
