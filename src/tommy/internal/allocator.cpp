#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/buffer.hpp>
#include <tommy/internal/image.hpp>

// 
namespace tom {

    // 
    std::shared_ptr<MemoryAllocator>& Device::createAllocator() {
        if (!this->allocator) {
            this->allocator = std::make_shared<MemoryAllocator>(shared_from_this());
        };
        return this->allocator;
    };

    // MemoryAllocationInfo
    std::shared_ptr<MemoryAllocation> MemoryAllocator::allocateMemory(const std::shared_ptr<MemoryAllocation>& allocation, const vk::MemoryRequirements2& memoryRequirements = {}, const MemoryAllocationInfo& allocInfo = {}) {
        auto self = allocation;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        // 
        {
            const auto& memReqs = memoryRequirements.memoryRequirements;

            // 
            auto memInfoExt = vk::StructureChain<vk::MemoryDedicatedAllocateInfoKHR, vk::ExternalMemoryBufferCreateInfo>{
                vk::MemoryDedicatedAllocateInfoKHR{ .image = allocInfo.image, .buffer = allocInfo.buffer },
                vk::ExternalMemoryBufferCreateInfo{}
            };

            // TODO: smart allocation 
            self->getOffset() = 0ull;
            self->getDeviceMemory() = device->allocateMemoryObject(allocator, vk::MemoryAllocateInfo{
                .pNext = &memInfoExt.get<vk::MemoryDedicatedAllocateInfoKHR>(),
                .allocationSize = memReqs.size,
                .memoryTypeIndex = device->getPhysicalDevice()->getMemoryType(memReqs.memoryTypeBits),
            });
        };

        return self;
    };

    // 
    std::shared_ptr<DeviceBuffer> MemoryAllocator::allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const MemoryAllocationInfo& allocInfo = {}) {
        auto self = buffer;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        if (self->buffer) { // 
            auto memoryRequirements = vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirementsKHR>{ vk::MemoryRequirements2{  }, vk::MemoryDedicatedRequirementsKHR{} };
            device->getDevice().getBufferMemoryRequirements2(vk::BufferMemoryRequirementsInfo2{ .buffer = self->buffer }, memoryRequirements);
            self->bindMemory(allocator->allocateMemory(std::make_shared<tom::MemoryAllocation>(device), memoryRequirements.get<vk::MemoryRequirements2>(), MemoryAllocationInfo(allocInfo).withBuffer(self->buffer)));
        };

        return self;
    };

    // 
    std::shared_ptr<DeviceImage> MemoryAllocator::allocateImage(const std::shared_ptr<DeviceImage>& image, const MemoryAllocationInfo& allocInfo = {}) {
        auto self = image;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        if (self->image) { // 
            auto memoryRequirements = vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirementsKHR>{ vk::MemoryRequirements2{  }, vk::MemoryDedicatedRequirementsKHR{} };
            device->getDevice().getImageMemoryRequirements2(vk::ImageMemoryRequirementsInfo2{ .image = self->image }, memoryRequirements);
            self->bindMemory(allocator->allocateMemory(std::make_shared<tom::MemoryAllocation>(device), memoryRequirements.get<vk::MemoryRequirements2>(), MemoryAllocationInfo(allocInfo).withImage(self->image)));
        };

        return self;
    };

    // 
    std::shared_ptr<DeviceBuffer> MemoryAllocator::allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const MemoryAllocationInfo& allocInfo = {}) {
        return this->allocateBuffer(buffer->create(info), allocInfo);
    };

    // 
    std::shared_ptr<DeviceImage> MemoryAllocator::allocateAndCreateImage(const std::shared_ptr<DeviceImage>& image, const vk::ImageCreateInfo& info = {}, const MemoryAllocationInfo& allocInfo = {}) {
        return this->allocateImage(image->create(info), allocInfo);
    };

};
