#pragma once

#include "third_party/glm/glm.hpp"
#include "third_party/volk/volk.h"
// #include <vulkan/vulkan.h>

#include <windows.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <filesystem>
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
#include <tuple>

#include <glslang/Include/glslang_c_interface.h>

namespace fs = std::filesystem;

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

// CppCon 2015: Andrei Alexandrescu “Declarative Control Flow" https://www.youtube.com/watch?v=WjTrfoiB0MQ
#ifndef ANONYMOUS_VARIABLE
#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)
#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __COUNTER__)
# else
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
#endif
#endif

enum class Scope_Guard_On_Exit
{
};

template <typename Func>
class Scope_Guard
{
public:
    explicit Scope_Guard(T&& function) : function(std ::move(function))
    {
    }

    ~Scope_Guard()
    {
        function();
    }

private:
    Func function;
};

template<typename Func>
Scope_Guard<Func> operator+(ScopeGuardOnExit, Func&& function)
{
    return ScopeGuard<T>(std::forward<T>(function));
}

#define SCOPE_EXIT auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) = Scope_Guard_On_Exit() + [&]() noexcept
