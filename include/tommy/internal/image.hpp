#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {

    // 
    class DeviceImage: public std::enable_shared_from_this<DeviceImage> {
    protected: friend MemoryAllocator; friend MemoryAllocatorVma; // 
        std::shared_ptr<tom::Device> device = {};
        std::shared_ptr<tom::MemoryAllocation> memoryAllocation = {};
        std::function<void()> destructor = {};

        //
        vk::Image image = {};
        void* allocation = nullptr;

        //
        vk::ImageCreateInfo info = {};

        //
        std::vector<vk::ImageLayout> layoutHistory = {};

    public: // 
        DeviceImage(const std::shared_ptr<tom::Device>& device, const vk::Image& image = {}): device(device), image(image) {
            
        };

        // 
        ~DeviceImage() {
            auto& device = this->device->getDevice();
            if (this->destructor) { 
                this->destructor();
            };
            if (this->image) {
                device.bindImageMemory2(vk::BindImageMemoryInfo{ .image = this->image, .memory = {}, .memoryOffset = 0ull });
                device.destroyImage(this->image);
                this->image = vk::Image{};
            };
            this->destructor = {};
        };

        // 
        virtual std::shared_ptr<DeviceImage> bindMemory(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {});
        virtual std::shared_ptr<DeviceImage> create(const vk::ImageCreateInfo& info = {}, const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {});

        // 
        virtual inline std::vector<vk::ImageLayout>& getLayoutHistory() { return layoutHistory; };
        virtual inline std::shared_ptr<tom::MemoryAllocation>& getMemoryAllocation() { return memoryAllocation; };
        virtual inline std::shared_ptr<tom::Device>& getDevice() { return device; };
        virtual inline vk::Image& getImage() { return image; };

        // 
        virtual inline const std::vector<vk::ImageLayout>& getLayoutHistory() const { return layoutHistory; };
        virtual inline const std::shared_ptr<tom::MemoryAllocation>& getMemoryAllocation() const { return memoryAllocation; };
        virtual inline const std::shared_ptr<tom::Device>& getDevice() const { return device; };
        virtual inline const vk::Image& getImage() const { return image; };
    };

    //
    class ImageView : public std::enable_shared_from_this<ImageView> { protected: 
    protected:
        std::shared_ptr<tom::DeviceImage> deviceImage = {};

        // 
        vk::ImageView imageView = {};
        vk::ImageViewCreateInfo info = {};

    public: // 
        ImageView(const std::shared_ptr<tom::DeviceImage>& deviceImage, const vk::ImageView& imageView = {}): deviceImage(deviceImage), imageView(imageView) {
            this->constructor();
        };

        // 
        virtual std::shared_ptr<ImageView> constructor() {
            return shared_from_this();
        };

        // 
        virtual std::shared_ptr<ImageView> create(const vk::ImageViewCreateInfo& info = {});

        // 
        virtual inline std::shared_ptr<tom::DeviceImage>& getDeviceImage() { return deviceImage; };

        // 
        virtual inline const std::shared_ptr<tom::DeviceImage>& getDeviceImage() const { return deviceImage; };
    };

};
