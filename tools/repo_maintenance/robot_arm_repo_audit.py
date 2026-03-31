from pathlib import Path
from collections import Counter, defaultdict
import os
import hashlib

ROOT = Path(r"I:\Projects\Robot_Arm_")
MAX_TREE_DEPTH = 3
TOP_N_LARGEST = 25
TOP_N_DUPLICATES = 20

IGNORE_DIRS = {
    ".git", "__pycache__", ".vscode", ".idea", "node_modules",
    "dist", "build", ".vs", "Debug", "Release"
}

MESSY_KEYWORDS = [
    "copy", "backup", "old", "temp", "test", "finalfinal",
    "new folder", "untitled", "draft", "ver", "version"
]

LIKELY_PROJECT_FILES = {
    ".ino", ".py", ".cpp", ".c", ".h", ".hpp", ".md",
    ".stl", ".step", ".f3d", ".csv", ".json", ".kicad_pcb",
    ".kicad_sch", ".pcbdoc", ".schdoc"
}


def human_size(num):
    for unit in ["B", "KB", "MB", "GB", "TB"]:
        if num < 1024:
            return f"{num:.1f} {unit}"
        num /= 1024
    return f"{num:.1f} PB"


def safe_stat_size(path):
    try:
        return path.stat().st_size
    except Exception:
        return 0


def file_hash(path, chunk_size=65536):
    h = hashlib.sha256()
    try:
        with open(path, "rb") as f:
            while True:
                chunk = f.read(chunk_size)
                if not chunk:
                    break
                h.update(chunk)
        return h.hexdigest()
    except Exception:
        return None


def print_tree(path, prefix="", depth=0, max_depth=3):
    if depth > max_depth:
        return
    try:
        items = sorted(path.iterdir(), key=lambda p: (p.is_file(), p.name.lower()))
    except Exception:
        print(prefix + "[unreadable]")
        return

    for i, item in enumerate(items):
        connector = "└── " if i == len(items) - 1 else "├── "
        print(prefix + connector + item.name)

        if item.is_dir() and item.name not in IGNORE_DIRS:
            extension = "    " if i == len(items) - 1 else "│   "
            print_tree(item, prefix + extension, depth + 1, max_depth)


if not ROOT.exists():
    print(f"Path does not exist: {ROOT}")
    raise SystemExit(1)

file_count = 0
dir_count = 0
ext_counter = Counter()
size_by_ext = defaultdict(int)
largest_files = []
messy_files = []
candidate_project_dirs = Counter()
readme_dirs = set()
all_files = []
hash_groups = defaultdict(list)

for current_root, dirs, files in os.walk(ROOT):
    dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
    dir_count += len(dirs)

    current_path = Path(current_root)

    # detect README presence
    for f in files:
        if f.lower() == "readme.md":
            readme_dirs.add(current_path)

    for f in files:
        file_count += 1
        full_path = current_path / f
        all_files.append(full_path)

        ext = full_path.suffix.lower() if full_path.suffix else "[no extension]"
        ext_counter[ext] += 1

        size = safe_stat_size(full_path)
        size_by_ext[ext] += size
        largest_files.append((size, full_path))

        lower_name = f.lower()
        if any(keyword in lower_name for keyword in MESSY_KEYWORDS):
            messy_files.append(full_path)

        # Count likely project-relevant files by top-level folder
        try:
            relative_parts = full_path.relative_to(ROOT).parts
            if len(relative_parts) > 0:
                top_folder = relative_parts[0]
                if ext in LIKELY_PROJECT_FILES:
                    candidate_project_dirs[top_folder] += 1
        except Exception:
            pass

        # Hash only smaller files to avoid huge scan time
        if size <= 50 * 1024 * 1024:  # 50 MB
            h = file_hash(full_path)
            if h:
                hash_groups[h].append(full_path)

largest_files.sort(reverse=True, key=lambda x: x[0])

duplicate_groups = [paths for paths in hash_groups.values() if len(paths) > 1]
duplicate_groups.sort(key=lambda g: (-len(g), -sum(safe_stat_size(p) for p in g)))

print("=" * 80)
print("ROBOT ARM REPO AUDIT")
print("=" * 80)
print(f"Root: {ROOT}")

print("\nFOLDER TREE (depth 3)")
print("-" * 80)
print(ROOT.name)
print_tree(ROOT, max_depth=MAX_TREE_DEPTH)

print("\nSUMMARY")
print("-" * 80)
print(f"Folders: {dir_count}")
print(f"Files:   {file_count}")

print("\nFILE TYPES")
print("-" * 80)
for ext, count in ext_counter.most_common():
    print(f"{ext:15} {count:6} files   {human_size(size_by_ext[ext])}")

print("\nLARGEST FILES")
print("-" * 80)
for size, path in largest_files[:TOP_N_LARGEST]:
    print(f"{human_size(size):>10}   {path}")

print("\nMESSY / REVIEW THESE")
print("-" * 80)
if messy_files:
    for path in messy_files[:100]:
        print(path)
else:
    print("No obviously messy file names found.")

print("\nPOSSIBLE DUPLICATES (same file content)")
print("-" * 80)
if duplicate_groups:
    shown = 0
    for group in duplicate_groups:
        print(f"\nDuplicate group ({len(group)} files):")
        for path in group[:10]:
            print(f"  {path}")
        if len(group) > 10:
            print("  ...")
        shown += 1
        if shown >= TOP_N_DUPLICATES:
            break
else:
    print("No duplicates found.")

print("\nTOP-LEVEL FOLDERS MOST LIKELY TO BE REAL PROJECT AREAS")
print("-" * 80)
if candidate_project_dirs:
    for folder, score in candidate_project_dirs.most_common():
        full = ROOT / folder
        has_readme = full in readme_dirs
        print(f"{folder:35} likely project files: {score:4}   README: {'yes' if has_readme else 'no'}")
else:
    print("No project-like folders detected.")

print("\nTOP-LEVEL CONTENT")
print("-" * 80)
try:
    for item in sorted(ROOT.iterdir(), key=lambda p: (p.is_file(), p.name.lower())):
        if item.is_dir():
            file_total = sum(1 for x in item.rglob("*") if x.is_file())
            print(f"[DIR ] {item.name:35} {file_total:6} files")
        else:
            print(f"[FILE] {item.name:35} {human_size(safe_stat_size(item))}")
except Exception as e:
    print(f"Could not list top-level content: {e}")

print("\nSUGGESTED CLEANUP CHECKLIST")
print("-" * 80)
print("1. Keep only final, meaningful project folders at the repo root.")
print("2. Move extra experiments into an /archive folder or separate repo.")
print("3. Add README.md to the main folders that matter.")
print("4. Remove duplicate files and weirdly named copies.")
print("5. Move giant media/output files out of the repo if not needed.")
print("6. Add a .gitignore for build/cache/generated files.")
print("7. Keep CAD, code, electronics, docs, and media organized by folder.")
print("8. Make the repo root easy to understand in under 30 seconds.")

report_path = ROOT / "repo_audit_report.txt"
try:
    with open(report_path, "w", encoding="utf-8") as f:
        f.write("Robot Arm Repo Audit completed successfully.\n")
        f.write(f"Root: {ROOT}\n")
        f.write(f"Folders: {dir_count}\n")
        f.write(f"Files: {file_count}\n")
    print(f"\nSaved short report to: {report_path}")
except Exception as e:
    print(f"\nCould not save report file: {e}")