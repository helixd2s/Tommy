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

    // 
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

    // 
    namespace vktm {
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
    namespace d12tm {
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
    enum class MemoryUsage : uint32_t {
        eUnknown = 0u,
        eGPUOnly = 1u,
        eCPUOnly = 2u,
        eCPUtoGPU = 3u,
        eGPUtoCPU = 4u,
        eCPUCopy = 5u,
        eGPULazilyAllocated = 6u
    };

    namespace d12tm {
        struct MemoryAllocationInfo {
            
        };
    };

    // 
    namespace vktm {
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
    struct MemoryAllocationInfo {
        MemoryUsage usage = MemoryUsage::eGPUOnly;
        //vktm::MemoryAllocationInfo* vkInfo = nullptr;
        //d12tm::MemoryAllocationInfo* d12Info = nullptr;
        void* apiInfo = nullptr;

        // 
        MemoryAllocationInfo withVulkan(const vktm::MemoryAllocationInfo& vinfo = {}) {
            auto info = MemoryAllocationInfo(*this);
            info.apiInfo = (void*)&vinfo;
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






    // 
    class QueueBase: public std::enable_shared_from_this<QueueBase> {
        public:
    };

    // 
    class DeviceBase: public std::enable_shared_from_this<DeviceBase> {
        public:
    };

    // 
    class PhysicalDeviceBase: public std::enable_shared_from_this<PhysicalDeviceBase> {
        public:
    };

    // 
    class InstanceBase: public std::enable_shared_from_this<InstanceBase> {
        public:
    };


    // 
    class MemoryAllocationBase: public std::enable_shared_from_this<MemoryAllocationBase> {
        public:
    };

    // 
    class DeviceImageBase: public std::enable_shared_from_this<DeviceImageBase> {
        public:
    };

    // 
    class DeviceBufferBase: public std::enable_shared_from_this<DeviceBufferBase> {
        public:
    };

    // 
    class DeviceMemoryBase: public std::enable_shared_from_this<DeviceMemoryBase> {
        public:
    };

    // 
    class BufferAllocationBase: public std::enable_shared_from_this<BufferAllocationBase> { 
        public:
    };

    //
    class ImageViewBase : public std::enable_shared_from_this<ImageViewBase> {
        public:
    };

};
