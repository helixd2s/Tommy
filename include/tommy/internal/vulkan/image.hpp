#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {

    namespace vulkan {

        // 
        class DeviceImageData : public DeviceImageBase { public: 
            vk::Image image = {};
            vk::ImageCreateInfo info = {};
        };

        //
        class ImageViewData : public ImageViewBase { public: 
            vk::DescriptorImageInfo info = {};
            vk::ImageViewCreateInfo imageViewInfo = {};
            vk::SamplerCreateInfo samplerInfo = {};
            std::vector<vk::ImageLayout> layoutHistory = {};
        };

        // BASED ON ALLOCATION!!!
        class DeviceImage: public MemoryAllocation {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma; friend ImageView;// 
            std::shared_ptr<DeviceImageData> api = {};
            std::function<void()> destructor = {};
            void* allocation = nullptr;

        public: // 
            // legacy
            DeviceImage(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const vk::DeviceSize& memoryOffset = 0ull, const vk::Image& image = {}): MemoryAllocation(deviceMemory, memoryOffset) {
                api = std::make_shared<DeviceImageData>();
                api->image = image;
            };

            // legacy
            DeviceImage(const vk::Image& image = {}) {
                api = std::make_shared<DeviceImageData>();
                api->image = image;
            };

            // 
            ~DeviceImage() {
                if (this->destructor) { 
                    this->destructor();
                };
                this->destructor = {};

                if (this->data) {
                    auto device = this->getDevice()->getData();
                    if (api->image) {
                        device->device.bindImageMemory2(vk::BindImageMemoryInfo{ .image = api->image, .memory = {}, .memoryOffset = 0ull });
                        device->device.destroyImage(api->image);
                        api->image = vk::Image{};
                    };
                };
            };

            // 
            virtual std::shared_ptr<DeviceImage> bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});
            virtual std::shared_ptr<DeviceImage> create(const vk::ImageCreateInfo& info = {}, const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});

            // 
            virtual inline std::shared_ptr<MemoryAllocation> getMemoryAllocation() { return shared_from_this(); };
            virtual inline std::shared_ptr<DeviceImageData> getApi() { return api; };

            // 
            virtual inline std::shared_ptr<DeviceImageData> getApi() const { return api; };
        };

        //
        class ImageView : public std::enable_shared_from_this<ImageView> { protected: 
        protected:
            friend DeviceImage;
            std::shared_ptr<DeviceImage> deviceImage = {};
            std::shared_ptr<ImageViewData> data = {};
            ImageViewKey key = {};

        public: // 
            ImageView(const std::shared_ptr<DeviceImage>& deviceImage, const vk::DescriptorImageInfo& info = {}): deviceImage(deviceImage) {
                data = std::make_shared<DeviceImageData>();
                data->info = info;
                this->constructor();
            };

            // 
            virtual std::shared_ptr<ImageView> constructor() {
                return shared_from_this();
            };

            // 
            virtual std::shared_ptr<ImageView> createImageView(const vk::ImageViewCreateInfo& info = {});
            virtual std::shared_ptr<ImageView> createSampler(const vk::SamplerCreateInfo& info = {});

            // 
            virtual inline std::shared_ptr<DeviceImage>& getDeviceImage() { return deviceImage; };
            virtual inline vk::DescriptorImageInfo& getInfo() { data->info.imageLayout = data->layoutHistory.back(); return data->info; };
            virtual inline ImageViewKey& getKey() { return key; };

            // 
            virtual inline const std::shared_ptr<DeviceImage>& getDeviceImage() const { return deviceImage; };
            virtual inline vk::DescriptorImageInfo getInfo() const { return vk::DescriptorImageInfo{ data->info.sampler, data->info.imageView, data->layoutHistory.back() }; };
            virtual inline const ImageViewKey& getKey() const { return key; };
        };

    };

};
