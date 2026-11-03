#!/bin/bash

# Environment setup script for spikeatac_mini_env
# Activates mamba, sources ROS2, and sets up PYTHONPATH

echo "Setting up spikeatac_mini_env environment..."

mamba activate spikeatac_mini_env

ENV_PATH="${CONDA_PREFIX:-/opt/miniforge3/envs/spikeatac_mini_env}"
PYTHON_VERSION=$(python -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')" 2>/dev/null || echo "3.10")

export LD_LIBRARY_PATH="${ENV_PATH}/lib:$LD_LIBRARY_PATH"

echo "Sourcing ROS2 Humble..."
source /opt/ros/humble/setup.bash

# --- Python paths (order matters) ---
export PYTHONPATH="${ENV_PATH}/lib/python${PYTHON_VERSION}/site-packages:$PYTHONPATH"
export PYTHONPATH="/opt/ros/humble/lib/python3.10/site-packages:$PYTHONPATH"
export PYTHONPATH="/opt/ros/humble/local/lib/python3.10/dist-packages:$PYTHONPATH"

echo "Environment ready! (spikeatac_mini_env) is activated."
