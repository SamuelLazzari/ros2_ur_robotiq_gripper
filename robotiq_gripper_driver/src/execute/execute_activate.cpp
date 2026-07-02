#include "robotiq_gripper_driver/execute/execute_activate.hpp"

void execute_activate(
  std::shared_ptr<rclcpp_action::ServerGoalHandle<
    robotiq_gripper_msgs::action::Activate>> goal_handle,
  GripperServer & node)
{
  auto result = std::make_shared<robotiq_gripper_msgs::action::Activate::Result>();

  try {
    node.gripper_->activate();

    double raw = node.gripper_->getCurrentPosition();
    result->current_position_mm    = node.to_mm(raw);
    result->object_detection_status = node.gripper_->objectDetectionStatus();
    result->success = true;
    result->message = "Gripper activated successfully";
    goal_handle->succeed(result);

  } catch (const std::exception & e) {
    result->success = false;
    result->message = e.what();
    goal_handle->abort(result);
  }
}
