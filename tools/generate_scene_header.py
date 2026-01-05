#!/usr/bin/env python3
"""
Reads a scene JSON description and generates a corresponding C++ header for that scene.

Usage (PowerShell):
  python tools/generate_scene_header.py
  python tools/generate_scene_header.py stages/test_automated_scene.json
  python tools/generate_scene_header.py stages/test_automated_scene.json include/test_automated_scene_defs.h

If output path is omitted it writes the header to:
  include/game_scene_defs/<scene_name>_defs.h
"""

import json
import sys
from pathlib import Path
from typing import Any, Dict, List, Set
from termcolor import colored

# Core always-needed structural includes (edit here if structural dependencies change)
STRUCTURAL_INCLUDES = [
    'scene_colors_generator.h',
    'stage_section.h',
    'static_model_3d_item.h',
    'bn_color.h',
    'bn_span.h',
]

# Palette-driven headers (map palette name -> header providing fr::model_3d_items::<name>_colors symbol)
# Add entries here when introducing a new non-model palette color array.
PALETTE_HEADER_MAP = {
    'debug_collider': 'colliders.h',
    'laser': 'player_laser.h',
    'hurt': 'player_ship.h',
    # model palettes like 'bush', 'asteroid1', 'player_ship_02', 'shot' are provided by their model headers
}

# Optional explicit overrides: model JSON name -> header file (under models/)
MODEL_HEADER_OVERRIDES: Dict[str, str] = {}

# Optional explicit symbol overrides (if fr::model_3d_items symbol differs)
MODEL_SYMBOL_OVERRIDES: Dict[str, str] = {}


def _macro_guard(name: str) -> str:
    return ''.join(c.upper() if c.isalnum() else '_' for c in f"{name}_defs_h")


def _model_root(model_name: str) -> str:
    # Heuristic: keep entire name unless it ends with a specific known suffix we want to drop.
    # For bush_full we want bush (drop _full). For others keep.
    if model_name.endswith('_full'):
        return model_name.rsplit('_', 1)[0]
    # Keep player_ship_02 etc intact.
    return model_name


def _header_from_model(model_name: str) -> str:
    if model_name in MODEL_HEADER_OVERRIDES:
        return MODEL_HEADER_OVERRIDES[model_name]
    root = _model_root(model_name)
    return f"models/{root}.h"


def _symbol_from_model(model_name: str) -> str:
    return MODEL_SYMBOL_OVERRIDES.get(model_name, model_name)


def generate_header(scene: Dict[str, Any]) -> str:
    name = scene['name']
    palette: List[str] = scene.get('palette', [])
    sections: List[Dict[str, Any]] = scene.get('sections', [])

    guard = _macro_guard(name)

    # Collect model names actually used (enabled only) for includes.
    model_names: Set[str] = set()
    for s in sections:
        for idx, m in enumerate(s.get('staticModels', [])):
            if not m.get('enabled', True):
                continue
            model_names.add(m['model'])

    # Derive headers from static models
    model_headers = {_header_from_model(m) for m in model_names}

    # Also attempt to include headers for palette entries that match existing model asset names
    for p in palette:
        # if a header with this palette name exists (or its root) add it
        candidate = _header_from_model(p)
        if Path('include')/candidate in (Path('include')/candidate).parent.glob(Path(candidate).name):
            model_headers.add(candidate)
    model_headers = sorted(model_headers)

    # Build includes using top-level configuration constants
    palette_includes = []
    missing_palette_symbols = []
    for p in palette:
        hdr = PALETTE_HEADER_MAP.get(p)
        if hdr:
            palette_includes.append(hdr)
        else:
            # If it's also a model-based palette (model header exists) we already added via model_headers.
            candidate_model_header = _header_from_model(p)
            if candidate_model_header not in model_headers:
                # Not mapped and no model header discovered: warn later.
                missing_palette_symbols.append(p)

    # 3. Model headers already resolved (model_headers)

    # Combine and deduplicate preserving order (structural -> palette -> models)
    ordered_all = []
    seen = set()
    for group in (STRUCTURAL_INCLUDES, palette_includes, model_headers):
        for inc in group:
            if inc not in seen:
                ordered_all.append(inc)
                seen.add(inc)

    include_lines = [f'#include "{inc}"' for inc in ordered_all]

    # Emit warnings for missing palette symbols (stderr) so user can fix JSON or mapping
    for mp in missing_palette_symbols:
        print(
            f"[generate_scene_header] WARNING: Palette '{mp}' has no associated header (add mapping in PALETTE_HEADER_MAP or provide model header)",
            file=sys.stderr,
        )

    # Emit static models constants and section arrays
    section_blocks: List[str] = []

    for s in sections:
        sid = s['id']
        start = s['range']['start']
        end = s['range']['end']
        static_models = s.get('staticModels', [])
        enemies = s.get('enemies', [])

        model_const_lines: List[str] = []
        model_items_lines: List[str] = []
        model_index = 1
        for m in static_models:
            if not m.get('enabled', True):
                continue
            model_name = m['model']
            symbol = _symbol_from_model(model_name)
            pos = m['position']
            param_value = m.get('rotation', 0)  # overloaded numeric param
            palette_override = m.get('paletteOverride')
            const_id = f"_s{sid}_model_{model_index}"
            model_index += 1
            use_palette = False
            if palette_override:
                # Validate palette symbol existence by heuristics: we only know built-in ones if present in palette list
                if palette_override in palette:
                    use_palette = True
            # Interpret input JSON static model positions as LOCAL section space.
            # Section start Y is the world-space anchor; convert local y -> world y.
            section_start_y = start  # world-space Y where this section begins rendering
            local_y = pos['y']
            world_y = section_start_y + local_y
            if use_palette:
                palette_symbol = f"fr::model_3d_items::{palette_override}_colors"
                ctor = (f"static_model_3d_item<fr::model_3d_items::{symbol}>(\n"
                        f"        fr::point_3d({pos['x']}, {world_y}, {pos['z']}), {param_value}, {palette_symbol})")
            else:
                ctor = (f"static_model_3d_item<fr::model_3d_items::{symbol}>(\n"
                        f"        fr::point_3d({pos['x']}, {world_y}, {pos['z']}), {param_value})")
            model_const_lines.append(f"constexpr auto {const_id} =\n    {ctor};")
            model_items_lines.append(f"    {const_id}.item(),")

        if model_items_lines:
            # remove last comma for neatness (optional)
            model_items_lines[-1] = model_items_lines[-1].rstrip(',')

        enemy_property_const_lines: List[str] = []
        enemy_lines: List[str] = []
        enemy_index = 1
        for e in enemies:
            if not e.get('enabled', True):
                continue
            pos = e['position']
            spawn_offset = e.get('rotation', 0)  # reused field
            etype = e['type']
            # Enemies now also interpret JSON y as LOCAL section space.
            world_enemy_y = start + pos['y']
            
            # Check for enemy-specific optional properties
            props_ptr = "nullptr"
            if etype == 'OYSTER':
                player_distance = e.get('playerDistance')
                if player_distance is not None:
                    props_const_name = f"_s{sid}_enemy_{enemy_index}_props"
                    enemy_property_const_lines.append(
                        f"constexpr oyster_properties {props_const_name} = {{{player_distance}}};")
                    props_ptr = f"&{props_const_name}"
            elif etype == 'NAIAH':
                # Add naiah property parsing here when needed
                pass
            # For any other enemy type or if no properties are set, props_ptr remains nullptr
            
            enemy_lines.append(
                f"    enemy_def{{fr::point_3d({pos['x']}, {world_enemy_y}, {pos['z']}), {spawn_offset}, enemy_type::{etype}, {props_ptr}}},")
            enemy_index += 1
            
        if enemy_lines:
            enemy_lines[-1] = enemy_lines[-1].rstrip(',')

        section_src = []
        section_src.extend(model_const_lines)
        section_src.append("")
        section_src.extend(enemy_property_const_lines)
        if enemy_property_const_lines:
            section_src.append("")
        # Explicitly type initializer_lists so empty lists compile (no deduction failure)
        if model_items_lines:
            section_src.append(f"constexpr std::initializer_list<fr::model_3d_item> _section_{sid}_static_model_items = {{")
            section_src.extend(model_items_lines)
            section_src.append("};")
        else:
            section_src.append(f"constexpr std::initializer_list<fr::model_3d_item> _section_{sid}_static_model_items = {{}};")
        section_src.append("")
        if enemy_lines:
            section_src.append(f"constexpr std::initializer_list<enemy_def> _section_{sid}_enemies = {{")
            section_src.extend(enemy_lines)
            section_src.append("};")
        else:
            section_src.append(f"constexpr std::initializer_list<enemy_def> _section_{sid}_enemies = {{}};")
        section_src.append("")
        section_src.append(f"constexpr int _section_{sid}_start = {start};")
        section_src.append(f"constexpr int _section_{sid}_end = {end};")
        end_section = s.get('end_section', False)
        end_section_str = 'true' if end_section else 'false'
        section_src.append(f"constexpr bool _section_{sid}_end_section = {end_section_str};")
        section_src.append("")
        section_src.append(
            f"constexpr stage_section section_{sid}(_section_{sid}_start, _section_{sid}_end,\n"
            f"                                  _section_{sid}_static_model_items, _section_{sid}_enemies,\n"
            f"                                  _section_{sid}_end_section);")
        section_blocks.append('\n'.join(section_src))

    # Sections list
    section_ptrs = ',\n    '.join(f"&section_{s['id']}" for s in sections)
    sections_full_block = (
        "constexpr const auto sections_full = {\n"
        f"    {section_ptrs},\n" \
        "};\n\nconstexpr stage_section_list_ptr sections = sections_full.begin();\nconstexpr size_t sections_count = sections_full.size();"
    )

    # Palette / colors block
    palette_lines = []
    palette_lines.append("constexpr const auto raw_scene_colors = {")
    for p in palette:
        # Only emit palettes that likely exist; skip if header not available and symbol doesn't look like a model.
        palette_lines.append(f"    bn::span<const bn::color>(fr::model_3d_items::{p}_colors),")
    if palette:
        palette_lines[-1] = palette_lines[-1].rstrip(',')  # last line no trailing comma
    palette_lines.append("};")
    palette_lines.append("")
    palette_lines.append("constexpr size_t model_palette_count = raw_scene_colors.size();")
    palette_lines.append("constexpr size_t scene_palette_size = calculate_total_size(raw_scene_colors);")
    palette_lines.append("constexpr const bn::span<const bn::color> *raw_scene_color_ptr = raw_scene_colors.begin();")
    palette_lines.append("")
    palette_lines.append("constexpr bn::array<bn::color, scene_palette_size> scene_colors = generate_scene_colors<scene_palette_size>(raw_scene_colors);")
    palette_lines.append("")
    palette_lines.append("inline color_mapping_handler *get_scene_color_mapping()\n{\n    return new color_mapping_handler(model_palette_count, scene_palette_size,\n                                     raw_scene_color_ptr, scene_colors.data());\n};")

    header_lines: List[str] = []
    header_lines.append("// THIS FILE IS AUTOGENERATED. EDIT THE .json INSTEAD\n")
    header_lines.append(f"#ifndef {guard}")
    header_lines.append(f"#define {guard}")
    header_lines.append("")
    header_lines.extend(include_lines)
    header_lines.append("")
    header_lines.append("using namespace scene_colors_generator;")
    header_lines.append("")
    header_lines.append("// --- Sections\n")
    header_lines.append('\n\n'.join(section_blocks))
    header_lines.append("\n// # Sections List\n")
    header_lines.append(sections_full_block)
    header_lines.append("\n// --- Colors\n")
    header_lines.extend(palette_lines)
    header_lines.append("\n#endif")

    return '\n'.join(header_lines) + '\n'


def _process_one(in_path: Path, explicit_out: Path | None = None) -> Path:
    with in_path.open('r', encoding='utf-8') as f:
        scene = json.load(f)
    if explicit_out is not None:
        out_path = explicit_out
        out_path.parent.mkdir(parents=True, exist_ok=True)
    else:
        out_dir = Path('include') / 'game_scene_defs'
        out_dir.mkdir(parents=True, exist_ok=True)
        out_path = out_dir / f"{scene['name']}_defs.h"
    header_text = generate_header(scene)
    # Only rewrite if changed to avoid unnecessary rebuilds
    if out_path.exists():
        old = out_path.read_text(encoding='utf-8')
        if old == header_text:
            print(f"Unchanged: {out_path}")
            return out_path
    out_path.write_text(header_text, encoding='utf-8')
    print(f"Generated: {out_path}")
    return out_path


def main(argv: List[str]) -> int:
    # Batch mode: no args or only program name -> process all stages/*.json
    if len(argv) <= 1:
        stages_dir = Path('stages')
        if not stages_dir.is_dir():
            print("No stages directory found.", file=sys.stderr)
            return 1
        json_files = sorted(stages_dir.glob('*.json'))
        if not json_files:
            print("No JSON stage files found in stages/", file=sys.stderr)
            return 2
        for jf in json_files:
            try:
                _process_one(jf)
            except Exception as exc:  # pragma: no cover
                print(f"{colored("Failed", 'red', attrs=["reverse", "blink", "bold"])}: {jf}: {exc}", file=sys.stderr)
        return 0

    if len(argv) > 3:
        print("Usage: generate_scene_header.py [<input.json> [output_header.h]]", file=sys.stderr)
        return 1

    in_path = Path(argv[1])
    if not in_path.is_file():
        print(f"Input JSON not found: {in_path}", file=sys.stderr)
        return 2

    explicit_out = Path(argv[2]) if len(argv) == 3 else None
    _process_one(in_path, explicit_out)
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
