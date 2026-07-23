from launch import LaunchDescription
from launch_ros.actions import Node

#### CHANGE HERE #### 
robot_ip = ""
robot_port = 63352
verbose = True
#####################

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='robotiq_gripper_driver',
            executable='robotiq_calibration_node',
            name='robotiq_calibration_node',
            parameters=[{
                'robot_ip':   robot_ip,
                'robot_port': robot_port,
                'verbose':    verbose,
            }],
            output='screen',
        )
    ])