#pragma once
#include <stdio.h>
#include <string>

#define AIR_NAMESPACE_BEGIN namespace air {
#define AIR_NAMESPACE_END }

#define AIR_LOG(f) printf((std::string("[AIR]: ") + f + "\n").c_str());
#define AIR_LOG(f, ...) printf((std::string("[AIR]: ") + f + "\n").c_str(), __VA_ARGS__);