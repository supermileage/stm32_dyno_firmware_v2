#!/usr/bin/env bash
# Scripts/build-docker.sh
# Build inside the pinned Docker toolchain image — byte-for-byte the same
# environment CI uses. The repo is bind-mounted; output lands in build/<CONFIG>/
# on the host just like a native build.
#
# Usage: ./Scripts/build-docker.sh [Debug|Release] [--rebuild]
#   Debug|Release  build configuration (default: Debug)
#   --rebuild, -r  rebuild the toolchain image (do this after editing the
#                  Dockerfile; otherwise the existing image is reused)
#
# (On Windows, run from Git Bash / WSL, or use the docker commands from the README.)

set -euo pipefail

IMAGE="stm32-dyno-builder"
CONFIG="Debug"
REBUILD=0

for arg in "$@"; do
    case "$arg" in
        Debug|Release) CONFIG="$arg" ;;
        -r|--rebuild)  REBUILD=1 ;;
        -h|--help)     sed -n '2,12p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "ERROR: unknown argument '$arg' (expected Debug|Release|--rebuild)"; exit 1 ;;
    esac
done

PROJECT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

command -v docker >/dev/null 2>&1 || {
    echo "ERROR: docker not found. Install Docker first."
    exit 1
}

# Build the image only when missing or explicitly requested. The Dockerfile
# bakes in just the toolchain (the repo is bind-mounted, never COPYed), so the
# image rarely needs rebuilding — only after the Dockerfile itself changes.
if [[ $REBUILD -eq 1 ]] || ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
    echo "Building toolchain image ($IMAGE)..."
    docker build -t "$IMAGE" "$PROJECT_PATH"
else
    echo "Reusing existing image $IMAGE (pass --rebuild to refresh it)."
fi

# On Linux, run as the host user so build artifacts aren't owned by root, and
# add the ':z' SELinux relabel so the bind mount is readable on SELinux hosts
# (Fedora/RHEL). ':z' is a no-op where SELinux is absent.
# (Docker Desktop on macOS/Windows maps ownership automatically.)
MOUNT="$PROJECT_PATH:/work"
USER_FLAGS=()
if [[ "$(uname)" == "Linux" ]]; then
    MOUNT="$MOUNT:z"
    USER_FLAGS=(--user "$(id -u):$(id -g)" -e HOME=/tmp)
fi

echo "Building firmware ($CONFIG)..."
docker run --rm \
    -v "$MOUNT" -w /work \
    "${USER_FLAGS[@]}" \
    "$IMAGE" \
    bash -lc "python3 tools/message_gen/generate.py && cmake --preset $CONFIG && cmake --build --preset $CONFIG"

echo
echo "Build succeeded! Artifacts in build/$CONFIG/"
