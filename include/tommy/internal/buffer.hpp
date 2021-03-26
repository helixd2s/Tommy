#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {


    // 
    class DeviceBuffer: public MemoryAllocation{
    protected: friend MemoryAllocator; friend BufferAllocation; // 
        std::shared_ptr<DeviceBufferBase> api = {};
        std::function<void()> destructor = {};
        void* allocation = nullptr;

    public: // 

        // 
        virtual vk::DeviceAddress& getDeviceAddress();
        virtual vk::DeviceAddress getDeviceAddress() const;

        // 
        virtual std::shared_ptr<DeviceBuffer> bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});
        virtual std::shared_ptr<DeviceBuffer> create(const vk::BufferCreateInfo& info = {}, const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});

        // 
        virtual inline std::shared_ptr<MemoryAllocation> getMemoryAllocation() { return shared_from_this(); };
        virtual inline std::shared_ptr<DeviceBufferBase> getApi() { return api; };

        // 
        //virtual inline std::shared_ptr<MemoryAllocation> getMemoryAllocation() const { return shared_from_this(); };
        virtual inline std::shared_ptr<DeviceBufferBase> getApi() { return api; };
    };



    // abscent class...
    class BufferAllocation: public std::enable_shared_from_this<BufferAllocation> {
    protected:  // 
        friend DeviceBuffer;
        std::shared_ptr<DeviceBuffer> deviceBuffer = {};
        std::shared_ptr<BufferAllocationBase> data = {};

    public: // 
        // legacy
        BufferAllocation(const std::shared_ptr<DeviceBuffer>& deviceBuffer, const vk::DeviceSize& offset = 0ull, const vk::DeviceSize& range = VK_WHOLE_SIZE): deviceBuffer(deviceBuffer) {
            this->constructor(offset, range);
        };

        // 
        virtual std::shared_ptr<BufferAllocation> constructor(const vk::DeviceSize& offset = 0ull, const vk::DeviceSize& range = VK_WHOLE_SIZE) {
            return shared_from_this();
        };

        // 
        virtual inline vk::DeviceAddress getDeviceAddress() { return 0ull; };
        virtual inline std::shared_ptr<DeviceBuffer>& getDeviceBuffer() { return deviceBuffer; };

        // 
        virtual inline vk::DeviceAddress getDeviceAddress() const { return 0ull; };
        virtual inline const std::shared_ptr<DeviceBuffer>& getDeviceBuffer() const { return deviceBuffer; };
    };

};
