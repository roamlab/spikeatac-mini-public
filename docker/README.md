# Docker setup for spikeatac-mini

This directory contains everything needed to run the ROS2/micro-ROS side of
spikeatac-mini in a container: ROS2 Humble, the micro-ROS agent (built from
source against the `humble` branch), PlotJuggler, and a small Python env
(numpy/scipy/opencv/click) for the zeroing script and visualizer.

Flashing the Teensy itself is **not** done through this container — see the
top-level README's Firmware section. This container is only for running the
micro-ROS agent, PlotJuggler, and the visualizer once the Teensy is flashed
and connected.

## Prerequisites

- Docker and Docker Compose installed.
- Only tested on Ubuntu 22.04 (the scripts assume a native X11 display and
  `/dev/ttyACM*` serial devices).

## Quick start

```bash
# From the repo root
cd docker

# First-time setup: build the image
./build.sh

# Start the container and open a shell in it
./run.sh

# From another terminal, attach a second shell to the same container
./exec.sh

# Inside the container, activate the python env + ROS2 + workspaces
sourceenv
```

`run.sh` and `exec.sh` both source `setup_aliases.sh`, which defines:

- `sourceenv` — activate the `spikeatac_mini_env` mamba env and source ROS2.
- `pokestuff` / `pokestuff1` — run the micro-ROS agent against
  `/dev/ttyACM0` / `/dev/ttyACM1`.
- `humble` — source ROS2 Humble only.
- `uros` — source the micro-ROS workspace only.

The repo root is mounted into the container at `/work`, so e.g.
`visualizer/boot_spikeatac_mini_viz.sh` on the host is
`/work/visualizer/boot_spikeatac_mini_viz.sh` in the container.
