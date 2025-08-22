#!/usr/bin/env python3
import os, re, sys
from pathlib import Path

# --- Locate base directory and manifest --------------------------------------
base_dir = Path(__file__).resolve().parent
manifest_path = base_dir / "manifest.txt"
if not manifest_path.exists():
    # Also allow manifest.txt to live in parent if script is inside map\
    parent_manifest = base_dir.parent / "manifest.txt"
    if parent_manifest.exists():
        manifest_path = parent_manifest
        base_dir = manifest_path.parent
    else:
        print(f"[ERROR] manifest.txt not found in {base_dir} or its parent.")
        sys.exit(1)

# --- Locate map directory -----------------------------------------------------
# Case A: typical layout -> base_dir/map
map_dir = base_dir / "map"
# Case B: script is inside map folder (contains chunk_* subdirs)
if not map_dir.is_dir():
    if any(p.is_dir() and p.name.lower().startswith("chunk_") for p in Path(__file__).parent.iterdir()):
        map_dir = Path(__file__).parent
if not map_dir.is_dir():
    print(f"[ERROR] Could not locate your map folder.\nTried: {base_dir/'map'} and current folder.")
    sys.exit(1)

# --- Parse manifest: collect normalized chunk-relative paths ------------------
# We look for tokens that look like:
#   map/chunk_...*.obj   OR   chunk_...*.obj
token_re = re.compile(r'(?:map[\\/])?chunk_[^\s]*?\.obj', re.IGNORECASE)

def canon_rel(s: str) -> str:
    s = s.replace('\\', '/')
    if s.startswith('./'): s = s[2:]
    if s.lower().startswith('map/'): s = s[4:]
    s = s.strip()
    return s

manifest_set = set()
with manifest_path.open('r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        for m in token_re.findall(line):
            rel = canon_rel(m)
            if rel.lower().endswith('.obj') and '/tile_' in rel.lower():
                # store lowercase for matching robustness
                manifest_set.add(rel.lower())

# --- Walk filesystem: find all tile_*.obj under map_dir -----------------------
orphans = []
for root, _, files in os.walk(map_dir):
    for name in files:
        if not name.lower().endswith('.obj'): 
            continue
        if not name.lower().startswith('tile_'):
            continue
        full = Path(root) / name
        # Build chunk-relative form to compare against manifest_set
        try:
            rel = full.relative_to(map_dir).as_posix()  # forward slashes
        except ValueError:
            # Shouldn't happen, but be safe
            rel = full.as_posix()
        rel_canon = canon_rel(rel).lower()
        if rel_canon not in manifest_set:
            orphans.append(full.resolve())

# --- Write the list to to_delete.txt (absolute paths for bullet-proof delete) -
out_path = base_dir / "to_delete.txt"
with out_path.open('w', encoding='utf-8') as out:
    for p in sorted(orphans):
        out.write(str(p) + '\n')

print(f"Manifest file : {manifest_path}")
print(f"Map folder    : {map_dir}")
print(f"Manifest refs : {len(manifest_set)}")
print(f"Found tiles   : {sum(1 for _ in (p for _,_,fs in os.walk(map_dir) for p in fs if p.lower().startswith('tile_') and p.lower().endswith('.obj')))}")
print(f"Orphans       : {len(orphans)}")
print(f"Wrote         : {out_path}")
