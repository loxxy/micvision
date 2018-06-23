/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2018, Micvision, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/


#ifndef MICVISION_EXPLORATION_H_
#define MICVISION_EXPLORATION_H_
#include <ros/ros.h>
#include <std_srvs/Trigger.h>
#include <sensor_msgs/LaserScan.h>
#include <tf/transform_listener.h>
#include <actionlib/server/simple_action_server.h>
#include <actionlib/client/simple_action_client.h>
#include <pluginlib/class_loader.h>

#include <micvision/ExplorationAction.h>
#include <micvision/grid_map.h>
#include <micvision/commands.h>

#include <move_base_msgs/MoveBaseAction.h>

#include <Eigen/Core>

#include <vector>
#include <string>

namespace micvision {

using Action = micvision::ExplorationAction;
using Server = actionlib::SimpleActionServer<Action>;
using Client = actionlib::SimpleActionClient<Action>;

class MicvisionExploration {
 public:
  MicvisionExploration();
  ~MicvisionExploration();

 private:
  bool receiveStop(std_srvs::Trigger::Request &req,
                   std_srvs::Trigger::Response &res);
  bool receiveStopExploration(std_srvs::Trigger::Request &req,
                              std_srvs::Trigger::Response &res);
  bool receivePause(std_srvs::Trigger::Request &req,
                    std_srvs::Trigger::Response &res);
  void receiveExplorationGoal(
      const micvision::ExplorationGoal::ConstPtr &goal);
  void mapCallback(const nav_msgs::OccupancyGrid& global_map);
  void scanCallback(const sensor_msgs::LaserScan& scan);
  void externGoalCallback(const geometry_msgs::PoseStamped&);
  Pixel world2pixel(const Point& point) const;
  Point pixel2world(const Pixel& pixel) const;

  bool setCurrentPosition();
  void stop();

  // start pixel is [0, 0]
  std::vector<Pixel> bresenham(const Pixel& end);
  void findBestDirection();
  // int safeIndex(int index) const;

 private:
  // Everything related to ROS
  tf::TransformListener tf_listener_;
  ros::ServiceServer stop_server_;
  ros::ServiceServer pause_server_;
  ros::ServiceServer stop_exploration_server_;

  std::string map_frame_;
  std::string robot_frame_;
  std::string explore_action_topic_;

  Server* exploration_action_server_;

  // Current status and goals
  bool is_paused_ = false;
  bool is_stopped_ = false;
  bool exploration_running_ = false;
  unsigned int start_index_ = -1;
  double update_frequency_;

  double angles_ = -2*M_PI;

  // Everything related to the global map and plan
  GridMap current_map_;

  ros::Publisher goal_publisher_;
  ros::Subscriber map_sub_;
  ros::Subscriber scan_sub_;
  ros::Subscriber extern_goal_sub_;
  ros::Publisher stop_publisher_;

  Pixel robot_pixel_ = Pixel(0, 0);
  Point robot_point_;
  double robot_direction_ = 0;
  // int directon_index_ = 0;
  Pixel goal_pixel_ = Pixel(-1, -1);
  Point goal_point_ = Point(-100.0f, -100.0f);

  std::vector<int> indices_;

  // scan relative
  double angle_min_ = -M_PI,
         angle_max_ =  M_PI,
         angle_increment_ = M_PI / 180.0f;
  int scan_size_ = 0;
};
}  // end namespace micvision
#endif  // end MICVISION_EXPLORATION_H_
