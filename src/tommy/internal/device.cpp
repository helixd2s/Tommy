#pragma once

#include <tommy/internal/device.hpp>
#include <tommy/internal/buffer.hpp>
#include <tommy/internal/memory.hpp>

// 
namespace tom {
    // 
    std::shared_ptr<DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer) const {
        std::shared_ptr<DeviceBuffer> deviceBuffer = {};

        if (buffers.find(buffer) != buffers.end()) {
            deviceBuffer = buffers.at(buffer).lock();
        };

        return deviceBuffer;
    };

    // 
    std::shared_ptr<DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const {
        std::shared_ptr<DeviceMemory> deviceMemoryObj = {};

        if (memories.find(deviceMemory) != memories.end()) {
            deviceMemoryObj = memories.at(deviceMemory).lock();
        };

        return deviceMemoryObj;
    };


    // 
    std::shared_ptr<DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer){
        std::shared_ptr<DeviceBuffer> deviceBuffer = {};

        if (buffers.find(buffer) != buffers.end()) {
            deviceBuffer = buffers.at(buffer).lock();
        };

        if (!deviceBuffer) { 
            deviceBuffer = std::make_shared<DeviceBuffer>(shared_from_this(), buffer);
            buffers.insert(std::make_pair<vk::Buffer, std::weak_ptr<DeviceBuffer>>(vk::Buffer(buffer), std::weak_ptr<DeviceBuffer>(deviceBuffer)));
        };

        return deviceBuffer;
    };

    // 
    std::shared_ptr<DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory){
        std::shared_ptr<DeviceMemory> deviceMemoryObj = {};

        if (memories.find(deviceMemory) != memories.end()) {
            deviceMemoryObj = memories.at(deviceMemory).lock();
        };

        if (!deviceMemoryObj) { 
            deviceMemoryObj = std::make_shared<DeviceMemory>(shared_from_this(), deviceMemory);
            memories.insert(std::make_pair<vk::DeviceMemory, std::weak_ptr<DeviceMemory>>(vk::DeviceMemory(deviceMemory), std::weak_ptr<DeviceMemory>(deviceMemoryObj)));
        };

        return deviceMemoryObj;
    };
};
