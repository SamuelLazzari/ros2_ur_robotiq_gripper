#include "robotiq_gripper_driver/execute/execute_set_speed.hpp"

void execute_set_speed(
  std::shared_ptr<rclcpp_action::ServerGoalHandle<
    robotiq_gripper_msgs::action::SetSpeed>> goal_handle,
  GripperServer & node)
{
  const auto goal = goal_handle->get_goal();
  auto result = std::make_shared<robotiq_gripper_msgs::action::SetSpeed::Result>();

  try {
    node.gripper_->setSpeed(goal->value);

    result->success = true;
    result->message = "Speed set successfully";
    goal_handle->succeed(result);

  } catch (const std::exception & e) {
    result->success = false;
    result->message = e.what();
    goal_handle->abort(result);
  }
}
