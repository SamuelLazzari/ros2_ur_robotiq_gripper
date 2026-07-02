#pragma once
#include "robotiq_gripper_driver/gripper_server.hpp"
#include "robotiq_gripper_msgs/action/open.hpp"

void execute_open(
  std::shared_ptr<rclcpp_action::ServerGoalHandle<
    robotiq_gripper_msgs::action::Open>> goal_handle,
  GripperServer & node);
