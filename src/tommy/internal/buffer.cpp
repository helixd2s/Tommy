#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/buffer.hpp>



// 
namespace tom {

    // 
    vk::DeviceAddress& DeviceBuffer::getDeviceAddress() {
        return (address = address ? address : this->device->getDevice().getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = this->buffer }));
    };

    // 
    vk::DeviceAddress DeviceBuffer::getDeviceAddress() const {
        return (address ? address : this->device->getDevice().getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = this->buffer }));
    };

    // 
    std::shared_ptr<DeviceBuffer> DeviceBuffer::bindMemory(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
        if (memoryAllocation) {
            this->memoryAllocation = memoryAllocation;
            this->device->getDevice().bindBufferMemory2(vk::BindBufferMemoryInfo{
                .buffer = this->buffer,
                .memory = this->memoryAllocation->getDeviceMemory()->getMemory(),
                .memoryOffset = this->memoryAllocation->getOffset()
            });
        };
        return shared_from_this();
    };

    // 
    std::shared_ptr<DeviceBuffer> DeviceBuffer::create(const vk::BufferCreateInfo& info = {}, const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
        this->buffer = this->device->getDevice().createBuffer( this->info = info.queueFamilyIndexCount ? vk::BufferCreateInfo(info) : vk::BufferCreateInfo(info).setQueueFamilyIndices(this->device->getQueueFamilyIndices()) );
        this->bindMemory(memoryAllocation);
        this->address = 0ull;
        return shared_from_this();
    };

};
