#pragma once
#include "robotiq_gripper_driver/gripper_server.hpp"
#include "robotiq_gripper_msgs/action/status.hpp"

void execute_status(
  std::shared_ptr<rclcpp_action::ServerGoalHandle<
    robotiq_gripper_msgs::action::Status>> goal_handle,
  GripperServer & node);
