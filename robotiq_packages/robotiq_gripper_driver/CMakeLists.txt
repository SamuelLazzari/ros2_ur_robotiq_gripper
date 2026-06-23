cmake_minimum_required(VERSION 3.8)
project(robotiq_gripper_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── Dipendenze ──────────────────────────────────────────────────────────────
find_package(ament_cmake          REQUIRED)
find_package(rclcpp               REQUIRED)
find_package(rclcpp_action        REQUIRED)
find_package(robotiq_gripper_msgs REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(sensor_msgs REQUIRED)  # ← aggiungi questa dipendenza per poter utilizzare sensor_msgs::msg::JointState

# ── Sorgenti ────────────────────────────────────────────────────────────────
set(EXECUTE_SOURCES
  src/execute/execute_activate.cpp
  src/execute/execute_open.cpp
  src/execute/execute_close.cpp
  src/execute/execute_move.cpp
  src/execute/execute_set_force.cpp
  src/execute/execute_set_speed.cpp
  src/execute/execute_status.cpp
)

# ── Target ──────────────────────────────────────────────────────────────────
add_executable(robotiq_gripper_server
  src/main.cpp
  src/gripper_server.cpp
  ${EXECUTE_SOURCES}
)

# Nodo di calibrazione NUOVO
add_executable(robotiq_calibration_node
  src/calibration_node.cpp
)

target_include_directories(robotiq_gripper_server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# NUOVO
target_include_directories(robotiq_calibration_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  ${UR_RTDE_INCLUDE_DIR}
)

# ament_target_dependencies gestisce rclcpp e i pacchetti ament
ament_target_dependencies(robotiq_gripper_server
  rclcpp
  rclcpp_action
  robotiq_gripper_msgs
  ament_index_cpp
  sensor_msgs              # ← aggiungi questa dipendenza per poter utilizzare sensor_msgs::msg::JointState
)

# NUOVO
ament_target_dependencies(robotiq_calibration_node
  rclcpp
  ament_index_cpp
)

target_link_libraries(robotiq_gripper_server
   rtde
)

# NUOVO
target_link_libraries(robotiq_calibration_node
   rtde
)

# ── Install ─────────────────────────────────────────────────────────────────
install(TARGETS robotiq_gripper_server
  DESTINATION lib/${PROJECT_NAME}
)

# NUOVO
install(TARGETS robotiq_calibration_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

# Installa launch file e cartella config, NUOVO
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)
install(DIRECTORY config/  DESTINATION share/${PROJECT_NAME}/config)


ament_package()













