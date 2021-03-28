#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {

    // 
    class DeviceBuffer: public MemoryAllocation{
    protected: friend MemoryAllocator; friend BufferAllocation; // 
        std::shared_ptr<DeviceBufferBase> api = {};

    public: // 
        DeviceBuffer(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}, const std::shared_ptr<DeviceBufferBase>& api = {}): MemoryAllocation(deviceMemory, data), api(api) 
        {};

        // 
        DeviceBuffer(const std::shared_ptr<Device>& device = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}, const std::shared_ptr<DeviceBufferBase>& api = {}) : MemoryAllocation(device, data), api(api) 
        {};

        // 
        DeviceBuffer(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const std::shared_ptr<DeviceBufferBase>& api = {}): MemoryAllocation(deviceMemory), api(api) 
        {};

        // 
        DeviceBuffer(const std::shared_ptr<Device>& device = {}, const std::shared_ptr<DeviceBufferBase>& api = {}) : MemoryAllocation(device), api(api) 
        {};

        // 
        virtual uintptr_t& getDeviceAddress() override;
        virtual uintptr_t getDeviceAddress() const override;

        // 
        virtual std::shared_ptr<MemoryAllocation> bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) override;
        virtual std::shared_ptr<MemoryAllocation> create(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) override;

        // 
        virtual inline std::shared_ptr<MemoryAllocation> getMemoryAllocation() { return shared_from_this(); };
        virtual inline std::shared_ptr<DeviceBufferBase>& getApi() { return api; };

        // 
        //virtual inline std::shared_ptr<MemoryAllocation> getMemoryAllocation() const { return shared_from_this(); };
        virtual inline const std::shared_ptr<DeviceBufferBase>& getApi() const { return api; };
    };



    // abscent class...
    class BufferAllocation: public std::enable_shared_from_this<BufferAllocation> {
    protected: friend DeviceBuffer; // 
        std::shared_ptr<DeviceBuffer> deviceBuffer = {};
        std::shared_ptr<BufferAllocationBase> data = {};

    public: // 
        // legacy
        BufferAllocation(const std::shared_ptr<DeviceBuffer>& deviceBuffer, const uintptr_t& offset = 0ull, const uintptr_t& range = 0xFFFFFFFF, const std::shared_ptr<BufferAllocationBase>& data = {}): deviceBuffer(deviceBuffer), data(data) 
        { this->constructor(offset, range); };

        // 
        virtual std::shared_ptr<BufferAllocation> constructor(const uintptr_t& offset = 0ull, const uintptr_t& range = 0xFFFFFFFF) {
            if (!this->data) { this->data = std::make_shared<BufferAllocationBase>(); };
            return shared_from_this();
        };

        // 
        virtual inline uintptr_t getDeviceAddress() { return 0ull; };
        virtual inline std::shared_ptr<DeviceBuffer>& getDeviceBuffer() { return deviceBuffer; };
        virtual inline std::shared_ptr<BufferAllocationBase>& getData() { return data; };

        // 
        virtual inline uintptr_t getDeviceAddress() const { return 0ull; };
        virtual inline const std::shared_ptr<DeviceBuffer>& getDeviceBuffer() const { return deviceBuffer; };
        virtual inline const std::shared_ptr<BufferAllocationBase>& getData() const { return data; };
    };

};
