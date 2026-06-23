#pragma once
#include "robotiq_gripper_driver/gripper_server.hpp"
#include "robotiq_gripper_msgs/action/activate.hpp"

void execute_activate(
  std::shared_ptr<rclcpp_action::ServerGoalHandle<
    robotiq_gripper_msgs::action::Activate>> goal_handle,
  GripperServer & node);
