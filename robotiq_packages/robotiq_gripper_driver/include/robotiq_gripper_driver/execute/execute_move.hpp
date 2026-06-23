#pragma once
#include "robotiq_gripper_driver/gripper_server.hpp"
#include "robotiq_gripper_msgs/action/move.hpp"

void execute_move(
  std::shared_ptr<rclcpp_action::ServerGoalHandle<
    robotiq_gripper_msgs::action::Move>> goal_handle,
  GripperServer & node);
