
import generate_scene_header
import batch_import_models
import cleanup_files

from termcolor import colored


def task_generate_scenes() -> int:
    return generate_scene_header.main([])


def task_import_models() -> int:
    return batch_import_models.main([])


def task_cleanup_files() -> int:
    return cleanup_files.main()


# Precompile steps: (name, function)
TASKS = [
    ("model importer", task_import_models),
    ("scene header generation", task_generate_scenes),
    ("cleanup unwanted files", task_cleanup_files),
]


def main() -> int:
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
