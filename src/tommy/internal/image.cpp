#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/image.hpp>


// 
namespace tom {

    // 
    std::shared_ptr<DeviceImage> DeviceImage::bindMemory(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
        if (memoryAllocation) {
            this->memoryAllocation = memoryAllocation;
            this->getDevice()->getDevice().bindImageMemory2(vk::BindImageMemoryInfo{
                .image = this->image,
                .memory = this->memoryAllocation->getDeviceMemory()->getMemory(),
                .memoryOffset = this->memoryAllocation->getOffset()
            });
        };
        return shared_from_this();
    };

    // 
    std::shared_ptr<DeviceImage> DeviceImage::create(const vk::ImageCreateInfo& info = {}, const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
        this->image = this->getDevice()->getDevice().createImage( this->info = info.queueFamilyIndexCount ? vk::ImageCreateInfo(info) : vk::ImageCreateInfo(info).setQueueFamilyIndices(this->getDevice()->getQueueFamilyIndices()) );
        this->bindMemory(memoryAllocation);
        this->layoutHistory.clear();
        this->layoutHistory.push_back(info.initialLayout);
        return shared_from_this();
    };


    // 
    std::shared_ptr<ImageView> ImageView::createImageView(const vk::ImageViewCreateInfo& info = {}) {
        this->info.imageView = this->deviceImage->getDevice()->getDevice().createImageView(this->imageViewInfo = info);
        return shared_from_this();
    };

    // 
    std::shared_ptr<ImageView> ImageView::createSampler(const vk::SamplerCreateInfo& info = {}) {
        this->info.sampler = this->deviceImage->getDevice()->getDevice().createSampler(this->samplerInfo = info);
        return shared_from_this();
    };

};
