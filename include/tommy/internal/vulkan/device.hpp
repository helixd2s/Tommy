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
            uint32_t queueFamilyIndex = 0u;

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
            std::unordered_map<vk::DeviceAddress, std::shared_ptr<BufferAllocation>> bufferAllocations = {};
            std::unordered_map<vk::Buffer, std::shared_ptr<DeviceBuffer>> buffers = {};
            std::unordered_map<vk::DeviceMemory, std::shared_ptr<DeviceMemory>> memories = {};
            std::unordered_map<ImageViewKey, std::shared_ptr<ImageView>> imageViews = {};

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
            Queue(const std::shared_ptr<tom::Device>& device, const vk::Queue& queue = {}, const uint32_t& queueFamilyIndex = 0u) : tom::Queue(device, QueueData::makeShared(queue, queueFamilyIndex)) {
                this->constructor();
            };

            // 
            virtual std::shared_ptr<tom::Queue> constructor() override {
                auto data = this->getDataTyped();
                auto device = std::dynamic_pointer_cast<DeviceData>(this->getDevice()->getData());
                data->commandPool = device->device.createCommandPool(vk::CommandPoolCreateInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer, .queueFamilyIndex = data->queueFamilyIndex });
                return shared_from_this();
            };

            // 
            virtual vk::Fence submitCmds(const std::vector<vk::CommandBuffer>& commandBuffers, vk::SubmitInfo2KHR submitInfo = vk::SubmitInfo2KHR{}) const;
            virtual std::future<vk::Result> submitOnce(const std::function<void(const vk::CommandBuffer&)>& submitCommand, const vk::SubmitInfo2KHR& submitInfo = vk::SubmitInfo2KHR{}) const;
        };


        // 
        class Device: public tom::Device {
        protected:  //
            virtual inline std::shared_ptr<DeviceData> getDataTyped() { return std::dynamic_pointer_cast<DeviceData>(this->data); };
            virtual inline std::shared_ptr<DeviceData> getDataTyped() const { return std::dynamic_pointer_cast<DeviceData>(this->data); };

        public: // 
            Device(const std::shared_ptr<tom::Instance>& instance, const std::shared_ptr<tom::PhysicalDevice>& physical): tom::Device(instance, physical) { // 
                
            };

            // 
            virtual std::shared_ptr<tom::Device> constructor() override;

            // 
            //virtual inline vk::DispatchLoaderDynamic& getDispatch() { return dispatch; };
            //virtual inline vk::Device& getDevice() { return device; };
            //virtual inline vk::DescriptorPool& getDescriptorPool() { return descriptorPool; };
            virtual inline std::vector<uint32_t>& getQueueFamilyIndices() { return queueFamilyIndices; };
            virtual std::shared_ptr<tom::DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer);
            virtual std::shared_ptr<tom::DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory);
            virtual std::shared_ptr<tom::DeviceMemory> allocateMemoryObject(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});
            virtual std::shared_ptr<tom::BufferAllocation> getBufferAllocationObject(const vk::DeviceAddress& deviceAddress = 0ull);
            virtual std::shared_ptr<tom::ImageView> getImageViewObject(const tom::ImageViewKey& imageViewKey) override;

            // 
            virtual vk::Buffer setDeviceBufferObject(const std::shared_ptr<tom::DeviceBuffer>& deviceBuffer = {});
            virtual vk::DeviceMemory setDeviceMemoryObject(const std::shared_ptr<tom::DeviceMemory>& deviceMemoryObj = {});
            virtual vk::DeviceAddress setBufferAllocationObject(const std::shared_ptr<tom::BufferAllocation>& allocation = {});
            virtual tom::ImageViewKey setImageViewObject(const std::shared_ptr<tom::ImageView>& imageViewObj = {}) override;
            virtual std::shared_ptr<tom::DeviceMemory> allocateMemoryObject(const std::shared_ptr<tom::MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});

            // 
            //virtual inline const vk::DispatchLoaderDynamic& getDispatch() const { return dispatch; };
            //virtual inline const vk::Device& getDevice() const { return device; };
            //virtual inline const vk::DescriptorPool& getDescriptorPool() const { return descriptorPool; };
            virtual inline const std::vector<uint32_t>& getQueueFamilyIndices() const { return queueFamilyIndices; };
            virtual std::shared_ptr<tom::DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer) const;
            virtual std::shared_ptr<tom::DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const;
            virtual std::shared_ptr<tom::ImageView> getImageViewObject(const tom::ImageViewKey& imageViewKey) const override;
            virtual std::shared_ptr<tom::BufferAllocation> getBufferAllocationObject(const vk::DeviceAddress& deviceAddress = 0ull) const;
            //virtual vk::DeviceAddress setBufferAllocationObject(const vk::DeviceAddress& deviceAddress = 0ull, const std::shared_ptr<BufferAllocation>& allocation = {}) const;

            //
            virtual std::shared_ptr<tom::MemoryAllocator>& createAllocatorVk();
            virtual std::shared_ptr<tom::MemoryAllocator>& createAllocatorVma();
        };



        //
        class MemoryAllocator : public tom::MemoryAllocator { protected: // 

        public: // 
            MemoryAllocator(const std::shared_ptr<tom::Device>& device): tom::MemoryAllocator(device) {
                
            };

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
            MemoryAllocatorVma(const std::shared_ptr<tom::Device>& device): MemoryAllocator(device) {
                //this->constructor();
            };

            // 
            virtual std::shared_ptr<tom::MemoryAllocator> constructor() override;

            // 
            virtual std::shared_ptr<tom::DeviceBuffer> allocateAndCreateBuffer(const std::shared_ptr<tom::DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {}) override;
            virtual std::shared_ptr<tom::DeviceImage> allocateAndCreateImage(const std::shared_ptr<tom::DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {}) override;
            virtual std::shared_ptr<tom::MemoryAllocation> allocateMemory(const std::shared_ptr<tom::MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& allocInfo = {}) override;

        };

    };

};
