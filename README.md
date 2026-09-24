# A ROS 2 package collection for controlling and visualizing Robotiq 2F-140 gripper

This project provides a complete ROS 2 solution for Robotiq 2F-140 gripper, featuring a modular driver, custom ROS 2 actions, calibration tools, and URDF/Xacro descriptions for visualization and integration with Universal Robots.
The software was developed at [RIMLab](https://rimlab.ce.unipr.it/), the robotic laboratory of the University of Parma.

**Proceed with care when utilizing this application. The makers cannot be held accountable for any harm or damage that might occur from its operation.**

## Features

- Full control of Robotiq 2F-140 gripper.
- Position, force, and speed configuration.
- Gripper status feedback.
- ROS 2 Action Server-based interface.
- Calibration utilities.
- RViz visualization and URDF/Xacro descriptions.
- Universal Robots and ros2_control integration.



## Repository Structure
- [**`custom_robotiq_description`**] 
  Contains the robot descriptions and visualization files for the Robotiq 2F-140 gripper.
  
  - `config/` – ros2_control controller configuration files.
  - `launch/` – Launch files for RViz visualization and ros2_control.
  - `rviz/` – RViz configuration files.
  - `urdf/` – URDF/Xacro models for the 2F-140 gripper, including Universal Robots adapters and ros2_control support.

- [**`robotiq_gripper_driver`**]
  Implements the ROS 2 gripper driver and exposes all functionalities through ROS 2 Action Servers.

  - `include/execute/` – Headers for the implementation of each gripper command.
  - `src/execute/` – Source files implementing the gripper actions:
    - Activate
    - Open
    - Close
    - Move to position
    - Set force
    - Set speed
    - Get gripper status
  - `gripper_server.*` – Action server implementation.
  - `calibration_node.cpp` – Gripper calibration utility.
  - `launch/` – Launch files for the driver and calibration node.

- [**`robotiq_gripper_msgs`**] 
  Defines the custom ROS 2 action interfaces used by the driver.

- [**`Root Files`**] 
- `.gitignore` – Git configuration file.
- `.gitmodules` – Git submodules configuration.
- `CMakeLists.txt` – Build instructions for each ROS 2 package.
- `package.xml` – ROS 2 package metadata and dependencies.

## Prerequisites

Before using this project, make sure the following dependencies are installed on your system.

#### Install Robot Operating System 2 (ROS 2):

This project has been developed and tested with **ROS 2 Jazzy** on **Ubuntu 24.04**.

Follow the official ROS 2 installation guide:
https://docs.ros.org/en/jazzy/Installation.html

Install the required ROS 2 packages:

```bash
sudo apt update

sudo apt install python3-colcon-common-extensions
sudo apt install ros-jazzy-xacro
sudo apt install ros-jazzy-controller-manager
sudo apt install ros-jazzy-robot-state-publisher
sudo apt install ros-jazzy-rviz2
```

#### Install `ur_rtde`:

The gripper driver communicates with the robot using the **ur_rtde** library.

Install it from the official repository:

```bash
sudo add-apt-repository ppa:sdurobotics/ur-rtde
sudo apt update
sudo apt install librtde librtde-dev
```

#### Install `ur_ros_rtde`:

```bash
cd <ros_workspace_path>/src
# clone ur_ros_rtde repository
git clone https://github.com/SuperDiodo/ur_ros_rtde.git
cd ..
# build ur_ros_rtde only (optional packages will be ignored)
colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release --packages-up-to ur_ros_rtde
# or build the entire workspace
colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release
```
