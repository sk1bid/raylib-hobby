#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# build_web.sh – one‑shot Emscripten build for the raylib WebAssembly version
# -----------------------------------------------------------------------------
# Usage:
#   ./build_web.sh           # release (‑Os)
#   ./build_web.sh debug     # debug  (‑O0 ‑g4 assertions)
#
# Environment variables that can override defaults:
#   RAYLIB_WEB   – path to folder containing libraylib.web.a (default: ../raylib/src)
#   OUTDIR       – output folder (default: build/web)
# -----------------------------------------------------------------------------
set -euo pipefail

# ---- config -----------------------------------------------------------------
RAYLIB_WEB="${RAYLIB_WEB:-../../raylib/src}"
OUTDIR="${OUTDIR:-bin/site-root/Doodler}"

mkdir -p "$OUTDIR"

# collect all .c files under src/
SRC_FILES=$(find src -type f -name '*.c' | sort | tr '\n' ' ')

# choose optimisation level
if [[ "${1:-}" == "debug" ]]; then
  CFLAGS="-O0 -g4 -s ASSERTIONS=1"
  echo "[build] debug mode"
else
  CFLAGS="-Os"
  echo "[build] release mode"
fi

# pick correct raylib static library name
if [[ -f "$RAYLIB_WEB/libraylib.web.a" ]]; then
  RAYLIB_LIB="$RAYLIB_WEB/libraylib.web.a"
elif [[ -f "$RAYLIB_WEB/libraylib.a" ]]; then
  RAYLIB_LIB="$RAYLIB_WEB/libraylib.a"
else
  echo "\e[31m[error] libraylib Web library not found in $RAYLIB_WEB\e[0m" >&2
  exit 1
fi

# ---- compilation -------------------------------------------------------------
set -x   # echo commands
emcc $SRC_FILES \
  -o "$OUTDIR/index.html" \
  $CFLAGS -Wall \
  -Iinclude -I"$RAYLIB_WEB" \
  "$RAYLIB_LIB" \
  -s USE_GLFW=3 \
  -s ASYNCIFY \
  --shell-file "$RAYLIB_WEB/minshell.html" \
  --preload-file resources \
  -s TOTAL_MEMORY=67108864 \
  -DPLATFORM_WEB
set +x

echo -e "\n\e[32m[ok] Build complete → $OUTDIR/index.html\e[0m"
