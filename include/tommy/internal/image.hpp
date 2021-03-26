#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {

    // 
    class DeviceImage: public MemoryAllocation {
    protected: friend MemoryAllocator; friend ImageView;// 
        std::shared_ptr<DeviceImageBase> api = {};
        std::function<void()> destructor = {};
        void* allocation = nullptr;

    public: // 
        DeviceImage(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const vk::DeviceSize& memoryOffset = 0ull, const std::shared_ptr<MemoryAllocationBase>& data = {}, const std::shared_ptr<DeviceImageBase>& api = {}): MemoryAllocation(deviceMemory, memoryOffset, data), api(api) {
        };

        DeviceImage(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}, const std::shared_ptr<DeviceImageBase>& api = {}): MemoryAllocation(deviceMemory, data), api(api) {
        };

        DeviceImage(const std::shared_ptr<Device>& device = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}, const std::shared_ptr<DeviceImageBase>& api = {}) : MemoryAllocation(device, data), api(api) {
        };

        // 
        virtual std::shared_ptr<MemoryAllocation> bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) override;
        virtual std::shared_ptr<MemoryAllocation> create(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) override;

        // 
        virtual inline std::shared_ptr<MemoryAllocation> getMemoryAllocation() { return shared_from_this(); };
        virtual inline std::shared_ptr<DeviceImageBase> getApi() { return api; };

        // 
        virtual inline std::shared_ptr<DeviceImageBase> getApi() const { return api; };
    };

    // 
    class ImageView : public std::enable_shared_from_this<ImageView> { protected: 
    protected:
        friend DeviceImage;
        std::shared_ptr<DeviceImage> deviceImage = {};
        std::shared_ptr<ImageViewBase> data = {};
        ImageViewKey key = {};

    public: // 
        ImageView(const std::shared_ptr<DeviceImage>& deviceImage, const std::shared_ptr<ImageViewBase>& data): deviceImage(deviceImage), data(data) {
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
        virtual inline ImageViewKey& getKey() { return key; };

        // 
        virtual inline const std::shared_ptr<DeviceImage>& getDeviceImage() const { return deviceImage; };
        virtual inline const ImageViewKey& getKey() const { return key; };
    };

};
