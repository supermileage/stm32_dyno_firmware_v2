#!/usr/bin/env bash
# Scripts/build.sh
# Configure and build the CMake project (arm-none-eabi-gcc + Ninja).
# Optionally regenerate sources from the .ioc first if CUBEMX is set.
#
# Output (.elf/.hex/.bin/.map) lands in build/<CONFIG>/.

set -uo pipefail

usage() {
    cat <<'EOF'
Usage: ./Scripts/build.sh [CONFIG]

  CONFIG   - Debug (default) or Release

Environment variables:
  CUBEMX   - Path to STM32CubeMX. When set, code is regenerated from the
             .ioc before building (you only need this after editing the .ioc).
EOF
}

case "${1:-}" in
    -h|--help) usage; exit 0 ;;
esac

CONFIG="${1:-Debug}"
case "$CONFIG" in
    Debug|Release) ;;
    *) echo "ERROR: CONFIG must be Debug or Release (got '$CONFIG')"; exit 1 ;;
esac

PROJECT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_PATH"

# Required host tools
MISSING=0
for t in cmake ninja arm-none-eabi-gcc; do
    if ! command -v "$t" >/dev/null 2>&1; then
        echo "ERROR: '$t' not found on PATH"
        MISSING=1
    fi
done
if [[ $MISSING -ne 0 ]]; then
    echo "Install with: sudo dnf install cmake ninja-build arm-none-eabi-gcc-cs arm-none-eabi-newlib"
    exit 1
fi

# Optional: regenerate code from the .ioc via STM32CubeMX
if [[ -n "${CUBEMX:-}" ]]; then
    if [[ ! -x "$CUBEMX" ]]; then
        echo "ERROR: STM32CubeMX not executable at $CUBEMX"
        exit 1
    fi
    mapfile -t IOC_FILES < <(find "$PROJECT_PATH" -maxdepth 1 -name '*.ioc' | sort)
    if [[ ${#IOC_FILES[@]} -eq 0 ]]; then
        echo "ERROR: No .ioc file found in $PROJECT_PATH"
        exit 1
    fi
    echo "Regenerating from $(basename "${IOC_FILES[0]}")..."
    MX_SCRIPT="$(mktemp)"
    trap 'rm -f "$MX_SCRIPT"' EXIT
    printf 'config load %s\nproject generate\nexit\n' "${IOC_FILES[0]}" > "$MX_SCRIPT"
    MX_LAUNCHER=("$CUBEMX")
    if [[ -z "${DISPLAY:-}" ]] && command -v xvfb-run >/dev/null 2>&1; then
        MX_LAUNCHER=(xvfb-run -a "$CUBEMX")
    fi
    "${MX_LAUNCHER[@]}" -q "$MX_SCRIPT"
fi

echo "Configuring ($CONFIG)..."
cmake --preset "$CONFIG"

echo "Building ($CONFIG)..."
cmake --build --preset "$CONFIG"

echo
echo "Build succeeded! Artifacts in build/$CONFIG/:"
ls -1 "build/$CONFIG"/*.elf "build/$CONFIG"/*.hex "build/$CONFIG"/*.bin 2>/dev/null
