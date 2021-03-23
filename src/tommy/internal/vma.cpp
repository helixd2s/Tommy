#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/buffer.hpp>
#include <tommy/internal/image.hpp>

// 
#include <vulkan/vk_mem_alloc.h>

// 
namespace tom {

    //
    std::shared_ptr<MemoryAllocator> MemoryAllocatorVma::constructor() { //
        auto device = this->device.lock();
        auto& instanceDispatch = device->getInstance()->getDispatch();
        auto& deviceDispatch = device->getDispatch();

        // redirect Vulkan API functions
        VmaVulkanFunctions func = {};
        func.vkAllocateMemory = deviceDispatch.vkAllocateMemory;
        func.vkBindBufferMemory = deviceDispatch.vkBindBufferMemory;
        func.vkBindBufferMemory2KHR = deviceDispatch.vkBindBufferMemory2;
        func.vkBindImageMemory = deviceDispatch.vkBindImageMemory;
        func.vkBindImageMemory2KHR = deviceDispatch.vkBindImageMemory2;
        func.vkCmdCopyBuffer = deviceDispatch.vkCmdCopyBuffer;
        func.vkCreateBuffer = deviceDispatch.vkCreateBuffer;
        func.vkCreateImage = deviceDispatch.vkCreateImage;
        func.vkDestroyBuffer = deviceDispatch.vkDestroyBuffer;
        func.vkDestroyImage = deviceDispatch.vkDestroyImage;
        func.vkFlushMappedMemoryRanges = deviceDispatch.vkFlushMappedMemoryRanges;
        func.vkFreeMemory = deviceDispatch.vkFreeMemory;
        func.vkGetBufferMemoryRequirements = deviceDispatch.vkGetBufferMemoryRequirements;
        func.vkGetBufferMemoryRequirements2KHR = deviceDispatch.vkGetBufferMemoryRequirements2;
        func.vkGetImageMemoryRequirements = deviceDispatch.vkGetImageMemoryRequirements;
        func.vkGetImageMemoryRequirements2KHR = deviceDispatch.vkGetImageMemoryRequirements2;
        func.vkGetPhysicalDeviceMemoryProperties = instanceDispatch.vkGetPhysicalDeviceMemoryProperties;
        func.vkGetPhysicalDeviceMemoryProperties2KHR = instanceDispatch.vkGetPhysicalDeviceMemoryProperties2;
        func.vkGetPhysicalDeviceProperties = instanceDispatch.vkGetPhysicalDeviceProperties;
        func.vkInvalidateMappedMemoryRanges = deviceDispatch.vkInvalidateMappedMemoryRanges;
        func.vkMapMemory = deviceDispatch.vkMapMemory;
        func.vkUnmapMemory = deviceDispatch.vkUnmapMemory;

        // 
        VmaAllocatorCreateInfo vmaInfo = {};
        vmaInfo.pVulkanFunctions = &func;
        vmaInfo.device = device->getDevice();
        vmaInfo.instance = device->getInstance()->getInstance();
        vmaInfo.physicalDevice = device->getPhysicalDevice()->getPhysicalDevice();
        vmaInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        // 
        vk::throwResultException(vk::Result(vmaCreateAllocator(&vmaInfo, &(VmaAllocator&)this->allocator)), "Failed to create VMA allocator...");

        //
        return shared_from_this();
    };


    //
    std::shared_ptr<MemoryAllocator> MemoryAllocatorVma::allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = buffer;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        if (self->buffer) { // 
            VmaAllocationInfo allocInfo = {};
            VmaAllocationCreateInfo allocCreateInfo = { .usage = memUsage };
            if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
            };

            // 
            vk::throwResultException(vk::Result(vmaAllocateMemoryForBuffer((const VmaAllocator&)allocator->getAllocator(), self->buffer, &allocCreateInfo, &((VmaAllocation&)self->allocation), &allocInfo)), "VMA buffer allocation failed...");

            // TODO: use native allocation
            auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory); // TODO: IMPORTANT!!!
            self->bindMemory(std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset));
            self->memoryAllocation->getAllocation() = self->allocation;
            self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData;
            //deviceMemory->getMapped() = allocInfo.pMappedData; // not sure...

            //
            self->destructor = [self = std::weak_ptr(self), allocator](){
                auto of = self.lock();
                vmaFreeMemory((const VmaAllocator&)allocator->getAllocator(), (VmaAllocation&)of->getMemoryAllocation());
            };
        };

        return shared_from_this();
    };

    //
    std::shared_ptr<MemoryAllocator> MemoryAllocatorVma::allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = buffer;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        // 
        self->info = info;

        // 
        VmaAllocationCreateInfo allocCreateInfo = { .usage = memUsage };
        if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
            allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
        };

        // 
        vk::throwResultException(vk::Result(vmaCreateBuffer((const VmaAllocator&)allocator->getAllocator(), (const VkBufferCreateInfo*)&info, &allocCreateInfo, (VkBuffer*)&self->buffer, &((VmaAllocation&)self->allocation), nullptr)), "VMA buffer allocation failed...");

        // get allocation info
        VmaAllocationInfo allocInfo = {};
        vmaGetAllocationInfo((const VmaAllocator&)allocator->getAllocator(), ((VmaAllocation&)self->allocation), &allocInfo);

        // wrap device memory
        auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory);
        self->memoryAllocation = std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset);
        self->memoryAllocation->getAllocation() = self->allocation;
        self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData;
        //deviceMemory->getMapped() = allocInfo.pMappedData; // not sure...

        //deviceMemory->getAllocation() = allocation; // not sure...

        self->destructor = [self = std::weak_ptr(self), allocator](){
            auto of = self.lock();
            vmaDestroyBuffer((const VmaAllocator&)allocator->getAllocator(), of->getBuffer(), (VmaAllocation&)of->getMemoryAllocation());
            of->getBuffer() = vk::Buffer{};
            of->getMemoryAllocation() = nullptr;
        };

        return shared_from_this();
    };


    //
    std::shared_ptr<MemoryAllocator> MemoryAllocatorVma::allocateImage(const std::shared_ptr<DeviceImage>& image, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = image;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        if (self->image) { // 
            VmaAllocationInfo allocInfo = {};
            VmaAllocationCreateInfo allocCreateInfo = { .usage = memUsage };
            if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
            };

            // 
            vk::throwResultException(vk::Result(vmaAllocateMemoryForImage((const VmaAllocator&)allocator->getAllocator(), self->image, &allocCreateInfo, &((VmaAllocation&)self->allocation), &allocInfo)), "VMA image allocation failed...");

            // TODO: use native allocation
            auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory); // TODO: IMPORTANT!!!
            self->bindMemory(std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset));
            self->memoryAllocation->getAllocation() = self->allocation;
            self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData;
            //deviceMemory->getMapped() = allocInfo.pMappedData; // not sure...

            //
            self->destructor = [self = std::weak_ptr(self), allocator](){
                auto of = self.lock();
                vmaFreeMemory((const VmaAllocator&)allocator->getAllocator(), (VmaAllocation&)of->getMemoryAllocation());
            };
        };

        return shared_from_this();
    };

    //
    std::shared_ptr<MemoryAllocator> MemoryAllocatorVma::allocateAndCreateImage(const std::shared_ptr<DeviceImage>& image, const vk::ImageCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = image;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        // 
        self->info = info;

        // 
        VmaAllocationCreateInfo allocCreateInfo = { .usage = memUsage };
        if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
            allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
        };

        // 
        vk::throwResultException(vk::Result(vmaCreateImage((const VmaAllocator&)allocator->getAllocator(), (const VkImageCreateInfo*)&info, &allocCreateInfo, (VkImage*)&self->image, &((VmaAllocation&)self->allocation), nullptr)), "VMA image allocation failed...");

        // 
        self->layoutHistory.clear();
        self->layoutHistory.push_back(info.initialLayout);

        // get allocation info
        VmaAllocationInfo allocInfo = {};
        vmaGetAllocationInfo((const VmaAllocator&)allocator->getAllocator(), ((VmaAllocation&)self->allocation), &allocInfo);

        // wrap device memory
        auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory);
        self->memoryAllocation = std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset);
        self->memoryAllocation->getAllocation() = self->allocation;
        self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData;
        //deviceMemory->getMapped() = allocInfo.pMappedData; // not sure...

        //deviceMemory->getAllocation() = allocation; // not sure...

        self->destructor = [self = std::weak_ptr(self), allocator](){
            auto of = self.lock();
            vmaDestroyImage((const VmaAllocator&)allocator->getAllocator(), of->getImage(), (VmaAllocation&)of->getMemoryAllocation());
            of->getImage() = vk::Image{};
            of->getMemoryAllocation() = nullptr;
        };

        return shared_from_this();
    };

};
