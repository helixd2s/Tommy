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
            this->device->getDevice().bindImageMemory2(vk::BindImageMemoryInfo{
                .image = this->image,
                .memory = this->memoryAllocation->getDeviceMemory()->getMemory(),
                .memoryOffset = this->memoryAllocation->getOffset()
            });
        };
        return shared_from_this();
    };

    // 
    std::shared_ptr<DeviceImage> DeviceImage::create(const vk::ImageCreateInfo& info = {}, const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
        this->image = this->device->getDevice().createImage( this->info = info.queueFamilyIndexCount ? vk::ImageCreateInfo(info) : vk::ImageCreateInfo(info).setQueueFamilyIndices(this->device->getQueueFamilyIndices()) );
        this->bindMemory(memoryAllocation);
        this->layoutHistory.clear();
        this->layoutHistory.push_back(info.initialLayout);
        return shared_from_this();
    };


    // 
    std::shared_ptr<ImageView> ImageView::create(const vk::ImageViewCreateInfo& info = {}) {
        this->imageView = this->deviceImage->getDevice()->getDevice().createImageView(this->info = info);
        return shared_from_this();
    };

};
