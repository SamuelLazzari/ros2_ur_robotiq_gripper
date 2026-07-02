from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='robotiq_gripper_driver',
            executable='robotiq_calibration_node',
            name='robotiq_calibration_node',
            parameters=[{
                'robot_ip':   '160.78.27.23',
                'robot_port': 63352,
                'verbose':    True,
            }],
            output='screen',
        )
    ])