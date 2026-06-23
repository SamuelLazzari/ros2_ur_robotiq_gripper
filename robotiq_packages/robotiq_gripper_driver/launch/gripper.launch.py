from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
from launch.substitutions import Command

def generate_launch_description():
    return LaunchDescription([

        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{
                'robot_description': Command([
                    'xacro ',
                    '/home/samuel/ros_ws/src/robotiq_description/urdf/robotiq_2f_140_gripper.urdf.xacro'
                ])
            }]
        ),

        Node(
            package='robotiq_gripper_driver',
            executable='robotiq_gripper_server',
            name='robotiq_gripper_server',
            parameters=[{
                'robot_ip': '160.78.27.23',
                'robot_port': 63352,
                'verbose': True,
            }]
        ),

        ExecuteProcess(
            cmd=['rviz2'],
            output='screen'
        ),

    ])