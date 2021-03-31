#pragma once

// 
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <map>
#include <future>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#else

#endif

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



#ifdef _WIN32
    using ExtHandleType = HANDLE; // Win32
#else
    using ExtHandleType = int; // Fd
#endif

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
    class QueueBase: public std::enable_shared_from_this<QueueBase> { public: // 
        uint32_t queueFamilyIndex = 0u;
    };

    // 
    using DeviceImageKey = uintptr_t;
    using DeviceBufferKey = uintptr_t;
    using DeviceMemoryKey = uintptr_t;

    // 
    using image_t = DeviceImageKey;
    using buffer_t = DeviceBufferKey;
    using memory_t = DeviceMemoryKey;
    using image_view_t = ImageViewKey;

    // 
    class DeviceBase: public std::enable_shared_from_this<DeviceBase> { public: 
        std::unordered_map<uint32_t, std::vector<std::shared_ptr<Queue>>> queues = {};
        std::vector<uint32_t> queueFamilyIndices = {};

        // 
        std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices = {};

        // 
        std::unordered_map<uintptr_t, std::shared_ptr<MemoryAllocator>> allocators = {};
        std::unordered_map<uintptr_t, std::shared_ptr<BufferAllocation>> bufferAllocations = {};
        std::unordered_map<DeviceImageKey, std::shared_ptr<DeviceImage>> images = {};
        std::unordered_map<DeviceBufferKey, std::shared_ptr<DeviceBuffer>> buffers = {};
        std::unordered_map<DeviceMemoryKey, std::shared_ptr<DeviceMemory>> memories = {};
        std::unordered_map<ImageViewKey, std::shared_ptr<ImageView>> imageViews = {};

        // 
        static std::shared_ptr<DeviceBase> makeShared(std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices = {}) {
            auto data = std::make_shared<DeviceBase>();
            data->physicalDevices = physicalDevices;
            return data;
        };
    };

    // 
    class PhysicalDeviceBase: public std::enable_shared_from_this<PhysicalDeviceBase> {
        public:
    };

    // 
    class InstanceBase: public std::enable_shared_from_this<InstanceBase> { public: 
        
    };

    //
    struct ImageRegion {
        glm::ivec3 offset = glm::ivec3{0,0,0};
        glm::uvec3 extent = glm::uvec3{65536u,65536u,65536u};
        uint32_t mipLevel = 0u;
    };

    //
    using ApiResult = uint32_t;

    // 
    class MemoryAllocationBase: public std::enable_shared_from_this<MemoryAllocationBase> { public: // 
        void* allocation = nullptr;
        void* mapped = nullptr;
        std::function<void()> destructor = {};
        uintptr_t memoryOffset = 0ull;

        // 
        ~MemoryAllocationBase() {
            if (this->destructor) { this->destructor(); };
            this->destructor = {};
        };

        // 
        static std::shared_ptr<MemoryAllocationBase> makeShared(const uintptr_t& memoryOffset = 0ull) {
            std::shared_ptr<MemoryAllocationBase> data = {};
            data->memoryOffset = memoryOffset;
            return data;
        };
    };

    // 
    class MemoryAllocatorBase: public std::enable_shared_from_this<MemoryAllocatorBase> { public: // 
        void* allocator = nullptr;
    };

    // 
    class DeviceImageBase: public std::enable_shared_from_this<DeviceImageBase> { public:
        std::function<void()> destructor = {};
    };

    // 
    class DeviceBufferBase: public std::enable_shared_from_this<DeviceBufferBase> { public: 
        std::function<void()> destructor = {};
    };

    // 
    class DeviceMemoryBase: public std::enable_shared_from_this<DeviceMemoryBase> { public: //
        void* allocation = nullptr;
        void* mapped = nullptr;
        std::function<void()> destructor = {};
        ExtHandleType extHandle = {};

        // 
        ~DeviceMemoryBase() {
            if (this->destructor) { this->destructor(); };
            this->destructor = {};
        };
    };

    // 
    class DeviceMemoryApiBase: public std::enable_shared_from_this<DeviceMemoryApiBase> { public: //
        
    };

    // 
    class BufferAllocationBase: public std::enable_shared_from_this<BufferAllocationBase> { public: //
    };

    // 
    class ImageViewBase : public std::enable_shared_from_this<ImageViewBase> {
        public:
        ImageViewKey key = {};
    };

    // 
    class ComputePipelineBase: public std::enable_shared_from_this<ComputePipelineBase> { public: //
        
    };



};
