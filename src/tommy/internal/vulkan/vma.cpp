#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>
#include <tommy/internal/vulkan/buffer.hpp>
#include <tommy/internal/vulkan/image.hpp>

// 
#include <vulkan/vk_mem_alloc.h>

// 
namespace tom {

    namespace vulkan {
        // 
        std::shared_ptr<MemoryAllocator>& Device::createAllocatorVma() {
            if (!this->allocator) {
                this->allocator = std::dynamic_pointer_cast<MemoryAllocator>(std::make_shared<MemoryAllocatorVma>(shared_from_this()));
            };
            return this->allocator;
        };

        //
        std::shared_ptr<MemoryAllocator> MemoryAllocatorVma::constructor() { //
            auto device = this->device.lock();
            auto& instanceDispatch = device->getInstance()->getData()->dispatch;
            auto& deviceDispatch = device->getData()->dispatch;

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
            vmaInfo.device = device->getData()->device;
            vmaInfo.instance = device->getInstance()->getData()->instance;
            vmaInfo.physicalDevice = device->getPhysicalDevice()->getData()->physicalDevice;
            vmaInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

            // 
            vk::throwResultException(vk::Result(vmaCreateAllocator(&vmaInfo, &(VmaAllocator&)this->allocator)), "Failed to create VMA allocator...");

            //
            return shared_from_this();
        };

        // MemoryAllocationInfo
        std::shared_ptr<MemoryAllocation> MemoryAllocator::allocateMemory(const std::shared_ptr<MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& memAllocInfo = {}) {
            auto self = allocation;
            auto allocator = shared_from_this();
            auto device = this->device.lock();
            auto data = self->getData();

            //
            auto* vkInfo = (tom::vulkan::MemoryAllocationInfo*)(memAllocInfo.apiInfo);

            // 
            {   // 
                VmaAllocationInfo allocInfo = {};
                VmaAllocationCreateInfo allocCreateInfo = { .usage = reinterpret_cast<const VmaMemoryUsage&>(memAllocInfo.usage) };
                if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
                };

                // 
                if (vkInfo->buffer) {
                    vk::throwResultException(vk::Result(vmaAllocateMemoryForBuffer((const VmaAllocator&)allocator->getAllocator(), vkInfo->buffer, &allocCreateInfo, &((VmaAllocation&)self->allocation), &allocInfo)), "VMA buffer allocation failed...");
                } else 
                if (vkInfo->image) {
                    vk::throwResultException(vk::Result(vmaAllocateMemoryForImage((const VmaAllocator&)allocator->getAllocator(), vkInfo->image, &allocCreateInfo, &((VmaAllocation&)self->allocation), &allocInfo)), "VMA image allocation failed...");
                } else 
                {
                    vk::throwResultException(vk::Result(vmaAllocateMemory((const VmaAllocator&)allocator->getAllocator(), (VkMemoryRequirements*)&vkInfo->requirements.memoryRequirements, &allocCreateInfo, &((VmaAllocation&)self->allocation), &allocInfo)), "VMA memory allocation failed...");
                };

                // 
                self->deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory);
                data->memoryOffset = allocInfo.offset;
                self->allocation = self->allocation;
                self->mapped = allocInfo.pMappedData;
                //deviceMemory->getMapped() = allocInfo.pMappedData; // not sure...

                //
                self->destructor = [allocator, allocation = self->getAllocation()](){
                    vmaFreeMemory((const VmaAllocator&)allocator->getAllocator(), (VmaAllocation&)allocation);
                };
            };

            return self;
        };

        //
        std::shared_ptr<DeviceBuffer> MemoryAllocatorVma::allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const tom::MemoryAllocationInfo& memAllocInfo = {}) {
            auto self = buffer;
            auto allocator = shared_from_this();
            auto device = this->device.lock();
            auto data = self->getData();

            // 
            data->info = info;

            // 
            VmaAllocationInfo allocInfo = {};
            VmaAllocationCreateInfo allocCreateInfo = { .usage = reinterpret_cast<const VmaMemoryUsage&>(memAllocInfo.usage) };
            if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
            };

            // 
            vk::throwResultException(vk::Result(vmaCreateBuffer((const VmaAllocator&)allocator->getAllocator(), (const VkBufferCreateInfo*)&info, &allocCreateInfo, (VkBuffer*)&data->buffer, &((VmaAllocation&)self->allocation), nullptr)), "VMA buffer allocation failed...");
            vmaGetAllocationInfo((const VmaAllocator&)allocator->getAllocator(), ((VmaAllocation&)self->allocation), &allocInfo);
            device->setDeviceBufferObject(self);

            // wrap device memory
            auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory);
            self->memoryAllocation = std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset);
            self->memoryAllocation->getAllocation() = self->allocation;
            self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData;

            // not sure...
            //deviceMemory->getMapped() = allocInfo.pMappedData;
            //deviceMemory->getAllocation() = allocation;

            self->destructor = [data, allocator = allocator->getAllocator(), allocation = self->getMemoryAllocation()](){
                vmaDestroyBuffer((VmaAllocator&)allocator, data->buffer, (VmaAllocation&)allocation);
                data->buffer = vk::Buffer{};
            };

            return self;
        };

        //
        std::shared_ptr<DeviceImage> MemoryAllocatorVma::allocateAndCreateImage(const std::shared_ptr<DeviceImage>& image, const vk::ImageCreateInfo& info = {}, const tom::MemoryAllocationInfo& memAllocInfo = {}) {
            auto self = image;
            auto allocator = shared_from_this();
            auto device = this->device.lock();
            auto data = self->getData();

            // 
            data->info = info;

            // 
            VmaAllocationInfo allocInfo = {};
            VmaAllocationCreateInfo allocCreateInfo = { .usage = reinterpret_cast<const VmaMemoryUsage&>(memAllocInfo.usage) };
            if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
            };

            // 
            vk::throwResultException(vk::Result(vmaCreateImage((const VmaAllocator&)allocator->getAllocator(), (const VkImageCreateInfo*)&info, &allocCreateInfo, (VkImage*)&data->image, &((VmaAllocation&)self->allocation), nullptr)), "VMA image allocation failed...");
            vmaGetAllocationInfo((const VmaAllocator&)allocator->getAllocator(), ((VmaAllocation&)self->allocation), &allocInfo);

            // 
            //self->layoutHistory.clear();
            //self->layoutHistory.push_back(info.initialLayout);

            // wrap device memory
            auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory);
            self->memoryAllocation = std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset);
            self->memoryAllocation->getAllocation() = self->allocation;
            self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData;

            // not sure...
            //deviceMemory->getMapped() = allocInfo.pMappedData;
            //deviceMemory->getAllocation() = allocation;

            self->destructor = [data, allocator = allocator->getAllocator(), allocation = self->getMemoryAllocation()](){
                vmaDestroyImage((VmaAllocator&)allocator, data->image, (VmaAllocation&)allocation);
                data->image = vk::Image{};
            };

            return self;
        };
    };

};
