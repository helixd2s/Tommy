#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/buffer.hpp>


// 
namespace tom {

    // 
    std::shared_ptr<MemoryAllocator>& Device::createAllocatorVma() {
        if (!this->allocator) {
            this->allocator = std::dynamic_pointer_cast<MemoryAllocator>(std::make_shared<MemoryAllocatorVma>(shared_from_this()));
        };
        return this->allocator;
        //return std::dynamic_pointer_cast<MemoryAllocatorVma>(this->allocator);
    };

    // 
    std::shared_ptr<MemoryAllocator> MemoryAllocator::allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = buffer;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        if (self->buffer) { // 
            auto memoryRequirements = vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirementsKHR>{ vk::MemoryRequirements2{  }, vk::MemoryDedicatedRequirementsKHR{} };
            device->getDevice().getBufferMemoryRequirements2(vk::BufferMemoryRequirementsInfo2{ .buffer = self->buffer }, memoryRequirements);

            // 
            const auto& memReqs = memoryRequirements.get<vk::MemoryRequirements2>().memoryRequirements;

            // 
            self->bindMemory(std::make_shared<tom::MemoryAllocation>(device->allocateMemoryObject(allocator, vk::StructureChain<vk::MemoryAllocateInfo, vk::MemoryDedicatedAllocateInfoKHR, vk::ExternalMemoryBufferCreateInfo>{
                vk::MemoryAllocateInfo{
                    .allocationSize = memReqs.size,
                    .memoryTypeIndex = device->getPhysicalDevice()->getMemoryType(memReqs.memoryTypeBits),
                },
                vk::MemoryDedicatedAllocateInfoKHR{ .buffer = self->buffer },
                vk::ExternalMemoryBufferCreateInfo{}
            }.get<vk::MemoryAllocateInfo>()), 0ull));
        };

        return shared_from_this();
    };

    //
    std::shared_ptr<MemoryAllocator> MemoryAllocator::allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        this->allocateBuffer(buffer->create(info), memUsage);
        return shared_from_this();
    };

};
