#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_mem_alloc.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <map>
#include <future>
#include <iostream>

// 
namespace tom {

    class Device;
    class Instance;
    class Buffer;
    class Queue;

    class ImageView;
    class DeviceImage;
    class DeviceBuffer;
    class DeviceMemory;
    class MemoryAllocation;
    class PhysicalDevice;
    class MemoryAllocator;

    class MemoryAllocatorVma;



    // 
    class DescriptorSetSource: public std::enable_shared_from_this<DescriptorSetSource> { public:
        std::vector<vk::DescriptorBufferInfo> buffers = {};
        std::vector<vk::DescriptorImageInfo> textures2d = {};
    };

    // 
    class DescriptorSetLayouts: public std::enable_shared_from_this<DescriptorSetLayouts> { public:
        vk::DescriptorSetLayout buffers = {};
        vk::DescriptorSetLayout textures = {};
        vk::DescriptorSetLayout images = {};
    };

    // 
    class DescriptorSet: public std::enable_shared_from_this<DescriptorSet> { public:
        vk::DescriptorSet buffers = {};
        vk::DescriptorSet textures = {};
        vk::DescriptorSet images = {};
    };



    //
    using PhysicalDeviceFeaturesChain = vk::StructureChain<vk::PhysicalDeviceFeatures2&, vk::PhysicalDeviceBufferDeviceAddressFeatures&>;
    using PhysicalDevicePropertiesChain = vk::StructureChain<vk::PhysicalDeviceProperties2&>;

    //
    struct PhysicalDeviceFeatures {
        vk::PhysicalDeviceFeatures2 features = {};
        vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
    };

    //
    struct PhysicalDeviceProperties {
        vk::PhysicalDeviceProperties2 properties = {};
    };

    //
    struct SurfaceProperties {
        vk::Bool32 supported = false;
        vk::SurfaceCapabilities2KHR capabilities = {};
        std::vector<vk::PresentModeKHR> presentModes = {};
        std::vector<vk::SurfaceFormat2KHR> formats = {};
    };

};
