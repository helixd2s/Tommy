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

        // 
        class DeviceImage: public std::enable_shared_from_this<DeviceImage> {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma; friend ImageView;// 
            std::weak_ptr<Device> device = {};
            std::shared_ptr<MemoryAllocation> memoryAllocation = {};
            std::shared_ptr<DeviceImageData> data = {};
            std::function<void()> destructor = {};
            void* allocation = nullptr;

            

        public: // 
            // legacy
            DeviceImage(const std::shared_ptr<Device>& device, const vk::Image& image = {}): device(device) {
                data = std::make_shared<DeviceImageData>();
                data->image = image;
            };

            // 
            ~DeviceImage() {
                auto device = this->getDevice()->getData();
                if (this->destructor) { 
                    this->destructor();
                };
                if (data->image) {
                    device->device.bindImageMemory2(vk::BindImageMemoryInfo{ .image = data->image, .memory = {}, .memoryOffset = 0ull });
                    device->device.destroyImage(data->image);
                    data->image = vk::Image{};
                };
                this->destructor = {};
            };

            // 
            virtual std::shared_ptr<DeviceImage> bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});
            virtual std::shared_ptr<DeviceImage> create(const vk::ImageCreateInfo& info = {}, const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});

            // 
            virtual inline std::shared_ptr<MemoryAllocation>& getMemoryAllocation() { return memoryAllocation; };
            virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
            virtual inline std::shared_ptr<DeviceImageData> getData() { return data; };

            // 
            virtual inline const std::shared_ptr<MemoryAllocation>& getMemoryAllocation() const { return memoryAllocation; };
            virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
            virtual inline std::shared_ptr<DeviceImageData> getData() const { return data; };
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
