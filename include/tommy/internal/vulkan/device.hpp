#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "../device.hpp"

#include "./core.hpp"
#include "./instance.hpp"

// 
namespace tom {

    // 
    namespace vulkan {

        // 
        class QueueData: public QueueBase {
        public:
            vk::Queue queue = {};
            vk::CommandPool commandPool = {};
            std::vector<vk::CommandBuffer> commandBuffers = {};

            // 
            static std::shared_ptr<QueueData> makeShared(const vk::Queue& queue = {}, const uint32_t& queueFamilyIndex = 0u) {
                std::shared_ptr<QueueData> data = {};
                data->queue = queue;
                data->queueFamilyIndex = queueFamilyIndex;
                return data;
            };
        };

        // 
        class DeviceData: public DeviceBase {
        public:
            // 
            vk::Device device = {};
            vk::DescriptorPool descriptorPool = {};
            vk::DispatchLoaderDynamic dispatch = {};

            // 
            std::shared_ptr<DescriptorSetSource> descriptions = {};
            std::shared_ptr<DescriptorSetLayouts> descriptorSetLayouts = {};
            std::shared_ptr<DescriptorSet> descriptorSets = {};
        };




        // 
        class Queue: public tom::Queue {
        protected:  //
            virtual inline std::shared_ptr<QueueData> getDataTyped() { return std::dynamic_pointer_cast<QueueData>(this->data); };
            virtual inline std::shared_ptr<QueueData> getDataTyped() const { return std::dynamic_pointer_cast<QueueData>(this->data); };

        public: 
            // legacy
            //Queue(const std::shared_ptr<tom::Device>& device, const vk::Queue& queue = {}, const uint32_t& queueFamilyIndex = 0u) : tom::Queue(device, QueueData::makeShared(queue, queueFamilyIndex)) 
            //{};

            // 
            virtual std::shared_ptr<tom::Queue> constructor() override {
                auto data = this->getDataTyped();
                auto device = std::dynamic_pointer_cast<DeviceData>(this->getDevice()->getData());
                data->commandPool = device->device.createCommandPool(vk::CommandPoolCreateInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer, .queueFamilyIndex = data->queueFamilyIndex });
                return shared_from_this();
            };//commandBuffers

            // 
            virtual vk::Fence submitCmds(const vk::SubmitInfo2KHR& submitInfo = vk::SubmitInfo2KHR{}) const;
            virtual vk::Fence submitCmds(const std::vector<vk::CommandBuffer>& commandBuffers, vk::SubmitInfo2KHR submitInfo = vk::SubmitInfo2KHR{}) const;
            virtual std::future<ApiResult> submitOnce(const std::function<void(const vk::CommandBuffer&)>& submitCommand, const vk::SubmitInfo2KHR& submitInfo = vk::SubmitInfo2KHR{}) const;

            // 
            virtual std::future<ApiResult> copyDeviceBuffers(const std::shared_ptr<tom::BufferAllocation>& src, const std::shared_ptr<tom::BufferAllocation>& dst) override;
            virtual std::future<ApiResult> copyDeviceBufferToImage(const std::shared_ptr<tom::BufferAllocation>& src, const std::shared_ptr<tom::ImageView>& dst, const ImageRegion& dstRegion = {}) override;
            virtual std::future<ApiResult> copyDeviceImageToBuffer(const std::shared_ptr<tom::ImageView>& src, const std::shared_ptr<tom::BufferAllocation>& dst, const ImageRegion& srcRegion = {}) override;

            // 
            virtual inline uintptr_t getKey() override { return reinterpret_cast<uintptr_t&>(std::dynamic_pointer_cast<QueueData>(this->getData())->queue); };
            virtual inline uintptr_t getKey() const override { return reinterpret_cast<const uintptr_t&>(std::dynamic_pointer_cast<QueueData>(this->getData())->queue); };
        };


        // 
        class Device: public tom::Device {
        protected:  //
            virtual inline std::shared_ptr<DeviceData> getDataTyped() { return std::dynamic_pointer_cast<DeviceData>(this->data); };
            virtual inline std::shared_ptr<DeviceData> getDataTyped() const { return std::dynamic_pointer_cast<DeviceData>(this->data); };

        public: // 
            Device(const std::shared_ptr<tom::Instance>& instance, const std::shared_ptr<DeviceData>& data): tom::Device(instance, data) 
            {};

            // 
            virtual std::shared_ptr<tom::Device> constructor() override;

            //
            virtual std::shared_ptr<tom::DeviceMemory> allocateMemoryObject(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});
            virtual std::shared_ptr<tom::ImageView> getImageViewObject(const tom::ImageViewKey& imageViewKey) override;
            virtual std::shared_ptr<tom::BufferAllocation> getBufferAllocationObject(const uintptr_t& deviceAddress = 0ull) override;
            virtual std::shared_ptr<tom::DeviceImage> getDeviceImageObject(const DeviceImageKey& image) override;
            virtual std::shared_ptr<tom::DeviceBuffer> getDeviceBufferObject(const DeviceBufferKey& buffer) override;
            virtual std::shared_ptr<tom::DeviceMemory> getDeviceMemoryObject(const DeviceMemoryKey& deviceMemory) override;

            // 
            virtual std::shared_ptr<tom::DeviceMemory> allocateMemoryObject(const std::shared_ptr<tom::MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});
            virtual tom::ImageViewKey setImageViewObject(const std::shared_ptr<tom::ImageView>& imageViewObj = {}) override;
            virtual uintptr_t setBufferAllocationObject(const std::shared_ptr<tom::BufferAllocation>& allocation = {}) override;
            virtual DeviceImageKey setDeviceImageObject(const std::shared_ptr<tom::DeviceImage>& image = {}) override;
            virtual DeviceBufferKey setDeviceBufferObject(const std::shared_ptr<tom::DeviceBuffer>& deviceBuffer = {}) override;
            virtual DeviceMemoryKey setDeviceMemoryObject(const std::shared_ptr<tom::DeviceMemory>& deviceMemoryObj = {}) override;

            // 
            virtual std::shared_ptr<tom::ImageView> getImageViewObject(const tom::ImageViewKey& imageViewKey) const override;
            virtual std::shared_ptr<tom::BufferAllocation> getBufferAllocationObject(const uintptr_t& deviceAddress = 0ull) const override;
            virtual std::shared_ptr<tom::DeviceImage> getDeviceImageObject(const DeviceImageKey& image) const override;
            virtual std::shared_ptr<tom::DeviceBuffer> getDeviceBufferObject(const DeviceBufferKey& buffer) const override;
            virtual std::shared_ptr<tom::DeviceMemory> getDeviceMemoryObject(const DeviceMemoryKey& deviceMemory) const override;

            // 
            virtual inline uintptr_t getKey() override { return reinterpret_cast<uintptr_t&>(std::dynamic_pointer_cast<DeviceData>(this->getData())->device); };
            virtual inline uintptr_t getKey() const override { return reinterpret_cast<const uintptr_t&>(std::dynamic_pointer_cast<DeviceData>(this->getData())->device); };

            //
            virtual std::shared_ptr<tom::MemoryAllocator>& createAllocatorVk();
            virtual std::shared_ptr<tom::MemoryAllocator>& createAllocatorVma();
        };



        // 
        class MemoryAllocator : public tom::MemoryAllocator { protected: // 

        public: // 
            MemoryAllocator(const std::shared_ptr<tom::Device>& device): tom::MemoryAllocator(device) 
            {};

            // 
            virtual std::shared_ptr<tom::MemoryAllocator> constructor() { 
                return shared_from_this();
            };

            // 
            virtual std::shared_ptr<tom::DeviceBuffer> allocateBuffer(const std::shared_ptr<tom::DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {}) override;
            virtual std::shared_ptr<tom::DeviceBuffer> allocateAndCreateBuffer(const std::shared_ptr<tom::DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {}) override;

            // 
            virtual std::shared_ptr<tom::DeviceImage> allocateImage(const std::shared_ptr<tom::DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {}) override;
            virtual std::shared_ptr<tom::DeviceImage> allocateAndCreateImage(const std::shared_ptr<tom::DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {}) override;

            // 
            virtual std::shared_ptr<tom::MemoryAllocation> allocateMemory(const std::shared_ptr<tom::MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& allocInfo = {}) override;
        };

        // 
        class MemoryAllocatorVma : public MemoryAllocator { protected: // 
        public:
            MemoryAllocatorVma(const std::shared_ptr<tom::Device>& device): MemoryAllocator(device) 
            {};

            // 
            virtual std::shared_ptr<tom::MemoryAllocator> constructor() override;

            // 
            virtual std::shared_ptr<tom::DeviceBuffer> allocateAndCreateBuffer(const std::shared_ptr<tom::DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {}) override;
            virtual std::shared_ptr<tom::DeviceImage> allocateAndCreateImage(const std::shared_ptr<tom::DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {}) override;
            virtual std::shared_ptr<tom::MemoryAllocation> allocateMemory(const std::shared_ptr<tom::MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& allocInfo = {}) override;

        };

    };

};
