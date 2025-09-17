
import generate_scene_header


def task_generate_scenes() -> int:
    return generate_scene_header.main([])


# Precompile steps: (name, function)
TASKS = [
    ("scene header generation", task_generate_scenes),
]


def main() -> int:
    for label, func in TASKS:
        print(f"[precompile] Running {label} ...")
        code = func()
        if code != 0:
            print(f"[precompile] {label} failed (exit {code})", file=sys.stderr)
            return code
    print("[precompile] All tasks complete.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
