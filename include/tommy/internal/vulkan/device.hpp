#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
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
        class Queue: public std::enable_shared_from_this<Queue> {
        protected:  //
            //std::shared_ptr<tom::Device> device = {};
            std::weak_ptr<Device> device = {};
            std::shared_ptr<QueueData> data = {};

        public: 
            // legacy
            Queue(const std::shared_ptr<Device>& device, const vk::Queue& queue = {}, const uint32_t& queueFamilyIndex = 0u) : device(device) {
                data = std::make_shared<QueueData>();
                data->queue = queue;
                data->queueFamilyIndex = queueFamilyIndex;
                this->constructor();
            };

            // 
            virtual std::shared_ptr<Queue> constructor() {
                data->commandPool = this->getDevice()->getData()->device.createCommandPool(vk::CommandPoolCreateInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer, .queueFamilyIndex = data->queueFamilyIndex });
                return shared_from_this();
            };

            // 
            virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
            virtual inline std::shared_ptr<QueueData> getData() const { return data; };

            // 
            virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
            virtual inline std::shared_ptr<QueueData> getData() { return data; };

            // 
            virtual vk::Fence submitCmds(const std::vector<vk::CommandBuffer>& commandBuffers, vk::SubmitInfo2KHR submitInfo = vk::SubmitInfo2KHR{}) const;
            virtual std::future<vk::Result> submitOnce(const std::function<void(const vk::CommandBuffer&)>& submitCommand, const vk::SubmitInfo2KHR& submitInfo = vk::SubmitInfo2KHR{}) const;
        };


        // 
        class Device: public std::enable_shared_from_this<Device> {
        protected:  // 
            std::weak_ptr<Instance> instance = {};
            std::shared_ptr<PhysicalDevice> physical = {};
            std::shared_ptr<DeviceData> data = {};
            std::shared_ptr<MemoryAllocator> allocator = {};

            // 
            std::unordered_map<uint32_t, std::vector<std::shared_ptr<Queue>>> queues = {};
            std::vector<uint32_t> queueFamilyIndices = {};

        public: // 
            Device(const std::shared_ptr<Instance>& instance, const std::shared_ptr<PhysicalDevice>& physical): instance(instance), physical(physical) { // 
                this->constructor();
            };

            // 
            virtual std::shared_ptr<Device> constructor();

            // 
            //virtual inline vk::DispatchLoaderDynamic& getDispatch() { return dispatch; };
            //virtual inline vk::Device& getDevice() { return device; };
            //virtual inline vk::DescriptorPool& getDescriptorPool() { return descriptorPool; };
            virtual inline std::vector<uint32_t>& getQueueFamilyIndices() { return queueFamilyIndices; };

            virtual inline std::shared_ptr<DeviceData> getData() const { return data; };
            virtual inline std::shared_ptr<Instance> getInstance() { return instance.lock(); };
            virtual inline std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) { return queues.at(queueFamilyIndex)[index]; };
            virtual inline std::shared_ptr<PhysicalDevice>& getPhysicalDevice(const uint32_t& deviceId = 0u) { return physical; };
            virtual std::shared_ptr<DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer);
            virtual std::shared_ptr<DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory);
            virtual std::shared_ptr<DeviceMemory> allocateMemoryObject(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});
            virtual std::shared_ptr<BufferAllocation> getBufferAllocationObject(const vk::DeviceAddress& deviceAddress = 0ull);
            virtual std::shared_ptr<ImageView> getImageViewObject(const ImageViewKey& imageViewKey);
            

            // 
            virtual vk::Buffer setDeviceBufferObject(const std::shared_ptr<DeviceBuffer>& deviceBuffer = {});
            virtual vk::DeviceMemory setDeviceMemoryObject(const std::shared_ptr<DeviceMemory>& deviceMemoryObj = {});
            virtual vk::DeviceAddress setBufferAllocationObject(const std::shared_ptr<BufferAllocation>& allocation = {});
            virtual ImageViewKey setImageViewObject(const std::shared_ptr<ImageView>& imageViewObj = {});

            // 
            //virtual inline const vk::DispatchLoaderDynamic& getDispatch() const { return dispatch; };
            //virtual inline const vk::Device& getDevice() const { return device; };
            //virtual inline const vk::DescriptorPool& getDescriptorPool() const { return descriptorPool; };
            virtual inline const std::vector<uint32_t>& getQueueFamilyIndices() const { return queueFamilyIndices; };

            virtual inline std::shared_ptr<DeviceData> getData() const { return data; };
            virtual inline std::shared_ptr<Instance> getInstance() const { return instance.lock(); };
            virtual inline const std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) const { return queues.at(queueFamilyIndex)[index]; };
            virtual inline const std::shared_ptr<PhysicalDevice>& getPhysicalDevice(const uint32_t& deviceId = 0u) const { return physical; };
            virtual std::shared_ptr<DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer) const;
            virtual std::shared_ptr<DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const;
            virtual std::shared_ptr<ImageView> getImageViewObject(const ImageViewKey& imageViewKey) const;
            virtual std::shared_ptr<BufferAllocation> getBufferAllocationObject(const vk::DeviceAddress& deviceAddress = 0ull) const;
            //virtual vk::DeviceAddress setBufferAllocationObject(const vk::DeviceAddress& deviceAddress = 0ull, const std::shared_ptr<BufferAllocation>& allocation = {}) const;

            //
            virtual std::shared_ptr<MemoryAllocator>& createAllocator();
            virtual std::shared_ptr<MemoryAllocator>& createAllocatorVma();
        };



        //
        class MemoryAllocator : public std::enable_shared_from_this<MemoryAllocator> { protected: // 
            std::weak_ptr<Device> device = {};
            void* allocator = nullptr;

        public: // 
            MemoryAllocator(const std::shared_ptr<Device>& device): device(device) {
                this->constructor();
            };

            // 
            virtual std::shared_ptr<MemoryAllocator> constructor() { 
                return shared_from_this();
            };

            // 
            virtual std::shared_ptr<DeviceBuffer> allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {});
            virtual std::shared_ptr<DeviceBuffer> allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {});

            // 
            virtual std::shared_ptr<DeviceImage> allocateImage(const std::shared_ptr<DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {});
            virtual std::shared_ptr<DeviceImage> allocateAndCreateImage(const std::shared_ptr<DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {});

            // 
            virtual std::shared_ptr<MemoryAllocation> allocateMemory(const std::shared_ptr<MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& allocInfo = {});

            // 
            virtual std::shared_ptr<Device> getDevice() { return device.lock(); };
            virtual inline void*& getAllocator() { return allocator; };

            // 
            virtual std::shared_ptr<Device> getDevice() const { return device.lock(); };
            virtual inline void* const& getAllocator() const { return allocator; };
        };

        //
        class MemoryAllocatorVma : public MemoryAllocator { protected: // 
        public:
            MemoryAllocatorVma(const std::shared_ptr<Device>& device): MemoryAllocator(device) {
                //this->constructor();
            };

            // 
            virtual std::shared_ptr<MemoryAllocator> constructor() override;

            // 
            //virtual std::shared_ptr<DeviceBuffer> allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const MemoryAllocationInfo& allocInfo = {}) override;
            //virtual std::shared_ptr<DeviceImage> allocateImage(const std::shared_ptr<DeviceImage>& image, const MemoryAllocationInfo& allocInfo = {}) override;
            virtual std::shared_ptr<DeviceBuffer> allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const tom::MemoryAllocationInfo& allocInfo = {}) override;
            virtual std::shared_ptr<DeviceImage> allocateAndCreateImage(const std::shared_ptr<DeviceImage>& image, const tom::MemoryAllocationInfo& allocInfo = {}) override;
            virtual std::shared_ptr<MemoryAllocation> allocateMemory(const std::shared_ptr<MemoryAllocation>& allocation, const tom::MemoryAllocationInfo& allocInfo = {}) override;

        };

    };

};
