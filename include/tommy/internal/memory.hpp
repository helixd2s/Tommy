

#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"

// 
namespace tom {

    //
    class DeviceMemory: public std::enable_shared_from_this<DeviceMemory> {
    protected: friend MemoryAllocator;
        std::weak_ptr<Device> device = {};
        std::shared_ptr<DeviceMemoryBase> data = {};
        std::shared_ptr<ExtHandleType> extHandle = {};

    public: // 
        // 
        DeviceMemory(const std::shared_ptr<Device>& device, const std::shared_ptr<DeviceMemoryBase>& data = {}): device(device), data(data) {
            
        };

        // 
        ~DeviceMemory() {
        };

        // 
        virtual std::shared_ptr<DeviceBuffer> bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});

        // 
        virtual inline std::shared_ptr<DeviceMemoryBase> getData() { return data; };
        virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
        virtual inline std::shared_ptr<ExtHandleType> getExtHandle() { return extHandle; };

        // 
        virtual inline std::shared_ptr<DeviceMemoryBase> getData() const { return data; };
        virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
        virtual inline std::shared_ptr<ExtHandleType> getExtHandle() const { return extHandle; };
    };

    // 
    class MemoryAllocation: public std::enable_shared_from_this<MemoryAllocation> {
    protected: friend MemoryAllocator; friend DeviceMemory; friend MemoryAllocation;
        std::weak_ptr<Device> device = {};
        std::shared_ptr<MemoryAllocationBase> data = {};
        std::shared_ptr<DeviceMemory> deviceMemory = {};
        

    public: // 
        MemoryAllocation(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}): data(data), deviceMemory(deviceMemory), device(deviceMemory->getDevice()) {
        };

        MemoryAllocation(const std::shared_ptr<Device>& device = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}) : device(device), data(data) {
        };

        // 
        ~MemoryAllocation() {
            this->deviceMemory = {};
            this->data = {};
        };

        // 
        virtual uintptr_t& getDeviceAddress();
        virtual uintptr_t getDeviceAddress() const;

        // 
        virtual std::shared_ptr<MemoryAllocation> bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});
        virtual std::shared_ptr<MemoryAllocation> create(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});

        // 
        virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
        virtual inline std::shared_ptr<MemoryAllocationBase> getData() { return data; };
        virtual inline std::shared_ptr<DeviceMemory>& getDeviceMemory() { return deviceMemory; };
        virtual inline void* getMapped() { return data->mapped ? data->mapped : ((uint8_t*)deviceMemory->getData()->mapped + data->memoryOffset); };

        // 
        virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
        virtual inline std::shared_ptr<MemoryAllocationBase> getData() const { return data; };
        virtual inline const std::shared_ptr<DeviceMemory>& getDeviceMemory() const { return deviceMemory; };
        virtual inline const void* getMapped() const { return data->mapped ? data->mapped : ((const uint8_t*)deviceMemory->getData()->mapped + data->memoryOffset); };
    };

};
