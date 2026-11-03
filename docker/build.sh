#!/bin/bash

# Build script for the spikeatac-mini Docker container

echo "Building spikeatac-mini Docker container..."

set -e

export USER_ID=$(id -u)
export GROUP_ID=$(id -g)

echo "Building with UID: $USER_ID, GID: $GROUP_ID"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

docker compose build spikeatac-mini

echo ""
echo "Build complete! You can now run:"
echo "  ./run.sh"
echo "  ./exec.sh"
