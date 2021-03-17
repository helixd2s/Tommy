

#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"

// 
namespace tom {

    //
    class DeviceMemory: public std::enable_shared_from_this<DeviceMemory> {
    protected:
        std::shared_ptr<tom::Device> device = {};

        // 
        vk::DeviceMemory memory = {};

    public: 
        DeviceMemory(const std::shared_ptr<tom::Device>& device, const vk::DeviceMemory& memory = {}): device(device), memory(memory) {
            
        };

        // 
        virtual std::shared_ptr<tom::Device>& getDevice() { return device; };
        virtual vk::DeviceMemory& getMemory() { return memory; };

        //
        virtual const std::shared_ptr<tom::Device>& getDevice() const { return device; };
        virtual const vk::DeviceMemory& getMemory() const { return memory; };
    };

    // 
    class MemoryAllocation: public std::enable_shared_from_this<MemoryAllocation> {
    protected:
        std::shared_ptr<tom::DeviceMemory> deviceMemory = {};

        // 
        vk::DeviceSize memoryOffset = 0ull;

    public: 
        MemoryAllocation(const std::shared_ptr<tom::DeviceMemory>& deviceMemory = {}, const vk::DeviceSize& memoryOffset = 0ull): deviceMemory(deviceMemory), memoryOffset(memoryOffset) {
            
        };

        // 
        virtual std::shared_ptr<tom::DeviceMemory>& getDeviceMemory() { return deviceMemory; };
        virtual vk::DeviceSize& getOffset() { return memoryOffset; };

        // 
        virtual const std::shared_ptr<tom::DeviceMemory>& getDeviceMemory() const { return deviceMemory; };
        virtual const vk::DeviceSize& getOffset() const { return memoryOffset; };
    };

};
