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
 *  @file    main.cpp
 *  @author  Anirudh Topiwala
 *  @copyright BSD License
 *             
 *  @brief Implementing Plane and Box Point cloud generation
 *
 *  @section DESCRIPTION
 *
 *  This program is used to synthesize plane point cloud with a 
 *  resolution of 0.1 m. The random box generation is also implemented and then
 *  both the point clouds are merged.
 *  
 */
#include <iostream>
#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_ros/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/point_types.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <math.h>
using namespace std;


#define PI 3.14159265;

typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;

bool dbeq(double a , double b, double eps= 0.1){
    return (std::abs(a-b)< eps);
}

// bool dist(double x1 , double y1, double x2 , double y2, double eps= 0.01){
// 	double distance = sqrt( pow((x1-x2),2) + pow((y1-y2),2));
// 	if (distance<1)
// 	ROS_ERROR_STREAM("Dist"<< distance);
//     return (std::abs(distance)< eps);
// }

class Generate {

private:

    PointCloud msg_plane;
    PointCloud msg_box;
    PointCloud merged;
    ros::NodeHandle n;
    ros::Publisher pub;
    ros::Timer plane_pub_timer;
    ros::Timer box_pub_timer;
    
    // Random number generator
    std::random_device rand_dev;
    std::mt19937  generator;
    std::uniform_real_distribution<> xy_rnd; 
    std::uniform_real_distribution<> yaw_rnd;
    std::uniform_real_distribution<> noise_rnd;

    /**
    * @brief      Here I generate a plane of points with a resolution of 0.1 m
    *
    * @param      None: It makes the plane as given by the constraints in the 
                        question.
    *
    * @return     void   pushes the cloud to the private variable of class.
    */
    void generate_plane() {
        msg_plane.header.frame_id = "world";
        pcl_conversions::toPCL(ros::Time::now(), msg_plane.header.stamp);
        for (double i = -5.0; i <= 5.0; i+=0.1) {
            for (double j = -5.0; j <= 5.0; j+=0.1) {
                pcl::PointXYZ point;
                point.x = i;
                point.y = j;
                point.z = 0.0;
                msg_plane.push_back(point);
            }
        }
    }

    /**
    * @brief      Here I generate a cube box with random position and 
                  Orientation.
    *
    * @param      x    The random x position of center of cube.
    * @param      y    The random y position of center of cube.
    * @param      yaw    The random yaw angle of the cube.
    *
    * @return     void   pushes the cloud to the private variable of class.
    */
    void generate_box(double x,double y, double yaw ) {
        // msg_box.header.frame_id = "map";
        // x=0;
        // y=0;
        // yaw=10;
        ROS_INFO_STREAM("Angle given: " << yaw* 180 / PI;);
        pcl_conversions::toPCL(ros::Time::now(), msg_box.header.stamp);
        for (double k = 0.0; k <=1; k+=0.1) {
            for (double i = x+0; i <=x+1 ; i+=0.1) {
                for (double j = y+0; j <=y+1; j+=0.1) {
                    if (dbeq(k,0) || dbeq(k,1)){
                        pcl::PointXYZ point;
                        point.x = x + (i-x)*cos(yaw) - (j-y)*sin(yaw);  
                        point.y = y + (i-x)*sin(yaw) + (j-y)*cos(yaw);
                        point.z = k;
                        msg_box.push_back(point);
                    }
                    else{
                         if (dbeq(i,x+0) || dbeq(i,x+1)){
                            pcl::PointXYZ point1;
                            point1.x = x + (i-x)*cos(yaw) - (j-y)*sin(yaw);  
                            point1.y = y + (i-x)*sin(yaw) + (j-y)*cos(yaw);
                            point1.z = k;
                            msg_box.push_back(point1);
                        }
                        else{
                            if (dbeq(j,y+0) || dbeq(j,y+1)){
                                pcl::PointXYZ point2;
                                point2.x = x + (i-x)*cos(yaw) - (j-y)*sin(yaw);  
                                point2.y = y + (i-x)*sin(yaw) + (j-y)*cos(yaw);
                                point2.z = k;
                                msg_box.push_back(point2);
                            } 
                        }
                    }
                }
            }
        }
    }

    /**
    * @brief      Merging the plane and box point cloud
    *
    * @param      None  Both the clouds are  private variables of the class.
    *
    * @return     void  the merged variable consisting of merged point cloud 
                  is set here.
    */
    void merging_both(){
    	// Removing Last Layer of Box
    	PointCloud layer1;
  		PointCloud::Ptr box_unfilt(new pcl::PointCloud<pcl::PointXYZ>);
  		*box_unfilt = msg_box;
    	pcl::PassThrough<pcl::PointXYZ> pass;
		pass.setInputCloud (box_unfilt);
		pass.setFilterFieldName ("z");
		pass.setFilterLimits (0.1, 1.0);
		pass.filter (msg_box);
		pass.setFilterLimitsNegative (true);
		pass.filter (layer1);


		// Subtarcting last layer of box from plane

		// Creating KdTree
		pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
		// Setting cloud for plane
		PointCloud::Ptr plane(new pcl::PointCloud<pcl::PointXYZ>);
		*plane = msg_plane; 
		kdtree.setInputCloud (plane);
		
		// Creating variables for kd tree
		std::vector<int> pointIdxRadiusSearch;
  		std::vector<float> pointRadiusSquaredDistance;
		float radius = 0.09;

		// Creating Inliers for filter		
		pcl::PointIndices::Ptr inliers(new pcl::PointIndices());
		pcl::ExtractIndices<pcl::PointXYZ> extract;

		for (int i = 0; i < layer1.points.size(); i++) {
			if (kdtree.radiusSearch (layer1.points[i], radius, pointIdxRadiusSearch, pointRadiusSquaredDistance) > 0 )
  				{
  					for (size_t j = 0; j < pointIdxRadiusSearch.size (); ++j){
  							inliers->indices.push_back(pointIdxRadiusSearch[j]);
  					}
			}
		}	
		extract.setInputCloud(plane);
		extract.setIndices(inliers);
		extract.setNegative(true);
		extract.filter(*plane);
		merged = msg_box + *plane ;

    }

    /**
    * @brief      Generating Noise and Adding to merged point cloud
    *
    * @param      None gets the point cloud from the private variables 
                  of the class.
    *
    * @return     PointCloud  return the cloud after adding noise to it.
    */
    PointCloud make_noise() {

            PointCloud merged_noise;
            for (int i = 0; i < merged.points.size(); i++) {
                pcl::PointXYZ point;
                point.x = merged.points[i].x + noise_rnd(generator);
                point.y = merged.points[i].y + noise_rnd(generator);
                point.z = merged.points[i].z + noise_rnd(generator);
                merged_noise.push_back(point);
            }
            return merged_noise;

    }

    /**
    * @brief      Callback with frequency of 5 hz for noise generation. 
    *
    * @param      setting the frequency to execute the callback.
    *
    * @return     void   publishing the cloud to topic
    */
    void publisher_Callback(const ros::TimerEvent&) {
        // pub.publish(msg_plane);
        auto merged_noise = make_noise();
        merged_noise.header.frame_id = "world";
        pub.publish(merged_noise);
        // ROS_INFO_STREAM("Publishing");
    }

    /**
    * @brief      Callback for generating random box and merging them every
                  second.This callback is executed every second as a new box is 
                  only generated every second. 
    *
    * @param      setting the frequency to execute the callback.
    *
    * @return     void   
    */
    void box_Callback(const ros::TimerEvent&) {
        double x = xy_rnd(generator);
        double y = xy_rnd(generator);
        double yaw = yaw_rnd(generator);
        generate_box(x,y,yaw);
        // pub.publish(msg_box);
        merging_both();
        msg_box.clear();
       ROS_INFO_STREAM("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    }


public:
    Generate(): xy_rnd(-2.0, 2.0),yaw_rnd(0.0, 3.1415/2),noise_rnd(-0.0002, 
        0.0002),generator(rand_dev()) {
        
        pub = n.advertise<PointCloud>("/cloud",1000);
        plane_pub_timer = n.createTimer(ros::Duration(0.2),
         &Generate::publisher_Callback, this);
        box_pub_timer = n.createTimer(ros::Duration(1),
         &Generate::box_Callback, this);
        generate_plane();
    }

};

int main(int argc, char** argv) {

    ros::init (argc, argv, "plt");
    Generate gen;
    ros::spin();
}