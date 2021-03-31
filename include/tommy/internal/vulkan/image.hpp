#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "../image.hpp"

#include "./core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {

    // 
    namespace vulkan {

        // 
        class DeviceImageData : public DeviceImageBase { public: 
            vk::Image image = {};
            vk::ImageCreateInfo info = {};

            // 
            static std::shared_ptr<DeviceImageData> makeShared(const vk::Image& image = {}) {
                std::shared_ptr<DeviceImageData> data = {};
                data->image = image;
                return data;
            };
        };

        //
        class ImageViewData : public ImageViewBase { public: 
            vk::DescriptorImageInfo info = {};
            vk::ImageViewCreateInfo imageViewInfo = {};
            vk::SamplerCreateInfo samplerInfo = {};
            std::vector<vk::ImageLayout> layoutHistory = {};

            // 
            static std::shared_ptr<ImageViewData> makeShared(const vk::DescriptorImageInfo& info = {}) {
                std::shared_ptr<ImageViewData> data = {};
                data->info = info;
                return data;
            };
        };

        // BASED ON ALLOCATION!!!
        class DeviceImage: public tom::DeviceImage {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma; friend ImageView;// 
            virtual inline std::shared_ptr<DeviceImageData> getApiTyped() { return std::dynamic_pointer_cast<DeviceImageData>(this->api); };
            virtual inline std::shared_ptr<DeviceImageData> getApiTyped() const { return std::dynamic_pointer_cast<DeviceImageData>(this->api); };

        public: // 
            // 
            virtual std::shared_ptr<tom::MemoryAllocation> bindMemory(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) override;
            virtual std::shared_ptr<tom::MemoryAllocation> create(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) override;

            // 
            virtual inline DeviceImageKey getKey() override { return reinterpret_cast<DeviceImageKey&>(std::dynamic_pointer_cast<DeviceImageData>(this->getApi())->image); };

            // 
            virtual inline DeviceImageKey getKey() const override { return reinterpret_cast<DeviceImageKey&>(std::dynamic_pointer_cast<DeviceImageData>(this->getApi())->image); };

        };

        // 
        class ImageView : public tom::ImageView { protected: 
        protected: friend DeviceImage;
            virtual inline std::shared_ptr<ImageViewData> getDataTyped() { return std::dynamic_pointer_cast<ImageViewData>(this->data); };
            virtual inline std::shared_ptr<ImageViewData> getDataTyped() const { return std::dynamic_pointer_cast<ImageViewData>(this->data); };

        public: // 
            ImageView(const std::shared_ptr<DeviceImage>& deviceImage, const std::shared_ptr<ImageViewData>& data = {}): tom::ImageView(deviceImage, data)
            { this->constructor(); };

            // 
            ImageView(const std::shared_ptr<Device>& device, const std::shared_ptr<ImageViewData>& data = {}): tom::ImageView(std::make_shared<DeviceImage>(device), data)
            { this->constructor(); };

            // 
            virtual std::shared_ptr<tom::ImageView> constructor() override {
                if (!this->data) { this->data = std::make_shared<ImageViewData>(); };
                return shared_from_this();
            };


            // 
            virtual std::shared_ptr<tom::ImageView> createImageView(const vk::ImageViewCreateInfo& info = {});
            virtual std::shared_ptr<tom::ImageView> createSampler(const vk::SamplerCreateInfo& info = {});

            // 
            virtual inline vk::DescriptorImageInfo& getInfo() { auto data = getDataTyped(); data->info.imageLayout = data->layoutHistory.back(); return data->info; };

            // 
            virtual inline vk::DescriptorImageInfo getInfo() const { auto data = getDataTyped(); return vk::DescriptorImageInfo{ data->info.sampler, data->info.imageView, data->layoutHistory.back() }; }

            // 
            virtual inline ImageViewKey getKey() override { return std::dynamic_pointer_cast<ImageViewData>(this->getData())->key; };

            // 
            virtual inline ImageViewKey getKey() const override { return std::dynamic_pointer_cast<ImageViewData>(this->getData())->key; };
        };

    };

};
