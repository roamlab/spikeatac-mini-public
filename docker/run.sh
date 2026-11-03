#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

export USER_ID=$(id -u)
export GROUP_ID=$(id -g)

# Allow local connections to X server
xhost +local:docker > /dev/null 2>&1

# Start container in detached mode
docker compose up -d spikeatac-mini

# Add setup_aliases.sh to .bashrc if not already there
docker compose exec spikeatac-mini bash -c "grep -q 'setup_aliases.sh' ~/.bashrc || echo 'source /work/docker/setup_aliases.sh' >> ~/.bashrc"

# Enter container with interactive shell (sources .bashrc, which has mamba init + aliases)
docker compose exec spikeatac-mini bash
