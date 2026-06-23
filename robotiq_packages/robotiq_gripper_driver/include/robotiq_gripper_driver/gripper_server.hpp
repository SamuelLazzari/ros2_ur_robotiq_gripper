#pragma once

#include <memory>
#include <string>
#include <thread>
#include <cmath>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include <ur_rtde/robotiq_gripper.h>

#include "robotiq_gripper_msgs/action/activate.hpp"
#include "robotiq_gripper_msgs/action/open.hpp"
#include "robotiq_gripper_msgs/action/close.hpp"
#include "robotiq_gripper_msgs/action/move.hpp"
#include "robotiq_gripper_msgs/action/set_force.hpp"
#include "robotiq_gripper_msgs/action/set_speed.hpp"
#include "robotiq_gripper_msgs/action/status.hpp"

#include "sensor_msgs/msg/joint_state.hpp"

using namespace ur_rtde;

class GripperServer : public rclcpp::Node
{
public:
  GripperServer();

  // Esposti pubblicamente per i file execute
  std::unique_ptr<RobotiqGripper> gripper_;
  double raw_open_;
  double raw_closed_;
  double max_pos_;

  double to_mm(double raw);
  double from_mm(double mm);
  std::string get_status_string(int status);

private:
  double min_pos_;
  double min_norm_;
  double max_norm_;
  double position_tolerance_;
  double status_frequency_;

  rclcpp_action::Server<robotiq_gripper_msgs::action::Activate>::SharedPtr  server_activate_;
  rclcpp_action::Server<robotiq_gripper_msgs::action::Open>::SharedPtr      server_open_;
  rclcpp_action::Server<robotiq_gripper_msgs::action::Close>::SharedPtr     server_close_;
  rclcpp_action::Server<robotiq_gripper_msgs::action::Move>::SharedPtr      server_move_;
  rclcpp_action::Server<robotiq_gripper_msgs::action::SetForce>::SharedPtr  server_set_force_;
  rclcpp_action::Server<robotiq_gripper_msgs::action::SetSpeed>::SharedPtr  server_set_speed_;
  rclcpp_action::Server<robotiq_gripper_msgs::action::Status>::SharedPtr    server_status_;

  // necessario per pubblicare lo stato del gripper con topic /joint_states
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_pub_;
  rclcpp::TimerBase::SharedPtr joint_state_timer_;

  // Funzioni
  void publishJointStates();
  double rawToRadians(double raw);

  void calibrate();

  bool load_calibration();

  void publish_joint_state();

  // Template factory: handle_goal, handle_cancel e handle_accepted
  // sono identici per tutti i server. Solo execute cambia.
  template<typename ActionT>
  typename rclcpp_action::Server<ActionT>::SharedPtr create_gripper_server(
    const std::string & action_name,
    std::function<void(std::shared_ptr<rclcpp_action::ServerGoalHandle<ActionT>>)> execute_fn)
  {
    using GoalHandle = rclcpp_action::ServerGoalHandle<ActionT>;

    return rclcpp_action::create_server<ActionT>(
      this,
      action_name,

      // handle_goal — uguale per tutti
      [this](const rclcpp_action::GoalUUID &,
             std::shared_ptr<const typename ActionT::Goal>)
      {
        RCLCPP_INFO(get_logger(), "Goal received");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
      },

      // handle_cancel — uguale per tutti
      [this](std::shared_ptr<GoalHandle>)
      {
        RCLCPP_WARN(get_logger(), "Cancel request received");
        return rclcpp_action::CancelResponse::ACCEPT;
      },

      // handle_accepted — uguale per tutti, lancia execute in thread detached
      [execute_fn](std::shared_ptr<GoalHandle> goal_handle)
      {
        std::thread{[execute_fn, goal_handle]() {
          execute_fn(goal_handle);
        }}.detach();
      }
    );
  }
};
