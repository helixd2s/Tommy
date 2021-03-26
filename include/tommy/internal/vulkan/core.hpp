#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else

#endif

#include "../core.hpp"
#include <vulkan/vulkan.hpp>
//#include <vulkan/vk_mem_alloc.h>


// 
namespace tom {

    // 
    namespace vulkan {
        class Device;
        class Instance;
        class BufferAllocation;
        class Queue;
        class ImageView;
        class DeviceImage;
        class DeviceBuffer;
        class DeviceMemory;
        class MemoryAllocation;
        class PhysicalDevice;
        class MemoryAllocator;
        class MemoryAllocatorVma;
    };

    // 
    namespace vulkan {
        // 
        class DescriptorSetSource: public std::enable_shared_from_this<DescriptorSetSource> { public: 
            std::vector<std::vector<std::shared_ptr<ImageView>>> typedImageViews = {};
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
        struct MemoryAllocationInfo {
            
            vk::Buffer buffer = {};
            vk::Image image = {};
            vk::MemoryRequirements2 requirements = {};

            // 
            MemoryAllocationInfo withBuffer(const vk::Buffer& buffer = {}) {
                auto info = MemoryAllocationInfo(*this);
                info.buffer = buffer;
                return info;
            };

            // 
            MemoryAllocationInfo withImage(const vk::Image& image = {}) {
                auto info = MemoryAllocationInfo(*this);
                info.image = image;
                return info;
            };
        };

    };

    // 
    namespace vulkan {
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

    // 
    MemoryAllocationInfo MemoryAllocationInfo::withVulkan(const vulkan::MemoryAllocationInfo& vinfo = {}) {
        auto info = MemoryAllocationInfo(*this);
        info.apiInfo = (void*)&vinfo;
        return info;
    };

};
