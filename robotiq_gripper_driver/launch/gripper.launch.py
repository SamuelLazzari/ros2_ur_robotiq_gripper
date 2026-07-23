from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
from launch.substitutions import Command
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    # Definiamo i percorsi
    custom_desc_share = get_package_share_directory('custom_robotiq_description')

    # Comando per processare il file Xacro
    robot_description_content = Command([
        'xacro ',
        os.path.join(custom_desc_share, 'urdf', 'robotiq_2f_140_gripper.urdf.xacro')
    ])

    return LaunchDescription([
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{
                'robot_description': Command([
                    'xacro ',
                    os.path.join(
                        get_package_share_directory('custom_robotiq_description'),
                        'urdf',
                        'robotiq_2f_140_gripper.urdf.xacro'
                    )
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