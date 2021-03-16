

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
        std::shared_ptr<tom::Device>& getDevice() { return device; };
        vk::DeviceMemory& getMemory() { return memory; };

        //
        const std::shared_ptr<tom::Device>& getDevice() const { return device; };
        const vk::DeviceMemory& getMemory() const { return memory; };
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
        std::shared_ptr<tom::DeviceMemory>& getDeviceMemory() { return deviceMemory; };
        vk::DeviceSize& getMemoryOffset() { return memoryOffset; };

        // 
        const std::shared_ptr<tom::DeviceMemory>& getDeviceMemory() const { return deviceMemory; };
        const vk::DeviceSize& getMemoryOffset() const { return memoryOffset; };
    };

};
