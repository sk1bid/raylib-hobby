#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# build_web.sh – one-shot Emscripten build for a raylib WebAssembly game
# -----------------------------------------------------------------------------
# Usage:
#   ./build_web.sh [debug] <game_name>
#       debug       – (optional) build with -O0 -g4 and ASSERTIONS=1
#       game_name   – folder in ../src/ containing the game’s source tree:
#
#           ../src/<game_name>/
#               ├── src/        (C files)
#               ├── include/    (optional headers)
#               └── resources/  (assets to pack into the .wasm)
#
# Environment variables you can override:
#   RAYLIB_WEB   – path to libraylib.web.a (default: ../../raylib/src)
#   OUTDIR_ROOT  – where built files are placed (default: ../site-root)
# -----------------------------------------------------------------------------
set -euo pipefail

# --- paths -------------------------------------------------------------------
RAYLIB_WEB="${RAYLIB_WEB:-../../raylib/src}"
OUTDIR_ROOT="${OUTDIR_ROOT:-../site-root}"

# --- parse CLI ---------------------------------------------------------------
MODE="release"
if [[ "${1:-}" == "debug" ]]; then
  MODE="debug"
  shift
fi

GAME="${1:-}"
if [[ -z "$GAME" ]]; then
  echo "Usage: ./build_web.sh [debug] <game_name>"
  exit 1
fi

SRC_DIR="../src/$GAME/src"
INC_DIR="../src/$GAME/include"
RES_DIR="../src/$GAME/resources"
OUTDIR="$OUTDIR_ROOT/$GAME"

# --- sanity checks -----------------------------------------------------------
[[ -d "$SRC_DIR" ]] || { echo "[error] source dir not found: $SRC_DIR" >&2; exit 1; }
mkdir -p "$OUTDIR"

# --- gather sources ----------------------------------------------------------
SRC_FILES=$(find "$SRC_DIR" -type f -name '*.c' -print0 | sort -z | tr '\0' ' ')
[[ -n "$SRC_FILES" ]] || { echo "[error] no .c files found in $SRC_DIR" >&2; exit 1; }

# --- build flags -------------------------------------------------------------
if [[ "$MODE" == "debug" ]]; then
  CFLAGS="-O0 -g4 -s ASSERTIONS=1"
  echo "[build] DEBUG mode"
else
  CFLAGS="-Os"
  echo "[build] RELEASE mode"
fi

INCLUDE_FLAGS="-I$SRC_DIR"
[[ -d "$INC_DIR" ]] && INCLUDE_FLAGS+=" -I$INC_DIR"
INCLUDE_FLAGS+=" -I$RAYLIB_WEB"

# --- locate raylib static library -------------------------------------------
if   [[ -f "$RAYLIB_WEB/libraylib.web.a" ]]; then RAYLIB_LIB="$RAYLIB_WEB/libraylib.web.a"
elif [[ -f "$RAYLIB_WEB/libraylib.a"     ]]; then RAYLIB_LIB="$RAYLIB_WEB/libraylib.a"
else
  echo "[error] libraylib Web library not found in $RAYLIB_WEB" >&2
  exit 1
fi

# --- emscripten compile ------------------------------------------------------
PRELOAD_OPT=""
[[ -d "$RES_DIR" ]] && PRELOAD_OPT="--preload-file $RES_DIR@resources"

set -x
emcc $SRC_FILES \
     -o "$OUTDIR/index.html" \
     $CFLAGS -Wall \
     $INCLUDE_FLAGS \
     "$RAYLIB_LIB" \
     -s USE_GLFW=3 \
     -s ASYNCIFY \
     --shell-file "$RAYLIB_WEB/minshell.html" \
     $PRELOAD_OPT \
     -s TOTAL_MEMORY=67108864 \
     -DPLATFORM_WEB
set +x

echo -e "\n[ok] Build complete → $OUTDIR/index.html"
