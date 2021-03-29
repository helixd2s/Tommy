#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {

    // 
    class DeviceImage: public MemoryAllocation {
    protected: friend MemoryAllocator; friend ImageView;// 
        std::shared_ptr<DeviceImageBase> api = {};

    public: // 
        DeviceImage(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}, const std::shared_ptr<DeviceImageBase>& api = {}): MemoryAllocation(deviceMemory, data), api(api) 
        {};

        // 
        DeviceImage(const std::shared_ptr<Device>& device = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}, const std::shared_ptr<DeviceImageBase>& api = {}) : MemoryAllocation(device, data), api(api) 
        {};

        // 
        DeviceImage(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const std::shared_ptr<DeviceImageBase>& api = {}): MemoryAllocation(deviceMemory), api(api) 
        {};

        // 
        DeviceImage(const std::shared_ptr<Device>& device = {}, const std::shared_ptr<DeviceImageBase>& api = {}) : MemoryAllocation(device), api(api) 
        {};

        // 
        virtual std::shared_ptr<MemoryAllocation> bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) override;
        virtual std::shared_ptr<MemoryAllocation> create(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {}) override;

        // 
        virtual inline std::shared_ptr<MemoryAllocation> getMemoryAllocation() { return shared_from_this(); };
        virtual inline std::shared_ptr<DeviceImageBase>& getApi() { return api; };

        // 
        virtual inline const std::shared_ptr<DeviceImageBase>& getApi() const { return api; };
    };

    // 
    class ImageView : public std::enable_shared_from_this<ImageView> { protected: 
    protected: friend DeviceImage;  // 
        std::shared_ptr<DeviceImage> deviceImage = {};
        std::shared_ptr<ImageViewBase> data = {};

    public: // 
        ImageView(const std::shared_ptr<DeviceImage>& deviceImage, const std::shared_ptr<ImageViewBase>& data = {}): deviceImage(deviceImage), data(data) 
        { this->constructor(); };

        // 
        ImageView(const std::shared_ptr<Device>& device, const std::shared_ptr<ImageViewBase>& data = {}): deviceImage(std::make_shared<DeviceImage>(device)), data(data) 
        { this->constructor(); };

        // 
        virtual std::shared_ptr<ImageView> constructor() {
            if (!this->data) { this->data = std::make_shared<ImageViewBase>(); };
            return shared_from_this();
        };

        // 
        virtual inline std::shared_ptr<DeviceImage>& getDeviceImage() { return deviceImage; };
        virtual inline std::shared_ptr<ImageViewBase>& getData() { return data; };

        // 
        virtual inline const std::shared_ptr<DeviceImage>& getDeviceImage() const { return deviceImage; };
        virtual inline const std::shared_ptr<ImageViewBase>& getData() const { return data; };
    };

};
