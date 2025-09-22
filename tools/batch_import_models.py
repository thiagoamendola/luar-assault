#!/usr/bin/env python3
import os
import sys
import glob
import json
from pathlib import Path

# Local import of converter
import wavefront2v3d

"""
Batch converter:
 - Scans ./obj (relative to project root or current working directory) for *.obj
 - For each foo.obj looks for foo.json alongside (metadata) and foo.hpp output path in ./include/generated/ (created if missing)
 - Model name derived from filename stem (non-alnum replaced with underscore)
 - Scale argument: if metadata has engine_scale it overrides; else default 10.0 (same semantics as earlier)
Usage:
    python tools/batch_import_models.py [obj_folder] [out_folder]
Defaults:
    obj_folder = ./obj
    out_folder = ./include/generated
"""

def sanitize_name(name: str) -> str:
    out = []
    for ch in name:
        if ch.isalnum() or ch == '_':
            out.append(ch)
        else:
            out.append('_')
    return ''.join(out)

def main(argv):
    # Anchor defaults to repository root (parent of this script's directory)
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    obj_dir = Path(argv[1]) if len(argv) > 1 else repo_root / 'obj'
    out_dir = Path(argv[2]) if len(argv) > 2 else repo_root / 'include' / 'models'
    obj_dir = obj_dir.resolve()
    out_dir = out_dir.resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    objs = sorted(obj_dir.glob('*.obj'))
    if not objs:
        print(f'No OBJ files found in {obj_dir}')
        return 0

    converted = 0
    for obj_path in objs:
        stem = obj_path.stem
        modelname = sanitize_name(stem)
        metadata_path = obj_path.with_suffix('.json')
        # Decide model scale: will be overwritten in converter if metadata has engine_scale
        modelscale = 10.0
        out_path = out_dir / f'{modelname}.h'
        overwrite = out_path.exists()
        print(f'Converting {obj_path.name} -> {out_path.name}' + (' (overwrite)' if overwrite else ''))
        try:
            wavefront2v3d.convert_wavefront(
                str(obj_path),
                str(out_path),
                modelname,
                modelscale,
                recalcvertexnorms=False,
                metadata_path=str(metadata_path) if metadata_path.exists() else None
            )
            converted += 1
        except Exception as e:
            print(f'ERROR converting {obj_path}: {e}')
    print(f'Done. Converted {converted}/{len(objs)} OBJ files.')
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
