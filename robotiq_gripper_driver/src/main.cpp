#include "rclcpp/rclcpp.hpp"
#include "robotiq_gripper_driver/gripper_server.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<GripperServer>());
  rclcpp::shutdown();
  return 0;
}
