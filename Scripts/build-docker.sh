#!/usr/bin/env bash
# Scripts/build-docker.sh
# Build inside the pinned Docker toolchain image — byte-for-byte the same
# environment CI uses. The repo is bind-mounted; output lands in build/<CONFIG>/
# on the host just like a native build.
#
# Usage: ./Scripts/build-docker.sh [Debug|Release]
# (On Windows, run from Git Bash / WSL, or use the docker commands from the README.)

set -euo pipefail

CONFIG="${1:-Debug}"
case "$CONFIG" in
    Debug|Release) ;;
    *) echo "ERROR: CONFIG must be Debug or Release (got '$CONFIG')"; exit 1 ;;
esac

PROJECT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMAGE="stm32-dyno-builder"

command -v docker >/dev/null 2>&1 || {
    echo "ERROR: docker not found. Install Docker first."
    exit 1
}

echo "Building toolchain image ($IMAGE)..."
docker build -t "$IMAGE" "$PROJECT_PATH"

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
    bash -lc "cmake --preset $CONFIG && cmake --build --preset $CONFIG"

echo
echo "Build succeeded! Artifacts in build/$CONFIG/"
