#!/usr/bin/env python3
#
# Wavefront to Varooom Converter
# Copyright 2024 Nikku4211
#
# Copying and distribution of this file, with or without
# modification, are permitted in any medium without royalty
# provided the copyright notice and this notice are preserved.
# This file is offered as-is, without any warranty.
#
import os
import sys
import math
import json
from random import randrange
from typing import List, Optional

#thanks gvaliente
def sub(a, b):
    result = []

    for i in range(len(a)):
        result.append(a[i] - b[i])

    return result


def cross(a, b):
    return [a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]]


def norm(v):
    return math.sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]))


def normal(p0, p1, p2):
    # cross_product = np.cross(p1 - p0, p2 - p0)
    cross_product = cross(sub(p1, p0), sub(p2, p0))
    # normal = cross_product / np.linalg.norm(cross_product)
    magnitude = norm(cross_product)
    return [cross_product[0] / magnitude, cross_product[1] / magnitude, cross_product[2] / magnitude]


def convert_wavefront(obj_path: str, out_path: str, modelname: str, modelscale: float, 
                       recalcvertexnorms: bool = False, metadata_path: Optional[str] = None):
    """Convert a Wavefront OBJ to v3d header. Optionally use metadata JSON containing:
        {
          "engine_scale": int,
          "engine_brightness": [int, int, ...]
        }
    If engine_scale present, overrides modelscale.
    If engine_brightness list length matches face count, replaces random brightness.
    """
    with open(obj_path, 'rt') as waveobjfile:
        waveobjdata = waveobjfile.readlines()

    wavemtlfilename = []

    #get material file from obj file
    for x in range(len(waveobjdata)):
        if waveobjdata[x].startswith("mtllib "):
            # Derive material file path relative to the OBJ file directory
            obj_dir = os.path.dirname(os.path.abspath(obj_path))
            wavemtlfilename = os.path.join(obj_dir, waveobjdata[x][7:].strip())
            break

    if not wavemtlfilename:
        wavemtldata = []
    else:
        try:
            with open(wavemtlfilename, 'rt') as wavemtlfile:
                wavemtldata = wavemtlfile.readlines()
        except FileNotFoundError:
            print(f"Warning: MTL file '{wavemtlfilename}' not found; continuing without material colors.")
            wavemtldata = []

    #wavefront variables
    wavevertices = []
    wavevertexnorms = []
    wavefacels = []
    wavecolours = []
    wavematerialcurrent = -1
    wavematerialindices = []
    wavematerialnames = []

    # Load metadata if provided
    metadata = {}
    brightness_override: List[int] = []
    if metadata_path and os.path.isfile(metadata_path):
        try:
            with open(metadata_path, 'rt', encoding='utf-8') as mf:
                metadata = json.load(mf) or {}
            if 'engine_scale' in metadata:
                try:
                    modelscale = float(metadata['engine_scale'])
                except Exception:
                    pass
            if isinstance(metadata.get('engine_brightness'), list):
                brightness_override = [int(max(0, min(7, int(v)))) for v in metadata['engine_brightness']]
        except Exception as e:
            print(f"Warning: failed to load metadata '{metadata_path}': {e}")

    # First pass: gather vertices
    for line in waveobjdata:
        if line.startswith('v '):
            comps = line[2:].split()
            if len(comps) >= 3:
                wavevertices.append([float(comps[0])*modelscale, float(comps[1])*modelscale, float(comps[2])*modelscale])
        elif line.startswith('vn ') and not recalcvertexnorms:
            wavevertexnorms.append(line[3:].split())
        elif line.startswith('f '):
            parts = line[2:].split()
            face_idx = []
            for part in parts:
                if '/' in part:
                    face_idx.append(str(int(part.split('/')[0]) - 1))
                else:
                    face_idx.append(str(int(part) - 1))
            wavefacels.append(face_idx)
            wavematerialindices.append(max(0, wavematerialcurrent))
        elif line.startswith('usemtl '):
            if wavematerialnames and line[7:] in wavematerialnames and wavematerialnames.index(line[7:]) < len(wavematerialnames):
                pass
            else:
                wavematerialcurrent += 1

    # Materials / colors
    for x in range(len(wavemtldata)):
        if wavemtldata[x].startswith('Kd '):
            if wavemtldata[x][3::] not in wavecolours:
                wavecolours.append(wavemtldata[x][3::])
                rgb = wavemtldata[x][3::].split()
                # store as tuple for later writing
                wavematerialnames.append(wavemtldata[x-3][7::] if x >=3 and wavemtldata[x-3].startswith('newmtl ') else f'colour_{len(wavecolours)-1}')

    # Auto normals if requested or absent
    if recalcvertexnorms or (not wavevertexnorms and wavefacels):
        wavevertexnorms = []
        for fi in range(len(wavefacels)):
            p0 = wavevertices[int(wavefacels[fi][0])]
            p1 = wavevertices[int(wavefacels[fi][1])]
            p2 = wavevertices[int(wavefacels[fi][2])]
            wavevertexnorms.append(normal(p0, p1, p2))

    if wavefacels and len(wavevertexnorms) != len(wavefacels):
        # Recompute all normals to fix inconsistency
        wavevertexnorms = []
        for fi in range(len(wavefacels)):
            p0 = wavevertices[int(wavefacels[fi][0])]
            p1 = wavevertices[int(wavefacels[fi][1])]
            p2 = wavevertices[int(wavefacels[fi][2])]
            wavevertexnorms.append(normal(p0, p1, p2))

    # Abort if empty geometry
    if not wavevertices or not wavefacels:
        print(f"Skipping empty OBJ: {obj_path}")
        return

    # Provide at least one color
    if not wavecolours:
        wavecolours.append('0 0 0')

    # Write file now that data is validated
    with open(out_path, 'wt') as v3dfile:
        v3dfile.write(f"""\n    /*\n     * Based on Nikku4211's Wavefront to Varooom Converter (github.com/nikku4211/)\n     * Modified to support metadata (engine_scale / engine_brightness)\n     * Source OBJ: {os.path.basename(obj_path)}\n""")
        if metadata_path:
            v3dfile.write(f"     * Metadata: {os.path.basename(metadata_path)}\n")
        v3dfile.write("""     *\n     */\n\n    #ifndef FR_MODEL_3D_ITEMS_%(upper)s_H\n    #define FR_MODEL_3D_ITEMS_%(upper)s_H\n\n    #include \"fr_model_3d_item.h\"\n\n    namespace fr::model_3d_items\n    {\n        constexpr inline vertex_3d %(name)s_vertices[] = {\n""" % {"upper": modelname.upper(), "name": modelname})
        for v in wavevertices:
            v3dfile.write('             vertex_3d(' + ','.join(str(c) for c in v) + '),\n')
        v3dfile.write("       };\n")
        v3dfile.write(f"    constexpr inline bn::color {modelname}_colors[] = {{\n")
        for col in wavecolours:
            rgb = col.split()
            v3dfile.write('             bn::color(' + ','.join(str(math.floor(float(c)*31)) for c in rgb[:3]) + '),\n')
        v3dfile.write("    };\n")
        for idx in range(len(wavecolours)):
            v3dfile.write(f"    constexpr inline int {modelname}_color_{idx} = {idx};\n")
        v3dfile.write(f"\n    constexpr inline face_3d {modelname}_faces_full[] = {{\n")
        use_override = len(brightness_override) == len(wavefacels)
        for i, vnorm in enumerate(wavevertexnorms):
            brightness_val = brightness_override[i] if use_override else randrange(8)
            v3dfile.write('        face_3d(' + f'{modelname}_vertices, vertex_3d(' + f"{vnorm[0]},{vnorm[1]},{vnorm[2]})," + ','.join(wavefacels[i]) + ',' + f"{wavematerialindices[i] if i < len(wavematerialindices) else 0},{brightness_val}),\n")
        v3dfile.write("    };\n")
        v3dfile.write(f"    constexpr inline model_3d_item {modelname}_full({modelname}_vertices, {modelname}_faces_full, {modelname}_colors);\n")
        v3dfile.write("    };\n")
        v3dfile.write(f"#endif // FR_MODEL_3D_ITEMS_{modelname.upper()}_H")


def main(argv):
    # Simple argument parser (manual to avoid adding dependency)
    import shlex
    if len(argv) <= 1 or any(a in argv for a in ('-h','--help')):
        print('usage: wavefront2v3d.py input.obj output.hpp modelname modelscale [--recalcnorms] [--metadata meta.json]')
        return 0
    if len(argv) < 5:
        print('error: missing required arguments')
        return 1
    input_obj = argv[1]
    output_v3d = argv[2]
    modelname = argv[3]
    try:
        modelscale = float(argv[4])
    except Exception:
        print('error: modelscale must be numeric')
        return 1
    recalc = False
    metadata_path = None
    for arg in argv[5:]:
        if arg in ('--recalcnorms','-rn'):
            recalc = True
        elif arg.startswith('--metadata='):
            metadata_path = arg.split('=',1)[1]
        elif arg == '--metadata':
            # next token
            pass
    if '--metadata' in argv:
        idx = argv.index('--metadata')
        if idx+1 < len(argv):
            metadata_path = argv[idx+1]
    convert_wavefront(input_obj, output_v3d, modelname, modelscale, recalcvertexnorms=recalc, metadata_path=metadata_path)
    return 0

if __name__=='__main__':
    sys.exit(main(sys.argv))