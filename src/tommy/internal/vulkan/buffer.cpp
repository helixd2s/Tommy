#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>
#include <tommy/internal/vulkan/buffer.hpp>



// 
namespace tom {

    // 
    namespace vulkan {
        // 
        vk::DeviceAddress& DeviceBuffer::getDeviceAddress() {
            return (data->address = data->address ? data->address : this->getDevice()->getData()->device.getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = data->buffer }));
        };

        // 
        vk::DeviceAddress DeviceBuffer::getDeviceAddress() const {
            return (data->address ? data->address : this->getDevice()->getData()->device.getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = data->buffer }));
        };

        // 
        std::shared_ptr<DeviceBuffer> DeviceBuffer::bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) {
            if (memoryAllocation) {
                this->memoryAllocation = memoryAllocation;
                this->getDevice()->getData()->device.bindBufferMemory2(vk::BindBufferMemoryInfo{
                    .buffer = data->buffer,
                    .memory = this->memoryAllocation->getDeviceMemory()->getData()->memory,
                    .memoryOffset = this->memoryAllocation->getData()->memoryOffset
                });
            };
            return shared_from_this();
        };

        // 
        std::shared_ptr<DeviceBuffer> DeviceBuffer::create(const vk::BufferCreateInfo& info = {}, const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) {
            auto device = this->getDevice();
            data->buffer = device->getData()->device.createBuffer( data->info = info.queueFamilyIndexCount ? vk::BufferCreateInfo(info) : vk::BufferCreateInfo(info).setQueueFamilyIndices(this->getDevice()->getQueueFamilyIndices()) );
            device->setDeviceBufferObject(shared_from_this());
            this->bindMemory(memoryAllocation);
            data->address = 0ull;
            return shared_from_this();
        };
    };

};
