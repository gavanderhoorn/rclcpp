// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#ifndef RCLCPP_LIFECYCLE__VISIBILITY_CONTROL_H_
#define RCLCPP_LIFECYCLE__VISIBILITY_CONTROL_H_

#ifdef _WIN32
  #define shared_EXPORTS 1
  #ifdef shared_EXPORTS
    #define LIFECYCLE_EXPORT __declspec(dllexport)
  #else
    #define LIFECYCLE_EXPORT __declspec(dllimport)
  #endif
#else
  #define LIFECYCLE_EXPORT
#endif

#endif  // RCLCPP_LIFECYCLE__VISIBILITY_CONTROL_H_