#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_mem_alloc.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <map>
#include <future>

// 
namespace tom {

    class Device;
    class Instance;
    class Buffer;
    class Queue;

    class DeviceBuffer;
    class DeviceMemory;
    class MemoryAllocation;
    class PhysicalDevice;

};
