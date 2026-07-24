from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
from launch.substitutions import Command
from launch.conditions import IfCondition
from ament_index_python.packages import get_package_share_directory
import os

#### CHANGE HERE #### 
robot_ip = "160.78.27.23"
robot_port = 63352
verbose = True
launch_rviz= False
launch_robot_state_publisher= False
#####################

def generate_launch_description():
    custom_desc_share = get_package_share_directory('custom_robotiq_description')

    robot_description_content = Command([
        'xacro ',
        os.path.join(custom_desc_share, 'urdf', 'robotiq_2f_140_gripper.urdf.xacro')
    ])

    nodes = [
        Node(
            package='robotiq_gripper_driver',
            executable='robotiq_gripper_server',
            name='robotiq_gripper_server',
            parameters=[{
                'robot_ip': robot_ip,
                'robot_port': robot_port,
                'verbose': verbose,
            }]
        ),
    ]

    if launch_robot_state_publisher:
        nodes.append(
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
                    )
        )

    if launch_rviz:
        nodes.append(
            ExecuteProcess(
                cmd=['rviz2'],
                output='screen'
            )
        )

    return LaunchDescription(nodes)