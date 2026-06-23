cmake_minimum_required(VERSION 3.8)
project(robotiq_gripper_msgs)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "action/Activate.action"
  "action/Open.action"
  "action/Close.action"
  "action/Move.action"
  "action/SetForce.action"
  "action/SetSpeed.action"
  "action/Status.action"
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
