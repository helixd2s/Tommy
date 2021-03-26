

#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "./device.hpp"

// 
namespace tom {

    //
    class DeviceMemory: public std::enable_shared_from_this<DeviceMemory> {
    protected: friend MemoryAllocator;
        std::weak_ptr<Device> device = {};
        std::shared_ptr<DeviceMemoryBase> data = {};
        std::function<void()> destructor = {};

        // 
        void* allocation = nullptr;
        void* mapped = nullptr;

        //
        //uint32_t glHandle = 0u;

    public: // 
        // legacy
        DeviceMemory(const std::shared_ptr<Device>& device, const std::shared_ptr<DeviceMemoryBase>& data = {}): device(device), data(data) {
            
        };

        // 
        ~DeviceMemory() {
            if (this->destructor) { this->destructor(); };
        };

        // 
        std::shared_ptr<DeviceMemory> allocate(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});

        // 
        virtual inline std::shared_ptr<DeviceMemoryBase> getData() { return data; };
        virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
        virtual inline void*& getAllocation() { return allocation; };
        virtual inline void*& getMapped() { return mapped; };

        // 
        virtual inline std::shared_ptr<DeviceMemoryBase> getData() const { return data; };
        virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
        virtual inline void* const& getAllocation() const { return allocation; };
        virtual inline void* const& getMapped() const { return mapped; };
    };

    // 
    class MemoryAllocation: public std::enable_shared_from_this<MemoryAllocation> {
    protected: friend MemoryAllocator; friend DeviceMemory; friend MemoryAllocation;
        std::weak_ptr<Device> device = {};
        std::shared_ptr<DeviceMemory> deviceMemory = {};
        std::shared_ptr<MemoryAllocationBase> data = {};
        std::function<void()> destructor = {};

        // 
        uintptr_t memoryOffset = 0ull;
        void* allocation = nullptr;
        void* mapped = nullptr;

    public: // 
        MemoryAllocation(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const vk::DeviceSize& memoryOffset = 0ull, const std::shared_ptr<MemoryAllocationBase>& data = {}): data(data), deviceMemory(deviceMemory), device(deviceMemory->getDevice()), memoryOffset(memoryOffset) {
        };

        MemoryAllocation(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}): data(data), deviceMemory(deviceMemory), device(deviceMemory->getDevice()) {
        };

        MemoryAllocation(const std::shared_ptr<Device>& device = {}) : device(device) {
        };

        // 
        ~MemoryAllocation() {
            if (this->destructor) { this->destructor(); };
            if (this->deviceMemory) { this->deviceMemory = {}; };
            this->destructor = {};
            this->deviceMemory = {};
            this->memoryOffset = 0ull;
        };

        // 
        virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
        virtual inline std::shared_ptr<MemoryAllocationBase> getData() { return data; };
        virtual inline std::shared_ptr<DeviceMemory>& getDeviceMemory() { return deviceMemory; };
        virtual inline void* getMapped() { return mapped ? mapped : ((uint8_t*)deviceMemory->getMapped() + this->memoryOffset); };
        virtual inline void*& getAllocation() { return allocation; };
        virtual inline void*& getMappedDefined() { return mapped; };
        virtual inline uintptr_t& getMemoryOffset() { return memoryOffset; };

        // 
        virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
        virtual inline std::shared_ptr<MemoryAllocationBase> getData() const { return data; };
        virtual inline const std::shared_ptr<DeviceMemory>& getDeviceMemory() const { return deviceMemory; };
        virtual inline const void* getMapped() const { return mapped ? mapped : ((const uint8_t*)deviceMemory->getMapped() + this->memoryOffset); };
        virtual inline void* const& getAllocation() const { return allocation; };
        virtual inline void* const& getMappedDefined() const { return mapped; };
        virtual inline const uintptr_t& getMemoryOffset() const { return memoryOffset; };
    };

};
