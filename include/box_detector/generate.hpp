/**
 * BSD 3-Clause LICENSE
 *
 * Copyright (c) 2019, Anirudh Topiwala
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without  
 * modification, are permitted provided that the following conditions are 
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the   
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its 
 * contributors may be used to endorse or promote products derived from this 
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
 * CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 *  @file    generate.hpp
 *  @author  Anirudh Topiwala
 *  @copyright BSD License
 *
 *  @brief Definitions for the class walker
 *
 *  @section DESCRIPTION
 *
 *  Defintiions for the class Generate. Also declaring the class Generate.
 */

#ifndef INCLUDE_GENERATE_HPP_
#define INCLUDE_GENERATE_HPP_

#include "ros/ros.h"
#include <stdio.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <tf/transform_broadcaster.h> 

typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;

class Generate {
 public:
   /**
   *   @brief  Constructor of class Walker
   *
   *   @param  none
   *   @return none
   */
  Generate();

   /**
   *   @brief  Destructor of class Walker
   *
   *   @param  none
   *   @return none
   */
  ~Generate();

   /**
   *   @brief  sensor callback function to return value and check if
   *   the obstacle is within range by subscribing to /scan topic.
   *
   *   @param  message type sensor_msgs::LaserScan
   *   @return none
   */
  void generate_plane();

   /**
   *   @brief  returns the value of obstacle
   *
   *   @param  none
   *   @return true if obstacle is within range, otherwise false
   */

  void publisher_Callback(const ros::TimerEvent&);

 private:
    // Initialiazation of Variables
    PointCloud::Ptr msg_plane;

    // ROS-related variables for time-specific use
    ros::NodeHandle n;
    ros::Publisher pub;
    ros::Timer plane_pub_timer;

};

#endif  // INCLUDE_GENERATE_HPP_