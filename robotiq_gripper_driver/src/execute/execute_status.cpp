#include "robotiq_gripper_driver/execute/execute_status.hpp"

void execute_status(
  std::shared_ptr<rclcpp_action::ServerGoalHandle<
    robotiq_gripper_msgs::action::Status>> goal_handle,
  GripperServer & node)
{
  auto result = std::make_shared<robotiq_gripper_msgs::action::Status::Result>();

  try {
    double raw      = node.gripper_->getCurrentPosition();
    double pos_mm   = node.to_mm(raw);
    int    status   = node.gripper_->objectDetectionStatus();
    std::string status_str = node.get_status_string(status);

    RCLCPP_INFO(rclcpp::get_logger("execute_status"),
      "STATUS → pos: %.2f mm | status: %s", pos_mm, status_str.c_str());

    result->current_position_mm     = pos_mm;
    result->object_detection_status = status;
    result->success = true;
    result->message = status_str;
    goal_handle->succeed(result);

  } catch (const std::exception & e) {
    result->success = false;
    result->message = e.what();
    goal_handle->abort(result);
  }
}
