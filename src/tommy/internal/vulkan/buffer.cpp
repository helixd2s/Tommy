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
        std::shared_ptr<tom::MemoryAllocation> DeviceBuffer::bindMemory(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
            auto api = this->getApiTyped();
            if (memoryAllocation) {
                this->data = memoryAllocation->getData();
                this->deviceMemory = memoryAllocation->getDeviceMemory();
            };
            if (this->data) {
                std::dynamic_pointer_cast<DeviceData>(this->getDevice()->getData())->device.bindBufferMemory2(vk::BindBufferMemoryInfo{
                    .buffer = api->buffer,
                    .memory = std::dynamic_pointer_cast<DeviceMemoryApi>(deviceMemory->getApi())->memory,
                    .memoryOffset = data->memoryOffset
                });
            };
            return std::dynamic_pointer_cast<tom::MemoryAllocation>(shared_from_this());
        };

        // auto api = self->getApi();
        std::shared_ptr<tom::MemoryAllocation> DeviceBuffer::create(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
            auto api = this->getApiTyped();
            auto self = std::dynamic_pointer_cast<DeviceBuffer>(shared_from_this());
            auto device = this->getDevice();
            api->buffer = std::dynamic_pointer_cast<DeviceData>(device->getData())->device.createBuffer( api->info.queueFamilyIndexCount ? api->info : vk::BufferCreateInfo(api->info).setQueueFamilyIndices(device->getQueueFamilyIndices()) );
            std::dynamic_pointer_cast<Device>(device)->setDeviceBufferObject(std::dynamic_pointer_cast<tom::DeviceBuffer>(shared_from_this()));
            this->bindMemory(memoryAllocation);
            api->address = 0ull;
            return self;
        };

        // 
        vk::DeviceAddress& DeviceBuffer::getDeviceAddress() {
            auto api = this->getApiTyped();
            return (api->address = api->address ? api->address : std::dynamic_pointer_cast<DeviceData>(this->getDevice()->getData())->device.getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = api->buffer }));
        };

        // 
        vk::DeviceAddress DeviceBuffer::getDeviceAddress() const {
            auto api = this->getApiTyped();
            return (api->address ? api->address : std::dynamic_pointer_cast<DeviceData>(this->getDevice()->getData())->device.getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = api->buffer }));
        };

    };

};
