#!/bin/bash
# Boots the spikeatac-mini visualizer.
#
# Plain standalone python script -- no plotjuggler, no colcon workspace to
# build. It subscribes directly to the raw /satmini_pvdf_feedback and
# /satmini_capacitive_feedback topics and does its own baseline zeroing.
#
# Usage: visualizer/boot_spikeatac_mini_viz.sh [extra args passed to visualize.py]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VIZ_SCRIPT="$SCRIPT_DIR/visualize.py"

if [[ ! -f "$VIZ_SCRIPT" ]]; then
  echo "ERROR: missing $VIZ_SCRIPT"
  exit 1
fi

source /opt/ros/humble/setup.bash

echo "[boot] launching spikeatac-mini visualizer"

exec python3 "$VIZ_SCRIPT" "$@"
