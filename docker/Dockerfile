# Docker setup adapted from https://github.com/roamlab/multimodal-firmware, trimmed
# to only what spikeatac-mini needs: ROS2 Humble, the micro-ROS agent, PlotJuggler,
# and a small python env for the visualizer/zeroing scripts.

FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC
ENV ROS_DISTRO=humble
ENV PYTHONUNBUFFERED=1

SHELL ["/bin/bash", "-lc"]


# === System ===

RUN apt-get update && apt-get install -y \
    curl \
    wget \
    gnupg2 \
    lsb-release \
    ca-certificates \
    software-properties-common \
    build-essential \
    cmake \
    pkg-config \
    git \
    vim \
    nano \
    htop \
    tree \
    usbutils \
    x11-apps


# === ROS2 ===

RUN curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | tee /etc/apt/sources.list.d/ros2.list > /dev/null \
    && apt-get update \
    && apt-get install -y \
        ros-humble-desktop \
        python3-rosdep \
        python3-colcon-common-extensions \
        python3-catkin-pkg-modules \
        python3-pip

RUN apt-get install -y python3-catkin-pkg

# Initialize rosdep
RUN rosdep init && rosdep update


# === micro-ROS Agent ===
# Note: Must be installed before miniforge to use system Python with catkin_pkg

WORKDIR /uros_ws
RUN git clone -b humble --single-branch https://github.com/micro-ROS/micro_ros_setup.git src/micro_ros_setup

RUN apt-get update && rosdep update \
    && rosdep install --from-paths src --ignore-src -y

RUN source /opt/ros/humble/setup.bash \
    && colcon build \
    && source install/local_setup.bash \
    && ros2 run micro_ros_setup create_agent_ws.sh \
    && ros2 run micro_ros_setup build_agent.sh


# === Miniforge (Mamba) ===

RUN wget https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh -O /tmp/miniforge.sh \
    && bash /tmp/miniforge.sh -b -p /opt/miniforge3 \
    && rm /tmp/miniforge.sh

ENV PATH="/opt/miniforge3/bin:${PATH}"

RUN conda config --set channel_priority strict


# === Setup Container + Environment ===

# Create user with same UID/GID as host user
ARG USER_ID=1000
ARG GROUP_ID=1000
RUN groupadd -g ${GROUP_ID} dockeruser && \
    useradd -u ${USER_ID} -g ${GROUP_ID} -m -s /bin/bash dockeruser && \
    usermod -a -G dialout dockeruser

RUN mkdir -p /opt/miniforge3/envs && \
    chown -R dockeruser:dockeruser /opt/miniforge3/envs && \
    chmod -R u+w /opt/miniforge3/envs

USER dockeruser
WORKDIR /home/dockeruser

# Create environment using mamba (faster than conda)
COPY docker/conda_environment_docker.yaml /tmp/
RUN mamba env create -f /tmp/conda_environment_docker.yaml

# Initialize mamba shell (like conda init) - sets up .bashrc for automatic initialization
RUN mamba shell init --shell bash --root-prefix /opt/miniforge3

USER root

# Set up workspace directory
WORKDIR /work
RUN mkdir -p /work && \
    chown -R dockeruser:dockeruser /work


# === PlotJuggler ===

RUN apt-get update && apt-get install -y \
    ros-humble-plotjuggler \
    ros-humble-plotjuggler-ros \
    ros-humble-plotjuggler-msgs


# Cleanup for lighter image:
RUN rm -rf /var/lib/apt/lists/*

USER dockeruser

CMD ["/bin/bash"]
