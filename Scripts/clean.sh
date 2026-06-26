#!/usr/bin/env bash
# Scripts/clean.sh
# Remove CMake build output. Does NOT touch generated C sources (tracked in git).

set -uo pipefail

PROJECT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "Cleaning build output in $PROJECT_PATH ..."

if [[ -d "$PROJECT_PATH/build" ]]; then
    echo "  removing build/"
    rm -rf "${PROJECT_PATH:?}/build"
fi

# Legacy STM32CubeIDE artifacts, if any remain from before the CMake migration
for d in Debug debug Release release; do
    if [[ -d "$PROJECT_PATH/$d" ]]; then
        echo "  removing $d/ (legacy CubeIDE output)"
        rm -rf "${PROJECT_PATH:?}/$d"
    fi
done
[[ -f "$PROJECT_PATH/build_log.txt" ]] && { echo "  removing build_log.txt"; rm -f "$PROJECT_PATH/build_log.txt"; }

echo "Clean complete."
