#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/image.hpp>


// 
namespace tom {
    namespace vktm {
        // 
        std::shared_ptr<DeviceImage> DeviceImage::bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) {
            if (memoryAllocation) {
                this->memoryAllocation = memoryAllocation;
                this->getDevice()->getData()->device.bindImageMemory2(vk::BindImageMemoryInfo{
                    .image = data->image,
                    .memory = this->memoryAllocation->getDeviceMemory()->getMemory(),
                    .memoryOffset = this->memoryAllocation->getOffset()
                });
            };
            return shared_from_this();
        };

        // 
        std::shared_ptr<DeviceImage> DeviceImage::create(const vk::ImageCreateInfo& info = {}, const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) {
            data->image = this->getDevice()->getData()->device.createImage( data->info = info.queueFamilyIndexCount ? vk::ImageCreateInfo(info) : vk::ImageCreateInfo(info).setQueueFamilyIndices(this->getDevice()->getQueueFamilyIndices()) );
            this->bindMemory(memoryAllocation);
            //this->layoutHistory.clear();
            //this->layoutHistory.push_back(info.initialLayout);
            return shared_from_this();
        };


        // 
        std::shared_ptr<ImageView> ImageView::createImageView(const vk::ImageViewCreateInfo& info = {}) {
            data->info.imageView = this->deviceImage->getDevice()->getData()->device.createImageView((data->imageViewInfo = info).setImage(this->deviceImage->getData()->image));
            data->layoutHistory.clear();
            data->layoutHistory.push_back(this->deviceImage->getData()->info.initialLayout);
            return shared_from_this();
        };

        // 
        std::shared_ptr<ImageView> ImageView::createSampler(const vk::SamplerCreateInfo& info = {}) {
            data->info.sampler = this->deviceImage->getDevice()->getData()->device.createSampler(data->samplerInfo = info);
            return shared_from_this();
        };

    };

};
