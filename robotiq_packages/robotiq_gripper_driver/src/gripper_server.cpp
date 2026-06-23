#include "robotiq_gripper_driver/gripper_server.hpp"
#include "robotiq_gripper_driver/execute/execute_activate.hpp"
#include "robotiq_gripper_driver/execute/execute_open.hpp"
#include "robotiq_gripper_driver/execute/execute_close.hpp"
#include "robotiq_gripper_driver/execute/execute_move.hpp"
#include "robotiq_gripper_driver/execute/execute_set_force.hpp"
#include "robotiq_gripper_driver/execute/execute_set_speed.hpp"
#include "robotiq_gripper_driver/execute/execute_status.hpp"

#include <fstream>
#include <ament_index_cpp/get_package_share_directory.hpp>

GripperServer::GripperServer()
: Node("robotiq_gripper_server")
{
  this->declare_parameter<std::string>("robot_ip",          "160.78.27.23");
  this->declare_parameter<int>        ("robot_port",        63352);
  this->declare_parameter<bool>       ("verbose",           true);
  this->declare_parameter<double>     ("max_position",      140.0);
  this->declare_parameter<double>     ("min_position",      0.0);
  this->declare_parameter<double>     ("min_norm",          0.0);
  this->declare_parameter<double>     ("max_norm",          1.0);
  this->declare_parameter<double>     ("status_frequency",  10.0);
  this->declare_parameter<double>     ("position_tolerance",0.5);

  max_pos_            = this->get_parameter("max_position").as_double();
  min_pos_            = this->get_parameter("min_position").as_double();
  min_norm_           = this->get_parameter("min_norm").as_double();
  max_norm_           = this->get_parameter("max_norm").as_double();
  status_frequency_   = this->get_parameter("status_frequency").as_double();
  position_tolerance_ = this->get_parameter("position_tolerance").as_double();

  std::string ip  = this->get_parameter("robot_ip").as_string();
  int         port    = this->get_parameter("robot_port").as_int();
  bool        verbose = this->get_parameter("verbose").as_bool();

  gripper_ = std::make_unique<RobotiqGripper>(ip, port, verbose);

  try {
    gripper_->connect();
    gripper_->setUnit(RobotiqGripper::POSITION, RobotiqGripper::UNIT_DEVICE);
    calibrate();
  } catch (const std::exception & e) {
    RCLCPP_FATAL(get_logger(), "Initialization failed: %s", e.what());
    rclcpp::shutdown();
    return;
  }

  server_activate_  = create_gripper_server<robotiq_gripper_msgs::action::Activate>(
    "gripper/activate",
    [this](auto gh){ execute_activate(gh, *this); });

  server_open_      = create_gripper_server<robotiq_gripper_msgs::action::Open>(
    "gripper/open",
    [this](auto gh){ execute_open(gh, *this); });

  server_close_     = create_gripper_server<robotiq_gripper_msgs::action::Close>(
    "gripper/close",
    [this](auto gh){ execute_close(gh, *this); });

  server_move_      = create_gripper_server<robotiq_gripper_msgs::action::Move>(
    "gripper/move",
    [this](auto gh){ execute_move(gh, *this); });

  server_set_force_ = create_gripper_server<robotiq_gripper_msgs::action::SetForce>(
    "gripper/set_force",
    [this](auto gh){ execute_set_force(gh, *this); });

  server_set_speed_ = create_gripper_server<robotiq_gripper_msgs::action::SetSpeed>(
    "gripper/set_speed",
    [this](auto gh){ execute_set_speed(gh, *this); });

  server_status_    = create_gripper_server<robotiq_gripper_msgs::action::Status>(
    "gripper/status",
    [this](auto gh){ execute_status(gh, *this); });

    joint_state_pub_ = this->create_publisher<sensor_msgs::msg::JointState>(
    "/joint_states", 10);

    joint_state_timer_ = this->create_wall_timer(
        std::chrono::milliseconds(100),  // 10Hz
        std::bind(&GripperServer::publish_joint_state, this));

  RCLCPP_INFO(get_logger(), "Robotiq Gripper Server ready.");
}

/* ===================== CALIBRAZIONE ===================== */
#include <fstream>
#include <ament_index_cpp/get_package_share_directory.hpp>

/* ===================== CALIBRAZIONE ===================== */
void GripperServer::calibrate()
{
  // Prima prova a caricare dal file
  if (load_calibration()) {
    RCLCPP_INFO(get_logger(),
      "Calibration loaded from file → open=%.2f | closed=%.2f",
      raw_open_, raw_closed_);
    return;
  }

  // File non trovato o invalido → calibrazione fisica
  RCLCPP_WARN(get_logger(), "No calibration file found, running physical calibration...");

  if (!gripper_->isActive()) {
    RCLCPP_WARN(get_logger(), "Gripper not active, activating...");
    gripper_->activate();
    if (!gripper_->isActive())
      throw std::runtime_error("Gripper failed to activate");
    RCLCPP_INFO(get_logger(), "Gripper activated successfully");
  }

  if (gripper_->faultStatus() != 0)
    throw std::runtime_error("Gripper fault detected before calibration");

  RCLCPP_INFO(get_logger(), "Starting manual calibration...");

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
    throw std::runtime_error(
      "Calibration failed: unexpected polarity open=" +
      std::to_string(raw_open_) + " closed=" + std::to_string(raw_closed_));

  gripper_->open(0.3, 1.0);
  gripper_->waitForMotionComplete();

  RCLCPP_INFO(get_logger(),
    "Calibration done → open=%.2f | closed=%.2f | range=%.2f raw units",
    raw_open_, raw_closed_, raw_closed_ - raw_open_);
}

/* ===================== LOAD CALIBRAZIONE ===================== */
bool GripperServer::load_calibration()
{
  std::string share_dir =
    ament_index_cpp::get_package_share_directory("robotiq_gripper_driver");
  std::string path = share_dir + "/config/calibration.yaml";

  std::ifstream file(path);
  if (!file.is_open()) return false;

  double open = -1.0, closed = -1.0;
  std::string key;
  double value;

  while (file >> key >> value) {
    if (key == "raw_open:")   open   = value;
    if (key == "raw_closed:") closed = value;
  }

  if (open < 0 || closed < 0 || std::abs(open - closed) < 1e-3) {
    RCLCPP_WARN(get_logger(), "Calibration file invalid or incomplete");
    return false;
  }

  raw_open_   = open;
  raw_closed_ = closed;
  return true;
}

/* ===================== CONVERSIONI ===================== */
double GripperServer::to_mm(double raw)
{
  double mm = (raw - raw_closed_) / (raw_open_ - raw_closed_) * max_pos_;
  return std::max(0.0, std::min(max_pos_, mm));
}

double GripperServer::from_mm(double mm)
{
  return raw_closed_ + (mm / max_pos_) * (raw_open_ - raw_closed_);
}

/* ===================== UTILITY ===================== */
std::string GripperServer::get_status_string(int status)
{
  switch (status) {
    case RobotiqGripper::MOVING:               return "MOVING";
    case RobotiqGripper::STOPPED_OUTER_OBJECT: return "STOPPED_OUTER_OBJECT";
    case RobotiqGripper::STOPPED_INNER_OBJECT: return "STOPPED_INNER_OBJECT";
    case RobotiqGripper::AT_DEST:              return "AT_DEST";
    default:                                   return "UNKNOWN";
  }
}

double GripperServer::rawToRadians(double raw)
{
  constexpr double RAD_OPEN   = -0.07;
  constexpr double RAD_CLOSED =  0.7;

  double t = (raw - raw_open_) / (raw_closed_ - raw_open_);
  t = std::clamp(t, 0.0, 1.0);
  return RAD_OPEN + t * (RAD_CLOSED - RAD_OPEN);
}

void GripperServer::publish_joint_state()
{
  double raw = gripper_->getCurrentPosition();
  double finger_pos = rawToRadians(raw);

  auto msg = sensor_msgs::msg::JointState();
  msg.header.stamp = this->get_clock()->now();
  msg.name = {
    "finger_joint",
    "left_inner_knuckle_joint",
    "left_inner_finger_joint",
    "right_outer_knuckle_joint",
    "right_inner_knuckle_joint",
    "right_inner_finger_joint"
  };
  msg.position = {
     finger_pos,
    -finger_pos,
     finger_pos,
    -finger_pos,
    -finger_pos,
     finger_pos
  };

  joint_state_pub_->publish(msg);
}
