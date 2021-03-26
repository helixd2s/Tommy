#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>
#include <tommy/internal/vulkan/image.hpp>


// 
namespace tom {
    namespace vulkan {

        // 
        std::shared_ptr<DeviceImage> DeviceImage::bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) {
            if (memoryAllocation) {
                this->data = memoryAllocation->getData();
                this->memoryOffset = memoryAllocation->getMemoryOffset();
                this->mapped = memoryAllocation->getMapped();
                this->allocation = memoryAllocation->getAllocation();
            };
            if (this->data) {
                this->getDevice()->getData()->device.bindImageMemory2(vk::BindImageMemoryInfo{
                    .image = api->image,
                    .memory = deviceMemory->getData()->memory,
                    .memoryOffset = this->memoryOffset
                });
            };
            return std::dynamic_pointer_cast<DeviceImage>(shared_from_this());
        };

        // 
        std::shared_ptr<DeviceImage> DeviceImage::create(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) {
            auto self = std::dynamic_pointer_cast<DeviceImage>(shared_from_this());
            auto device = this->getDevice();
            api->image = device->getData()->device.createImage( api->info.queueFamilyIndexCount ? api->info : vk::ImageCreateInfo(api->info).setQueueFamilyIndices(device->getQueueFamilyIndices()) );
            this->bindMemory(memoryAllocation);
            //this->layoutHistory.clear();
            //this->layoutHistory.push_back(info.initialLayout);
            return self;
        };


        // 
        std::shared_ptr<ImageView> ImageView::createImageView(const vk::ImageViewCreateInfo& info = {}) {
            data->info.imageView = this->deviceImage->getDevice()->getData()->device.createImageView((data->imageViewInfo = info).setImage(this->deviceImage->getApi()->image));
            data->layoutHistory.clear();
            data->layoutHistory.push_back(this->deviceImage->getApi()->info.initialLayout);
            return shared_from_this();
        };

        // 
        std::shared_ptr<ImageView> ImageView::createSampler(const vk::SamplerCreateInfo& info = {}) {
            data->info.sampler = this->deviceImage->getDevice()->getData()->device.createSampler(data->samplerInfo = info);
            return shared_from_this();
        };

    };

};
