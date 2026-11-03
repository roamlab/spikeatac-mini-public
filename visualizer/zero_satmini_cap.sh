#!/bin/bash
# Republishes /satmini_capacitive_feedback as a baseline-zeroed topic,
# /satmini_capacitive_feedback_zeroed. Needed for the PlotJuggler config in
# plotjuggler_configs/satmini_multimodal_1.xml, which plots the zeroed topic.
# Not needed for visualizer/boot_spikeatac_mini_viz.sh, which zeroes internally.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

python3 "$SCRIPT_DIR/zero_signals.py" \
  --input-msg-type UInt16 \
  --input-topic /satmini_capacitive_feedback \
  --output-topic /satmini_capacitive_feedback_zeroed
