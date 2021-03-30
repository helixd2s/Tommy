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

    // 
    namespace vulkan {

        // 
        std::shared_ptr<tom::MemoryAllocator>& Device::createAllocatorVma() {
            auto allocator = std::dynamic_pointer_cast<MemoryAllocator>(std::make_shared<MemoryAllocatorVma>(shared_from_this()));
            auto allocatorHandle = uintptr_t(allocator->getData()->allocator);
            data->allocators[allocatorHandle] = allocator;
            return data->allocators.at(allocatorHandle);
        };

        //
        std::shared_ptr<tom::MemoryAllocator> MemoryAllocatorVma::constructor() { //
            auto device = this->getDevice();
            auto& instanceDispatch = std::dynamic_pointer_cast<InstanceData>(device->getInstance()->getData())->dispatch;
            auto& deviceDispatch = std::dynamic_pointer_cast<DeviceData>(device->getData())->dispatch;

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
            vmaInfo.device = std::dynamic_pointer_cast<DeviceData>(device->getData())->device;
            vmaInfo.instance = std::dynamic_pointer_cast<InstanceData>(device->getInstance()->getData())->instance;
            vmaInfo.physicalDevice = std::dynamic_pointer_cast<PhysicalDeviceData>(device->getPhysicalDevice()->getData())->physicalDevice;
            vmaInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

            // 
            vk::throwResultException(vk::Result(vmaCreateAllocator(&vmaInfo, &(VmaAllocator&)data->allocator)), "Failed to create VMA allocator...");

            //
            return shared_from_this();
        };

        // MemoryAllocationInfo
        std::shared_ptr<tom::MemoryAllocation> MemoryAllocator::allocateMemory(const std::shared_ptr<tom::MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& memAllocInfo = {}) {
            auto self = std::dynamic_pointer_cast<MemoryAllocation>(allocation); if (!self->getData()) { self->getData() = std::make_shared<MemoryAllocationData>(); };
            auto allocator = std::dynamic_pointer_cast<MemoryAllocator>(shared_from_this());
            auto device = std::dynamic_pointer_cast<Device>(this->getDevice());
            auto data = std::dynamic_pointer_cast<MemoryAllocationData>(self->getData());

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
                    vk::throwResultException(vk::Result(vmaAllocateMemoryForBuffer((const VmaAllocator&)allocator->getData()->allocator, vkInfo->buffer, &allocCreateInfo, &((VmaAllocation&)data->allocation), &allocInfo)), "VMA buffer allocation failed...");
                } else 
                if (vkInfo->image) {
                    vk::throwResultException(vk::Result(vmaAllocateMemoryForImage((const VmaAllocator&)allocator->getData()->allocator, vkInfo->image, &allocCreateInfo, &((VmaAllocation&)data->allocation), &allocInfo)), "VMA image allocation failed...");
                } else 
                {
                    vk::throwResultException(vk::Result(vmaAllocateMemory((const VmaAllocator&)allocator->getData()->allocator, (VkMemoryRequirements*)&vkInfo->requirements.memoryRequirements, &allocCreateInfo, &((VmaAllocation&)data->allocation), &allocInfo)), "VMA memory allocation failed...");
                };

                // 
                self->deviceMemory = device->getDeviceMemoryObject(reinterpret_cast<DeviceMemoryKey&>(allocInfo.deviceMemory));
                data->memoryOffset = allocInfo.offset;
                data->allocation = data->allocation;
                data->mapped = allocInfo.pMappedData;
                //deviceMemory->getMapped() = allocInfo.pMappedData; // not sure...

                //
                data->destructor = [allocator = allocator->getData()->allocator, allocation = data->allocation]{
                    vmaFreeMemory((VmaAllocator&)allocator, (VmaAllocation&)allocation);
                };
            };

            return self;
        };

        //
        std::shared_ptr<tom::DeviceBuffer> MemoryAllocatorVma::allocateAndCreateBuffer(const std::shared_ptr<tom::DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& memAllocInfo = {}) {
            auto self = std::dynamic_pointer_cast<DeviceBuffer>(buffer); if (!self->getData()) { self->getData() = std::make_shared<MemoryAllocationData>(); };
            auto allocator = std::dynamic_pointer_cast<MemoryAllocator>(shared_from_this());
            auto device = std::dynamic_pointer_cast<Device>(this->getDevice());
            auto data = std::dynamic_pointer_cast<MemoryAllocationData>(self->getData());
            auto api = std::dynamic_pointer_cast<DeviceBufferData>(self->getApi());
            auto info = api->info.queueFamilyIndexCount ? api->info : vk::BufferCreateInfo(api->info).setQueueFamilyIndices(device->getData()->queueFamilyIndices);

            // 
            VmaAllocationInfo allocInfo = {};
            VmaAllocationCreateInfo allocCreateInfo = { .usage = reinterpret_cast<const VmaMemoryUsage&>(memAllocInfo.usage) };
            if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
            };

            // 
            vk::throwResultException(vk::Result(vmaCreateBuffer((const VmaAllocator&)allocator->getData()->allocator, (const VkBufferCreateInfo*)&info, &allocCreateInfo, (VkBuffer*)&api->buffer, &((VmaAllocation&)data->allocation), nullptr)), "VMA buffer allocation failed...");
            vmaGetAllocationInfo((const VmaAllocator&)allocator->getData()->allocator, ((VmaAllocation&)data->allocation), &allocInfo);
            device->setDeviceBufferObject(self);

            // wrap device memory
            self->deviceMemory = device->getDeviceMemoryObject(reinterpret_cast<DeviceMemoryKey&>(allocInfo.deviceMemory));
            data->memoryOffset = allocInfo.offset;
            data->mapped = allocInfo.pMappedData;
            data->destructor = [api, allocator = allocator->getData()->allocator, allocation = data->allocation](){
                vmaDestroyBuffer((VmaAllocator&)allocator, api->buffer, (VmaAllocation&)allocation); api->buffer = vk::Buffer{};
            };

            return self;
        };

        //
        std::shared_ptr<tom::DeviceImage> MemoryAllocatorVma::allocateAndCreateImage(const std::shared_ptr<tom::DeviceImage>& image, const tom::MemoryAllocationInfo& memAllocInfo = {}) {
            auto self = std::dynamic_pointer_cast<DeviceImage>(image); if (!self->getData()) { self->getData() = std::make_shared<MemoryAllocationData>(); };
            auto allocator = std::dynamic_pointer_cast<MemoryAllocator>(shared_from_this());
            auto device = std::dynamic_pointer_cast<Device>(this->getDevice());
            auto data = std::dynamic_pointer_cast<MemoryAllocationData>(self->getData());
            auto api = std::dynamic_pointer_cast<DeviceImageData>(self->getApi());
            auto info = api->info.queueFamilyIndexCount ? api->info : vk::ImageCreateInfo(api->info).setQueueFamilyIndices(device->getData()->queueFamilyIndices);

            // 
            VmaAllocationInfo allocInfo = {};
            VmaAllocationCreateInfo allocCreateInfo = { .usage = reinterpret_cast<const VmaMemoryUsage&>(memAllocInfo.usage) };
            if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
            };

            // 
            vk::throwResultException(vk::Result(vmaCreateImage((const VmaAllocator&)allocator->getData()->allocator, (const VkImageCreateInfo*)&info, &allocCreateInfo, (VkImage*)&api->image, &((VmaAllocation&)data->allocation), nullptr)), "VMA image allocation failed...");
            vmaGetAllocationInfo((const VmaAllocator&)allocator->getData()->allocator, ((VmaAllocation&)data->allocation), &allocInfo);

            // wrap device memory
            self->deviceMemory = device->getDeviceMemoryObject(reinterpret_cast<DeviceMemoryKey&>(allocInfo.deviceMemory));
            data->memoryOffset = allocInfo.offset;
            data->mapped = allocInfo.pMappedData;
            data->destructor = [api, allocator = allocator->getData()->allocator, allocation = data->allocation](){
                vmaDestroyImage((VmaAllocator&)allocator, api->image, (VmaAllocation&)allocation); api->image = vk::Image{};
            };

            return self;
        };
    };

};
