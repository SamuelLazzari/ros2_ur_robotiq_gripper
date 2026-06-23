#include "robotiq_gripper_driver/execute/execute_move.hpp"

void execute_move(
  std::shared_ptr<rclcpp_action::ServerGoalHandle<
    robotiq_gripper_msgs::action::Move>> goal_handle,
  GripperServer & node)
{
  const auto goal = goal_handle->get_goal();
  auto result = std::make_shared<robotiq_gripper_msgs::action::Move::Result>();

  try {
    double target = node.from_mm(goal->position_mm);
    node.gripper_->move(target, goal->speed, goal->force);
    node.gripper_->waitForMotionComplete();

    double raw = node.gripper_->getCurrentPosition();
    result->current_position_mm     = node.to_mm(raw);
    result->object_detection_status = node.gripper_->objectDetectionStatus();
    result->success = true;
    result->message = "Command executed successfully";
    goal_handle->succeed(result);

  } catch (const std::exception & e) {
    result->success = false;
    result->message = e.what();
    goal_handle->abort(result);
  }
}
