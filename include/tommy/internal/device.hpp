#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./instance.hpp"

// 
namespace tom {

    // 
    class Queue: public std::enable_shared_from_this<Queue> {
    protected:  //
        std::weak_ptr<Device> device = {};
        std::shared_ptr<QueueBase> data = {};

    public: 
        // legacy
        Queue(const std::shared_ptr<Device>& device, const std::shared_ptr<QueueBase>& data = {}) : device(device), data(data) 
        { this->constructor(); };

        // 
        virtual std::shared_ptr<Queue> constructor() {
            if (!this->data) { this->data = std::make_shared<QueueBase>(); };
            return shared_from_this();
        };

        // 
        virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
        virtual inline const std::shared_ptr<QueueBase>& getData() const { return data; };

        // 
        virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
        virtual inline std::shared_ptr<QueueBase>& getData() { return data; };

        // 
        virtual std::future<ApiResult> copyDeviceBuffers(const std::shared_ptr<BufferAllocation>& src, const std::shared_ptr<BufferAllocation>& dst);
        virtual std::future<ApiResult> copyDeviceBufferToImage(const std::shared_ptr<BufferAllocation>& src, const std::shared_ptr<ImageView>& dst, const ImageRegion& dstRegion = {});
        virtual std::future<ApiResult> copyDeviceImageToBuffer(const std::shared_ptr<ImageView>& src, const std::shared_ptr<BufferAllocation>& dst, const ImageRegion& srcRegion = {});
    };


    // 
    class Device: public std::enable_shared_from_this<Device> {
    protected:  // 
        std::weak_ptr<Instance> instance = {};
        std::shared_ptr<DeviceBase> data = {};

    public: // 
        Device(const std::shared_ptr<Instance>& instance, const std::shared_ptr<DeviceBase>& data): instance(instance), data(data)
        { this->constructor(); };

        // 
        virtual std::shared_ptr<Device> constructor() {
            if (!this->data) { this->data = std::make_shared<DeviceBase>(); };
            return shared_from_this();
        };

        // 
        virtual inline std::shared_ptr<DeviceBase>& getData() { return data; };
        virtual inline std::shared_ptr<Instance> getInstance() { return instance.lock(); };
        virtual inline std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) { return data->queues.at(queueFamilyIndex)[index]; };
        virtual inline std::shared_ptr<PhysicalDevice>& getPhysicalDevice(const uint32_t& deviceId = 0u) { return data->physicalDevices[deviceId]; };
        virtual std::shared_ptr<ImageView> getImageViewObject(const ImageViewKey& imageViewKey);
        virtual std::shared_ptr<BufferAllocation> getBufferAllocationObject(const uintptr_t& deviceAddress = 0ull);
        virtual std::shared_ptr<tom::DeviceBuffer> getDeviceBufferObject(const DeviceBufferKey& buffer);
        virtual std::shared_ptr<tom::DeviceMemory> getDeviceMemoryObject(const DeviceMemoryKey& deviceMemory);

        // 
        virtual ImageViewKey setImageViewObject(const std::shared_ptr<ImageView>& imageViewObj = {});
        virtual uintptr_t setBufferAllocationObject(const std::shared_ptr<BufferAllocation>& allocation = {});
        virtual DeviceBufferKey setDeviceBufferObject(const std::shared_ptr<tom::DeviceBuffer>& deviceBuffer = {});
        virtual DeviceMemoryKey setDeviceMemoryObject(const std::shared_ptr<tom::DeviceMemory>& deviceMemoryObj = {});

        // 
        virtual inline std::shared_ptr<Instance> getInstance() const { return instance.lock(); };
        virtual inline const std::shared_ptr<DeviceBase>& getData() const { return data; };
        virtual inline const std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) const { return data->queues.at(queueFamilyIndex)[index]; };
        virtual inline const std::shared_ptr<PhysicalDevice>& getPhysicalDevice(const uint32_t& deviceId = 0u) const { return data->physicalDevices[deviceId]; };
        virtual std::shared_ptr<ImageView> getImageViewObject(const ImageViewKey& imageViewKey) const;
        virtual std::shared_ptr<BufferAllocation> getBufferAllocationObject(const uintptr_t& deviceAddress = 0ull) const;
        virtual std::shared_ptr<tom::DeviceBuffer> getDeviceBufferObject(const DeviceBufferKey& buffer) const;
        virtual std::shared_ptr<tom::DeviceMemory> getDeviceMemoryObject(const DeviceMemoryKey& deviceMemory) const;

        //
        virtual std::shared_ptr<MemoryAllocator>& createAllocator();
    };



    //
    class MemoryAllocator : public std::enable_shared_from_this<MemoryAllocator> { 
    protected: // 
        std::weak_ptr<Device> device = {};
        std::shared_ptr<MemoryAllocatorBase> data = {};

    public: // 
        MemoryAllocator(const std::shared_ptr<Device>& device, const std::shared_ptr<MemoryAllocatorBase>& data = {}): device(device), data(data) 
        { this->constructor(); };

        // 
        virtual std::shared_ptr<MemoryAllocator> constructor() {
            if (!this->data) { this->data = std::make_shared<MemoryAllocatorBase>(); }; 
            return shared_from_this();
        };

        // 
        virtual std::shared_ptr<DeviceBuffer> allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {});
        virtual std::shared_ptr<DeviceImage> allocateImage(const std::shared_ptr<DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {});
        virtual std::shared_ptr<MemoryAllocation> allocateMemory(const std::shared_ptr<MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& allocInfo = {});
        virtual std::shared_ptr<DeviceBuffer> allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {});
        virtual std::shared_ptr<DeviceImage> allocateAndCreateImage(const std::shared_ptr<DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {});

        // 
        virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
        virtual inline std::shared_ptr<MemoryAllocatorBase>& getData() { return data; };

        // 
        virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
        virtual inline const std::shared_ptr<MemoryAllocatorBase>& getData() const { return data; };
    };

};
