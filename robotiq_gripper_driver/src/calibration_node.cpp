#include <fstream>
#include <cmath>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include <ur_rtde/robotiq_gripper.h>
#include <ament_index_cpp/get_package_share_directory.hpp>

using namespace ur_rtde;

class CalibrationNode : public rclcpp::Node
{
public:
  CalibrationNode() : Node("robotiq_calibration_node")
  {
    this->declare_parameter<std::string>("robot_ip",   "160.78.27.23");
    this->declare_parameter<int>        ("robot_port", 63352);
    this->declare_parameter<bool>       ("verbose",    true);

    std::string ip   = this->get_parameter("robot_ip").as_string();
    int         port = this->get_parameter("robot_port").as_int();
    bool        verbose = this->get_parameter("verbose").as_bool();

    gripper_ = std::make_unique<RobotiqGripper>(ip, port, verbose);

    try {
      gripper_->connect();
      gripper_->setUnit(RobotiqGripper::POSITION, RobotiqGripper::UNIT_DEVICE);

      calibrate();
      save();

    } catch (const std::exception & e) {
      RCLCPP_FATAL(get_logger(), "Calibration failed: %s", e.what());
    }

    // Spegnimento automatico dopo aver finito
    rclcpp::shutdown();
  }

private:
  std::unique_ptr<RobotiqGripper> gripper_;
  double raw_open_;
  double raw_closed_;

  void calibrate()
  {
    if (!gripper_->isActive()) {
      RCLCPP_WARN(get_logger(), "Gripper not active, activating...");
      gripper_->activate();
      if (!gripper_->isActive())
        throw std::runtime_error("Gripper failed to activate");
    }

    if (gripper_->faultStatus() != 0)
      throw std::runtime_error("Gripper fault detected before calibration");

    RCLCPP_INFO(get_logger(), "Starting calibration...");

    gripper_->open(0.3, 1.0);
    int status = gripper_->waitForMotionComplete();
    if (status != RobotiqGripper::AT_DEST &&
        status != RobotiqGripper::STOPPED_OUTER_OBJECT)
      throw std::runtime_error("Calibration failed during open phase");
    raw_open_ = gripper_->getCurrentPosition();

    gripper_->close(0.3, 1.0);
    status = gripper_->waitForMotionComplete();
    if (status != RobotiqGripper::AT_DEST &&
        status != RobotiqGripper::STOPPED_INNER_OBJECT)
      throw std::runtime_error("Calibration failed during close phase");
    raw_closed_ = gripper_->getCurrentPosition();

    if (std::abs(raw_open_ - raw_closed_) < 1e-3)
      throw std::runtime_error("Calibration failed: range too small");

    if (raw_open_ >= raw_closed_)
      throw std::runtime_error("Calibration failed: unexpected polarity");

    // Lascia aperto
    gripper_->open(0.3, 1.0);
    gripper_->waitForMotionComplete();

    RCLCPP_INFO(get_logger(),
      "Calibration done → open=%.2f | closed=%.2f | range=%.2f",
      raw_open_, raw_closed_, raw_closed_ - raw_open_);
  }

  void save()
  {
    std::string share_dir =
      ament_index_cpp::get_package_share_directory("robotiq_gripper_driver");
    std::string path = share_dir + "/config/calibration.yaml";

    std::ofstream file(path);
    if (!file.is_open())
      throw std::runtime_error("Cannot write calibration file: " + path);

    file << "raw_open: "   << raw_open_   << "\n";
    file << "raw_closed: " << raw_closed_ << "\n";

    RCLCPP_INFO(get_logger(), "Calibration saved to: %s", path.c_str());
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  std::make_shared<CalibrationNode>();  // shutdown chiamato internamente
  return 0;
}