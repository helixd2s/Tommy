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
        Queue(const std::shared_ptr<Device>& device, const std::shared_ptr<QueueBase>& data = {}) : device(device), data(data) {
            this->constructor();
        };

        // 
        virtual std::shared_ptr<Queue> constructor() {
            return shared_from_this();
        };

        // 
        virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
        virtual inline std::shared_ptr<QueueBase> getData() const { return data; };

        // 
        virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
        virtual inline std::shared_ptr<QueueBase> getData() { return data; };
    };


    // 
    class Device: public std::enable_shared_from_this<Device> {
    protected:  // 
        std::weak_ptr<Instance> instance = {};
        std::shared_ptr<PhysicalDevice> physical = {};
        std::shared_ptr<DeviceBase> data = {};
        std::shared_ptr<MemoryAllocator> allocator = {};

        // 
        std::unordered_map<uint32_t, std::vector<std::shared_ptr<Queue>>> queues = {};
        std::vector<uint32_t> queueFamilyIndices = {};

    public: // 
        Device(const std::shared_ptr<Instance>& instance, const std::shared_ptr<PhysicalDevice>& physical): instance(instance), physical(physical) { // 
            this->constructor();
        };

        // 
        virtual std::shared_ptr<Device> constructor() {
            return shared_from_this();
        };

        // 
        //virtual inline vk::DispatchLoaderDynamic& getDispatch() { return dispatch; };
        //virtual inline vk::Device& getDevice() { return device; };
        //virtual inline vk::DescriptorPool& getDescriptorPool() { return descriptorPool; };
        virtual inline std::vector<uint32_t>& getQueueFamilyIndices() { return queueFamilyIndices; };

        virtual inline std::shared_ptr<DeviceBase> getData() const { return data; };
        virtual inline std::shared_ptr<Instance> getInstance() { return instance.lock(); };
        virtual inline std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) { return queues.at(queueFamilyIndex)[index]; };
        virtual inline std::shared_ptr<PhysicalDevice>& getPhysicalDevice(const uint32_t& deviceId = 0u) { return physical; };

        //MemoryAllocationInfo
        virtual std::shared_ptr<ImageView> getImageViewObject(const ImageViewKey& imageViewKey);


        //
        virtual ImageViewKey setImageViewObject(const std::shared_ptr<ImageView>& imageViewObj = {});

        // 
        //virtual inline const vk::DispatchLoaderDynamic& getDispatch() const { return dispatch; };
        //virtual inline const vk::Device& getDevice() const { return device; };
        //virtual inline const vk::DescriptorPool& getDescriptorPool() const { return descriptorPool; };
        virtual inline const std::vector<uint32_t>& getQueueFamilyIndices() const { return queueFamilyIndices; };

        virtual inline std::shared_ptr<DeviceBase> getData() const { return data; };
        virtual inline std::shared_ptr<Instance> getInstance() const { return instance.lock(); };
        virtual inline const std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) const { return queues.at(queueFamilyIndex)[index]; };
        virtual inline const std::shared_ptr<PhysicalDevice>& getPhysicalDevice(const uint32_t& deviceId = 0u) const { return physical; };
        virtual std::shared_ptr<ImageView> getImageViewObject(const ImageViewKey& imageViewKey) const;
        //virtual vk::DeviceAddress setBufferAllocationObject(const vk::DeviceAddress& deviceAddress = 0ull, const std::shared_ptr<BufferAllocation>& allocation = {}) const;

        //
        virtual std::shared_ptr<MemoryAllocator>& createAllocator();
    };



    //
    class MemoryAllocator : public std::enable_shared_from_this<MemoryAllocator> { 
    protected: // 
        std::weak_ptr<Device> device = {};
        std::shared_ptr<MemoryAllocatorBase> data = {};

    public: // 
        MemoryAllocator(const std::shared_ptr<Device>& device, const std::shared_ptr<MemoryAllocatorBase>& data = {}): device(device), data(data) {
            if (!this->data) { this->data = std::make_shared<MemoryAllocatorBase>(); };
            this->constructor();
        };

        // 
        virtual std::shared_ptr<MemoryAllocator> constructor() { 
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
        virtual inline std::shared_ptr<MemoryAllocatorBase> getData() { return data; };

        // 
        virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
        virtual inline std::shared_ptr<MemoryAllocatorBase> getData() const { return data; };
    };

};
