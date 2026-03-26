#pragma once

#include "third_party/glm/glm.hpp"
#include <vulkan/vulkan.h>

#include <windows.h>
#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using i16 = int16_t;
using i32 = int32_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using uc = unsigned char;

#define FRAMES_IN_FLIGHT 3

#define VK_CHECK(expr)                                           \
    do {                                                         \
        if ((expr) != VK_SUCCESS)                                \
        {                                                        \
            std::stringstream ss;                                \
            ss << __FILE__ << "(" << __LINE__ << "): " << #expr; \
            std::string message = ss.str();                      \
            throw std::runtime_error(message);                   \
        }                                                        \
    } while (0)

struct Window_Description
{
    std::string name;
    u32 width;
    u32 height;
    HWND handle;
};

Window_Description window_description;
