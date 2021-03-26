#pragma once

// 
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

    // 
    namespace vulkan {
        class DescriptorSetSource;
        class DescriptorSetLayouts;
        class DescriptorSet;
        struct MemoryAllocationInfo;
    };

    // 
    struct MemoryAllocationInfo {
        MemoryUsage usage = MemoryUsage::eGPUOnly;
        void* apiInfo = nullptr;

        // 
        MemoryAllocationInfo withVulkan(const vulkan::MemoryAllocationInfo& vinfo);
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
