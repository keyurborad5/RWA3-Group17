#pragma once

#include <cmath>
#include <tf2_ros/static_transform_broadcaster.h>
#include <utils.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/twist.hpp>
// for static broadcaster
#include "tf2_ros/static_transform_broadcaster.h"
// for dynamic broadcaster
#include "tf2_ros/transform_broadcaster.h"
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <ros2_aruco_interfaces/msg/aruco_markers.hpp>
#include <mage_msgs/msg/advanced_logical_camera_image.hpp>
using namespace std::chrono_literals;

class MyRobotNode : public rclcpp::Node
{
public:
    MyRobotNode(std::string node_name) : Node(node_name)
    {
        // parameter to decide whether to execute the broadcaster or not
        RCLCPP_INFO(this->get_logger(), "Broadcaster demo started");

        // initialize the transform broadcaster
        aruco_tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);

        // Load a buffer of transforms
        aruco_tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
        aruco_tf_buffer_->setUsingDedicatedThread(true);
        // Create a utils object to use the utility functions
        utils_ptr_ = std::make_shared<Utils>();

        
        //************************Listener******************************
        RCLCPP_INFO(this->get_logger(), "Listener demo started");

        // load a buffer of transforms
        aruco_tf_listener_buffer_ =std::make_unique<tf2_ros::Buffer>(this->get_clock());
        aruco_tf_listener_buffer_->setUsingDedicatedThread(true);
        aruco_transform_listener_ =std::make_shared<tf2_ros::TransformListener>(*aruco_tf_listener_buffer_);


        //********************Subscriber**************************
        aruco_cam_subscriber_=this->create_subscription<ros2_aruco_interfaces::msg::ArucoMarkers>("aruco_markers",rclcpp::SensorDataQoS(), std::bind(&MyRobotNode::aruco_cam_sub_cb,this,std::placeholders::_1));
        // part_cam_subscriber_=this->create_subscription<mage_msgs::msg::AdvancedLogicalCameraImage>("mage/advanced_logical_camera/image",rclcpp::SensorDataQoS(), std::bind(&MyRobotNode::part_cam_sub_cb,this,std::placeholders::_1));

        //********************Publisher*****************************
        cmd_val_publisher_=this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel",10);
        cmd_val_timer_ = this->create_wall_timer(std::chrono::milliseconds(100),std::bind(&MyRobotNode::cmd_val_pub_cb,this));


    }


private:
    /*!< Boolean parameter to whether or not start the broadcaster */
    bool param_broadcast_;
    /*!< Buffer that stores several seconds of transforms for easy lookup by the listener. */
    std::shared_ptr<tf2_ros::Buffer> aruco_tf_buffer_;
    /*!< MyRobotNode object */
    std::shared_ptr<tf2_ros::TransformBroadcaster> aruco_tf_broadcaster_;
    /*!< Buffer that stores several seconds of transforms for easy lookup by the listener. */
    std::shared_ptr<tf2_ros::Buffer> part_tf_buffer_;
    /*!< MyRobotNode object */
    std::shared_ptr<tf2_ros::TransformBroadcaster> part_tf_broadcaster_;
    /*!< Utils object to access utility functions*/
    std::shared_ptr<Utils> utils_ptr_;
    int part_color_;
    int part_type_;
    
    

    /**
     * @brief Timer to broadcast the transform
     *
     */
    void aruco_broadcaster(ros2_aruco_interfaces::msg::ArucoMarkers::SharedPtr);

    /**
     * @brief Timer to broadcast the transform
     *
     */
    void part_broadcaster(mage_msgs::msg::AdvancedLogicalCameraImage::SharedPtr);
    

    //******************Listener***********************

     /*!< Boolean variable to store the value of the parameter "listen" */
    bool param_listen_;
    /*!< Buffer that stores several seconds of transforms for easy lookup by the listener. */
    std::unique_ptr<tf2_ros::Buffer> aruco_tf_listener_buffer_;
    /*!< Transform listener object */
    std::shared_ptr<tf2_ros::TransformListener> aruco_transform_listener_{nullptr};
    
    /**
     * @brief Listen to a aruco transform
     *
     * @param source_frame Source frame (child frame) of the transform
     * @param target_frame Target frame (parent frame) of the transform
     */
    void aruco_listen_transform(const std::string &source_frame, const std::string &target_frame);

    /*!< Buffer that stores several seconds of transforms for easy lookup by the listener. */
    std::unique_ptr<tf2_ros::Buffer> part_tf_listener_buffer_;
    /*!< Transform listener object */
    std::shared_ptr<tf2_ros::TransformListener> part_transform_listener_{nullptr};
    
    /**
     * @brief Listen to a part transform
     *
     * @param source_frame Source frame (child frame) of the transform
     * @param target_frame Target frame (parent frame) of the transform
     */
    void part_listen_transform(const std::string &source_frame, const std::string &target_frame);


    //***********Subscriber**********************
    rclcpp::Subscription<ros2_aruco_interfaces::msg::ArucoMarkers>::SharedPtr aruco_cam_subscriber_;
    void aruco_cam_sub_cb(const ros2_aruco_interfaces::msg::ArucoMarkers::SharedPtr msg);

    rclcpp::Subscription<mage_msgs::msg::AdvancedLogicalCameraImage>::SharedPtr part_cam_subscriber_;
    void part_cam_sub_cb(const mage_msgs::msg::AdvancedLogicalCameraImage::SharedPtr msg);

    //*******************Publisher**********************
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_val_publisher_;
    rclcpp::TimerBase::SharedPtr cmd_val_timer_;
    void cmd_val_pub_cb();


   
};
