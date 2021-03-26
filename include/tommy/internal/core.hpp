#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else

#endif


#include <vulkan/vulkan.hpp>
//#include <vulkan/vk_mem_alloc.h>
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


    //
    enum class ImageViewType : uint32_t {
        eSampler = 0,
        eTexture1d = 1,
        eTexture2d = 2,
        eTexture3d = 3,
        eImage1d = 4,
        eImage2d = 5,
        eImage3d = 6,
        eTextureSampler1d = 7,
        eTextureSampler2d = 8,
        eTextureSampler3d = 9,
    };


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
    enum class MemoryUsage : uint32_t {
        eUnknown = 0u,
        eGPUOnly = 1u,
        eCPUOnly = 2u,
        eCPUtoGPU = 3u,
        eGPUtoCPU = 4u,
        eCPUCopy = 5u,
        eGPULazilyAllocated = 6u
    };

    // 
    struct MemoryAllocationInfo {
        uint32_t sType = 0xFFA00001;
        void* pNext = nullptr;
        MemoryUsage usage = MemoryUsage::eGPUOnly;
        vk::Buffer buffer = {};
        vk::Image image = {};

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


    //
    struct ImageViewKey {
        ImageViewType type = ImageViewType::eSampler;
        uint32_t index = 0u;

        // 
        operator uint64_t&() { return reinterpret_cast<uint64_t&>(*this); };
        operator const uint64_t&() const { return reinterpret_cast<const uint64_t&>(*this); };
        uint64_t& operator=(const uint64_t& a) { return (reinterpret_cast<uint64_t&>(*this) = a); };
    };

};
