import os
import sys
from pathlib import Path
from termcolor import colored


# File patterns to delete
UNWANTED_PATTERNS = [
    "*.bmp~",
    "*.blend1",
    "*-autosave.kra",
]


def delete_files_by_pattern(root_dir: Path, pattern: str) -> int:
    """Delete all files matching the given pattern recursively."""
    deleted_count = 0
    for file_path in root_dir.rglob(pattern):
        if file_path.is_file():
            try:
                file_path.unlink()
                print(f"{colored('[cleanup]', 'cyan')} Deleted: {file_path.relative_to(root_dir)}")
                deleted_count += 1
            except Exception as e:
                print(f"{colored('[cleanup]', 'cyan')} {colored('Error', 'red')} deleting {file_path}: {e}", file=sys.stderr)
    return deleted_count


def main(args=None) -> int:
    """Main cleanup function."""
    # Get the project root (parent of tools directory)
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    
    total_deleted = 0
    for pattern in UNWANTED_PATTERNS:
        print(f"{colored('[cleanup]', 'cyan')} Searching for {colored(pattern, 'yellow')} files...")
        deleted = delete_files_by_pattern(project_root, pattern)
        total_deleted += deleted
    
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
