
import argparse
import sys
import os

# Add the gba-free-fonts tools directory to sys.path
script_dir = os.path.dirname(os.path.abspath(__file__))
fonts_tools_path = os.path.join(script_dir, '..', '..', 'gba-free-fonts', 'tools', 'butano')
sys.path.insert(0, os.path.abspath(fonts_tools_path))

import generate_scene_header
import batch_import_models
import cleanup_files
import butano_fonts_tool
import generate_audio_viewer_defs

from termcolor import colored

# Global variables for command-line arguments
build_dir = 'build'
fonts_path = ''
texts_path = ''


def task_generate_scenes() -> int:
    return generate_scene_header.main([])

def task_import_models() -> int:
    return batch_import_models.main([])

def _invalidate_font_cache_if_png_changed() -> None:
    """
    butano_fonts_tool only tracks .fnt files for change detection — PNG atlas
    changes are invisible to it and silently skipped.  We fix that here by
    checking every PNG inside the font folders: if any PNG is newer than the
    cached font-info file we delete that cache file so the tool is forced to
    do a full regeneration on the next run.
    """
    font_info_path = os.path.join(build_dir, '_bn_fonts_files_info.txt')
    if not os.path.exists(font_info_path):
        return  # no cache yet – nothing to invalidate

    cache_mtime = os.path.getmtime(font_info_path)

    for folder in fonts_path.split():
        if not os.path.isdir(folder):
            continue
        for fname in os.listdir(folder):
            if fname.lower().endswith('.png') or fname.lower().endswith('.bmp'):
                fpath = os.path.join(folder, fname)
                if os.path.getmtime(fpath) > cache_mtime:
                    print(f"{colored('[precompile]', 'light_blue')} Font PNG changed ({fname}), invalidating font cache.")
                    os.remove(font_info_path)
                    return  # one hit is enough


def task_generate_fonts() -> int:
    try:
        _invalidate_font_cache_if_png_changed()
        butano_fonts_tool.process_fonts(fonts_path, build_dir, texts_path)
        return 0
    except Exception as ex:
        print(f"Error in font generation: {ex}")
        return -1

def task_generate_audio_defs() -> int:
    return generate_audio_viewer_defs.main([])

def task_cleanup_files() -> int:
    return cleanup_files.main()


# Precompile steps: (name, function)
TASKS = [
    ("model importer", task_import_models),
    ("scene header generation", task_generate_scenes),
    ("audio viewer generation", task_generate_audio_defs),
    ("font importing", task_generate_fonts),
    ("cleanup unwanted files", task_cleanup_files),
]


def main(args=None) -> int:
    global build_dir, fonts_path, texts_path
    
    parser = argparse.ArgumentParser(description='Precompile build tasks')
    parser.add_argument('--build', type=str, default='build', help='Build directory path')
    parser.add_argument('--fonts', type=str, default='', help='Fonts directories or files')
    parser.add_argument('--texts', type=str, default='', help='Texts directories or files')
    
    parsed_args = parser.parse_args(args)
    
    # Store arguments in global variables
    build_dir = parsed_args.build
    fonts_path = parsed_args.fonts
    texts_path = parsed_args.texts
    
    for label, func in TASKS:
        print(f"{colored("[precompile]", 'light_blue')} Running {colored(label, 'cyan')} ...")
        code = func()
        if code != 0:
            print(f"{colored("[precompile]", 'light_blue')} {label} failed (exit {code})", file=sys.stderr)
            return code
    print(f"{colored("[precompile]", 'light_blue')} {colored("All tasks complete. Starting compilation.", 'green')}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
