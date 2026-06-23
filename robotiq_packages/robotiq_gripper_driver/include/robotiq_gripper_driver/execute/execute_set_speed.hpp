#pragma once
#include "robotiq_gripper_driver/gripper_server.hpp"
#include "robotiq_gripper_msgs/action/set_speed.hpp"

void execute_set_speed(
  std::shared_ptr<rclcpp_action::ServerGoalHandle<
    robotiq_gripper_msgs::action::SetSpeed>> goal_handle,
  GripperServer & node);
