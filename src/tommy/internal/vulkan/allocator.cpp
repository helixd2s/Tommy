#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>
#include <tommy/internal/vulkan/buffer.hpp>
#include <tommy/internal/vulkan/image.hpp>

// 
namespace tom {
    namespace vulkan {
        // 
        std::shared_ptr<MemoryAllocator>& Device::createAllocator() {
            if (!this->allocator) {
                this->allocator = std::make_shared<MemoryAllocator>(shared_from_this());
            };
            return this->allocator;
        };

        // MemoryAllocationInfo
        std::shared_ptr<MemoryAllocation> MemoryAllocator::allocateMemory(const std::shared_ptr<MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& allocInfo = {}) {
            auto self = allocation;
            auto allocator = shared_from_this();
            auto device = this->device.lock();
            auto data = self->getData();

            //
            auto* vkInfo = (tom::vulkan::MemoryAllocationInfo*)(allocInfo.apiInfo);

            // TODO: smart allocation 
            data->memoryOffset = 0ull;
            self->deviceMemory = device->allocateMemoryObject(allocator, vk::StructureChain<vk::MemoryAllocateInfo, vk::MemoryDedicatedAllocateInfoKHR, vk::ExternalMemoryBufferCreateInfo>{
                vk::MemoryAllocateInfo{
                    .allocationSize = vkInfo->requirements.memoryRequirements.size,
                    .memoryTypeIndex = device->getPhysicalDevice()->getMemoryType(vkInfo->requirements.memoryRequirements.memoryTypeBits),
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
        std::shared_ptr<DeviceBuffer> MemoryAllocator::allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {}) {
            auto self = buffer;
            auto allocator = shared_from_this();
            auto device = this->device.lock();
            auto data = self->getData();

            if (data->buffer) { // 
                auto memoryRequirements = vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirementsKHR>{ vk::MemoryRequirements2{  }, vk::MemoryDedicatedRequirementsKHR{} };
                device->getData()->device.getBufferMemoryRequirements2(vk::BufferMemoryRequirementsInfo2{ .buffer = data->buffer }, memoryRequirements);
                self->bindMemory(allocator->allocateMemory(std::make_shared<MemoryAllocation>(device), tom::MemoryAllocationInfo(allocInfo).withVulkan(MemoryAllocationInfo{.buffer = data->buffer, .requirements = memoryRequirements.get<vk::MemoryRequirements2>()})));
            };

            return self;
        };

        // 
        std::shared_ptr<DeviceImage> MemoryAllocator::allocateImage(const std::shared_ptr<DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {}) {
            auto self = image;
            auto allocator = shared_from_this();
            auto device = this->device.lock();
            auto data = self->getData();

            if (data->image) { // 
                auto memoryRequirements = vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirementsKHR>{ vk::MemoryRequirements2{  }, vk::MemoryDedicatedRequirementsKHR{} };
                device->getData()->device.getImageMemoryRequirements2(vk::ImageMemoryRequirementsInfo2{ .image = data->image }, memoryRequirements);
                self->bindMemory(allocator->allocateMemory(std::make_shared<MemoryAllocation>(device), tom::MemoryAllocationInfo(allocInfo).withVulkan(MemoryAllocationInfo{.image = data->image, .requirements = memoryRequirements.get<vk::MemoryRequirements2>()})));
            };

            return self;
        };

        // 
        std::shared_ptr<DeviceBuffer> MemoryAllocator::allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const tom::MemoryAllocationInfo& allocInfo = {}) {
            return this->allocateBuffer(buffer->create(info), allocInfo);
        };

        // 
        std::shared_ptr<DeviceImage> MemoryAllocator::allocateAndCreateImage(const std::shared_ptr<DeviceImage>& image, const vk::ImageCreateInfo& info = {}, const tom::MemoryAllocationInfo& allocInfo = {}) {
            return this->allocateImage(image->create(info), allocInfo);
        };
    };

};
