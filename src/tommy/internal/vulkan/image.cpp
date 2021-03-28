#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>
#include <tommy/internal/vulkan/image.hpp>


// 
namespace tom {

    // 
    namespace vulkan {

        // 
        std::shared_ptr<tom::MemoryAllocation> DeviceImage::bindMemory(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
            auto api = this->getApiTyped();
            if (memoryAllocation) {
                this->data = memoryAllocation->getData();
                this->deviceMemory = memoryAllocation->getDeviceMemory();
            };
            if (this->data) {
                std::dynamic_pointer_cast<DeviceData>(this->getDevice()->getData())->device.bindImageMemory2(vk::BindImageMemoryInfo{
                    .image = api->image,
                    .memory = std::dynamic_pointer_cast<DeviceMemoryApi>(deviceMemory->getApi())->memory,
                    .memoryOffset = data->memoryOffset
                });
            };
            return std::dynamic_pointer_cast<tom::MemoryAllocation>(shared_from_this());
        };

        // 
        std::shared_ptr<tom::MemoryAllocation> DeviceImage::create(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
            auto self = std::dynamic_pointer_cast<DeviceImage>(shared_from_this());
            auto device = this->getDevice();
            auto api = this->getApiTyped();
            auto info = api->info.queueFamilyIndexCount ? api->info : vk::ImageCreateInfo(api->info).setQueueFamilyIndices(device->getData()->queueFamilyIndices);
            api->image = std::dynamic_pointer_cast<DeviceData>(device->getData())->device.createImage( info );
            this->bindMemory(memoryAllocation);
            api->destructor = [image = api->image, device = std::dynamic_pointer_cast<DeviceData>(device->getData())->device]() { if (image) {
                device.bindImageMemory2(vk::BindImageMemoryInfo{ .image = image, .memory = {}, .memoryOffset = 0ull });
                device.destroyImage(image);
            }; };
            return std::dynamic_pointer_cast<tom::MemoryAllocation>(shared_from_this());
        };


        // 
        std::shared_ptr<tom::ImageView> ImageView::createImageView(const vk::ImageViewCreateInfo& info = {}) {
            auto data = this->getDataTyped();
            data->info.imageView = std::dynamic_pointer_cast<DeviceData>(this->deviceImage->getDevice()->getData())->device.createImageView((data->imageViewInfo = info).setImage(std::dynamic_pointer_cast<DeviceImageData>(this->deviceImage->getApi())->image));
            data->layoutHistory.clear();
            data->layoutHistory.push_back(std::dynamic_pointer_cast<DeviceImageData>(this->deviceImage->getApi())->info.initialLayout);
            return std::dynamic_pointer_cast<tom::ImageView>(shared_from_this());
        };

        // 
        std::shared_ptr<tom::ImageView> ImageView::createSampler(const vk::SamplerCreateInfo& info = {}) {
            auto data = this->getDataTyped();
            data->info.sampler = std::dynamic_pointer_cast<DeviceData>(this->deviceImage->getDevice()->getData())->device.createSampler(data->samplerInfo = info);
            return std::dynamic_pointer_cast<tom::ImageView>(shared_from_this());
        };

    };

};
