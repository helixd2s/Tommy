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
        std::shared_ptr<DeviceBuffer> DeviceBuffer::bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) {
            if (memoryAllocation) {
                this->data = memoryAllocation->getData();
                this->memoryOffset = memoryAllocation->getMemoryOffset();
                this->mapped = memoryAllocation->getMapped();
                this->allocation = memoryAllocation->getAllocation();
            };
            if (this->data) {
                this->getDeviceMemory()->getDevice()->getData()->device.bindBufferMemory2(vk::BindBufferMemoryInfo{
                    .buffer = api->buffer,
                    .memory = deviceMemory->getData()->memory,
                    .memoryOffset = this->memoryOffset
                });
            };
            return std::dynamic_pointer_cast<DeviceBuffer>(shared_from_this());
        };

        // auto api = self->getApi();
        std::shared_ptr<DeviceBuffer> DeviceBuffer::create(const vk::BufferCreateInfo& info = {}, const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) {
            auto self = std::dynamic_pointer_cast<DeviceBuffer>(shared_from_this());
            auto device = memoryAllocation ? memoryAllocation->getDeviceMemory()->getDevice() : this->getDeviceMemory()->getDevice();
            api->buffer = device->getData()->device.createBuffer( api->info = info.queueFamilyIndexCount ? vk::BufferCreateInfo(info) : vk::BufferCreateInfo(info).setQueueFamilyIndices(device->getQueueFamilyIndices()) );
            device->setDeviceBufferObject(self);
            this->bindMemory(memoryAllocation);
            api->address = 0ull;
            return self;
        };

        // 
        vk::DeviceAddress& DeviceBuffer::getDeviceAddress() {
            return (api->address = api->address ? api->address : this->getDeviceMemory()->getDevice()->getData()->device.getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = api->buffer }));
        };

        // 
        vk::DeviceAddress DeviceBuffer::getDeviceAddress() const {
            return (api->address ? api->address : this->getDeviceMemory()->getDevice()->getData()->device.getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = api->buffer }));
        };

    };

};
