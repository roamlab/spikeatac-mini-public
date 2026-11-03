#!/bin/bash

# Shared alias setup for the spikeatac-mini container.
# This file is sourced by both run.sh and exec.sh

# Source ROS2 humble
source /opt/ros/humble/setup.bash

alias sourceenv="source /work/docker/source_env.sh"
alias pokestuff="source /uros_ws/install/setup.bash && ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyACM0"
alias pokestuff1="source /uros_ws/install/setup.bash && ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyACM1"
alias humble="source /opt/ros/humble/setup.bash && echo sourced ROS2 humble"
alias uros="source /uros_ws/install/setup.bash && echo sourced micro-ROS workspace"

echo ""
echo "Available commands:"
echo "  sourceenv  - activate mamba env and source all workspaces"
echo "  pokestuff  - run micro-ROS agent on /dev/ttyACM0"
echo "  pokestuff1 - run micro-ROS agent on /dev/ttyACM1"
echo "  humble     - source ROS2 humble"
echo "  uros       - source micro-ROS workspace"
echo ""
echo "Use ./exec.sh to enter the container in another terminal."
echo ""
