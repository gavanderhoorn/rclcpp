// Copyright 2017 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "rclcpp/node.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp/time_source.hpp"


#include <utility>

#include "builtin_interfaces/msg/time.hpp"

#include "rcl/time.h"

#include "rclcpp/exceptions.hpp"

#include "rcutils/logging_macros.h"

namespace rclcpp
{

Time
TimeSource::now(rcl_time_source_type_t clock)
{
  // TODO(karsten1987): This impl throws explicitely on RCL_ROS_TIME
  // we have to do this, because rcl_time_source_init returns RCL_RET_OK
  // for RCL_ROS_TIME, however defaults to system time under the hood.
  // ref: https://github.com/ros2/rcl/blob/master/rcl/src/rcl/time.c#L66-L77
  // This section can be removed when rcl supports ROS_TIME correctly.
  // if (clock == RCL_ROS_TIME) {
  //   throw std::runtime_error("RCL_ROS_TIME is currently not implemented.");
  // }
  // 
  Time now(0, 0, clock);
  // 
  // auto ret = rcl_time_point_get_now(&now.rcl_time_);
  // if (ret != RCL_RET_OK) {
  //   rclcpp::exceptions::throw_from_rcl_error(
  //     ret, "could not get current time stamp");
  // }

  return now;
}

TimeSource::TimeSource(std::shared_ptr<rclcpp::node::Node> node)
: node_(node)
{
  // TODO(tfoote): Update QOS
  clock_subscription_ = node_->create_subscription<builtin_interfaces::msg::Time>(
    "clock", std::bind(&TimeSource::clock_cb, this, std::placeholders::_1), rmw_qos_profile_default);
    
  auto ret1 = rcl_time_source_init(RCL_ROS_TIME, &ros_time_source_);
  if (ret1 != RCL_RET_OK) {
    RCUTILS_LOG_ERROR("Failed to initialize ROS time source");
  }
  auto ret2 = rcl_time_source_init(RCL_ROS_TIME, &system_time_source_);
  if (ret2 != RCL_RET_OK) {
    RCUTILS_LOG_ERROR("Failed to initialize SYSTEM time source");
  }  
}

TimeSource::~TimeSource()
{
  auto ret1 = rcl_time_source_fini(&ros_time_source_);
  if (ret1 != RCL_RET_OK) {
    RCUTILS_LOG_ERROR("Failed to fini ROS time source");
  }
  auto ret2 = rcl_time_source_fini(&system_time_source_);
  if (ret2 != RCL_RET_OK) {
    RCUTILS_LOG_ERROR("Failed to fini SYSTEM time source");
  }
}

void TimeSource::clock_cb(const builtin_interfaces::msg::Time::SharedPtr msg)
{
  RCUTILS_LOG_INFO("Got clock message");
  
  bool is_enabled;
  auto ret = rcl_is_enabled_ros_time_override(&ros_time_source_, &is_enabled);
  if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR("Failed to check ros_time_override_status");
  }
  if (!is_enabled)
  {
   ret = rcl_enable_ros_time_override(&ros_time_source_);
   if (ret != RCL_RET_OK) {
       RCUTILS_LOG_ERROR("Failed to enable ros_time_override_status");
   }
  }
  //TODO(tfoote) switch this to be based on if there are clock publishers
  //TODO(tfoote) also setup disable
  
  rcl_time_point_t clock_time;
  ret = rcl_time_point_init(&clock_time, &ros_time_source_);
  if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR("Failed to init ros_time_point");
  }
  clock_time.nanoseconds = msg->sec * 1e9 + msg->nanosec;
  ret = rcl_set_ros_time_override(&ros_time_source_, clock_time.nanoseconds);
  if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR("Failed to set ros_time_override_status");
  }
}


}  // namespace rclcpp
