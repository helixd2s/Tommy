#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>
#include <tommy/internal/vulkan/buffer.hpp>
#include <tommy/internal/vulkan/image.hpp>

// 
namespace tom {

    // 
    namespace vulkan {
        // 
        std::shared_ptr<tom::MemoryAllocator>& Device::createAllocatorVk() {
            auto allocator = std::make_shared<MemoryAllocator>(shared_from_this());
            data->allocators.push_back(allocator);
            return data->allocators.back();
        };

        // 
        std::shared_ptr<tom::MemoryAllocation> MemoryAllocator::allocateMemory(const std::shared_ptr<tom::MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& allocInfo = {}) {
            auto self = std::dynamic_pointer_cast<MemoryAllocation>(allocation); if (!self->getData()) { self->getData() = std::make_shared<MemoryAllocationData>(); };
            auto allocator = std::dynamic_pointer_cast<MemoryAllocator>(shared_from_this());
            auto device = std::dynamic_pointer_cast<Device>(this->getDevice());
            auto data = std::dynamic_pointer_cast<MemoryAllocationData>(self->getData());

            // 
            auto* vkInfo = (tom::vulkan::MemoryAllocationInfo*)(allocInfo.apiInfo);

            // TODO: smart allocation 
            data->memoryOffset = 0ull;
            self->deviceMemory = device->allocateMemoryObject(allocator, vk::StructureChain<vk::MemoryAllocateInfo, vk::MemoryDedicatedAllocateInfoKHR, vk::ExternalMemoryBufferCreateInfo>{
                vk::MemoryAllocateInfo{
                    .allocationSize = vkInfo->requirements.memoryRequirements.size,
                    .memoryTypeIndex = std::dynamic_pointer_cast<PhysicalDevice>(device->getPhysicalDevice())->getMemoryType(vkInfo->requirements.memoryRequirements.memoryTypeBits),
                },
                vk::MemoryDedicatedAllocateInfoKHR{ .image = vkInfo->image, .buffer = vkInfo->buffer },
                vk::ExternalMemoryBufferCreateInfo{
    #ifdef _WIN32
                    .handleTypes = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32
    #else
                    .handleTypes = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd
    #endif
                }
            }.get<vk::MemoryAllocateInfo>());

            // 
            return self;
        };

        // 
        std::shared_ptr<tom::DeviceBuffer> MemoryAllocator::allocateBuffer(const std::shared_ptr<tom::DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {}) {
            auto self = std::dynamic_pointer_cast<DeviceBuffer>(buffer);
            auto allocator = std::dynamic_pointer_cast<MemoryAllocator>(shared_from_this());
            auto device = std::dynamic_pointer_cast<Device>(this->getDevice());
            auto data = std::dynamic_pointer_cast<MemoryAllocationData>(self->getData());
            auto api = std::dynamic_pointer_cast<DeviceBufferData>(self->getApi());

            if (api->buffer) { // 
                auto memoryRequirements = vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirementsKHR>{ vk::MemoryRequirements2{  }, vk::MemoryDedicatedRequirementsKHR{} };
                std::dynamic_pointer_cast<DeviceData>(device->getData())->device.getBufferMemoryRequirements2(vk::BufferMemoryRequirementsInfo2{ .buffer = api->buffer }, memoryRequirements);
                allocator->allocateMemory(self, tom::MemoryAllocationInfo(allocInfo).withVulkan(MemoryAllocationInfo{.buffer = api->buffer, .requirements = memoryRequirements.get<vk::MemoryRequirements2>()}));
            };

            return self;
        };

        // 
        std::shared_ptr<tom::DeviceImage> MemoryAllocator::allocateImage(const std::shared_ptr<tom::DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {}) {
            auto self = std::dynamic_pointer_cast<DeviceImage>(image);
            auto allocator = std::dynamic_pointer_cast<MemoryAllocator>(shared_from_this());
            auto device = std::dynamic_pointer_cast<Device>(this->getDevice());
            auto data = std::dynamic_pointer_cast<MemoryAllocationData>(self->getData());
            auto api = std::dynamic_pointer_cast<DeviceImageData>(self->getApi());

            if (api->image) { // 
                auto memoryRequirements = vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirementsKHR>{ vk::MemoryRequirements2{  }, vk::MemoryDedicatedRequirementsKHR{} };
                std::dynamic_pointer_cast<DeviceData>(device->getData())->device.getImageMemoryRequirements2(vk::ImageMemoryRequirementsInfo2{ .image = api->image }, memoryRequirements);
                allocator->allocateMemory(self, tom::MemoryAllocationInfo(allocInfo).withVulkan(MemoryAllocationInfo{.image = api->image, .requirements = memoryRequirements.get<vk::MemoryRequirements2>()}));
            };

            return self;
        };

        // 
        std::shared_ptr<tom::DeviceBuffer> MemoryAllocator::allocateAndCreateBuffer(const std::shared_ptr<tom::DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {}) {
            return this->allocateBuffer(std::dynamic_pointer_cast<tom::DeviceBuffer>(buffer->create()), allocInfo);
        };

        // 
        std::shared_ptr<tom::DeviceImage> MemoryAllocator::allocateAndCreateImage(const std::shared_ptr<tom::DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {}) {
            return this->allocateImage(std::dynamic_pointer_cast<tom::DeviceImage>(image->create()), allocInfo);
        };
    };

};
