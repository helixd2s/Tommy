#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./instance.hpp"

// 
namespace tom {

    // 
    class DescriptorSetSource: public std::enable_shared_from_this<DescriptorSetSource> { public:
        std::vector<vk::DescriptorBufferInfo> buffers = {};
        std::vector<vk::DescriptorImageInfo> textures2d = {};
    };

    // 
    class DescriptorSetLayouts: public std::enable_shared_from_this<DescriptorSetLayouts> { public:
        vk::DescriptorSetLayout buffers = {};
        vk::DescriptorSetLayout textures = {};
        vk::DescriptorSetLayout images = {};
    };

    // 
    class DescriptorSet: public std::enable_shared_from_this<DescriptorSet> { public:
        vk::DescriptorSet buffers = {};
        vk::DescriptorSet textures = {};
        vk::DescriptorSet images = {};
    };

    // 
    class Queue: public std::enable_shared_from_this<Queue> {
    protected:  //
        //std::shared_ptr<tom::Device> device = {};
        std::weak_ptr<tom::Device> device = {};

        // 
        vk::Queue queue = {};
        vk::CommandPool commandPool = {};
        uint32_t queueFamilyIndex = 0u;

    public: // 
        Queue(const std::shared_ptr<tom::Device>& device, const vk::Queue& queue = {}, const uint32_t& queueFamilyIndex = 0u) : device(device), queue(queue), queueFamilyIndex(queueFamilyIndex) {
            this->constructor();
        };

        //
        virtual void constructor() {
            this->commandPool = this->device.lock()->getDevice().createCommandPool(vk::CommandPoolCreateInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer, .queueFamilyIndex = queueFamilyIndex });
        };

        // 
        virtual inline const uint32_t& getQueueFamilyIndex() const { return queueFamilyIndex; };
        virtual inline const vk::Queue& getQueue() const { return queue; };
        virtual inline const vk::CommandPool& getCommandPool() const { return commandPool; };

        // 
        virtual inline uint32_t& getQueueFamilyIndex() { return queueFamilyIndex; };
        virtual inline vk::Queue& getQueue() { return queue; };
        virtual inline vk::CommandPool& getCommandPool() { return commandPool; };

        //
        virtual vk::Fence submitCmds(const std::vector<vk::CommandBuffer>& commandBuffers, vk::SubmitInfo2KHR submitInfo = vk::SubmitInfo2KHR{}) const;
        virtual std::future<vk::Result> submitOnce(const std::function<void(const vk::CommandBuffer&)>& submitCommand, const vk::SubmitInfo2KHR& submitInfo = vk::SubmitInfo2KHR{}) const;
    };

    // 
    class Device: public std::enable_shared_from_this<Device> {
    protected:  // 
        std::shared_ptr<Instance> instance = {};
        std::shared_ptr<PhysicalDevice> physical = {};

        // 
        vk::Device device = {};
        vk::DescriptorPool descriptorPool = {};
        vk::DispatchLoaderDynamic dispatch = {};

        // 
        std::shared_ptr<MemoryAllocator> allocator = {};

        // 
        std::unordered_map<uintptr_t, std::weak_ptr<Buffer>> bufferAllocations = {};
        std::unordered_map<vk::Buffer, std::weak_ptr<DeviceBuffer>> buffers = {};
        std::unordered_map<vk::DeviceMemory, std::weak_ptr<DeviceMemory>> memories = {};

        // 
        std::shared_ptr<tom::DescriptorSetSource> descriptions = {};
        std::shared_ptr<tom::DescriptorSetLayouts> descriptorSetLayouts = {};
        std::shared_ptr<tom::DescriptorSet> descriptorSets = {};

        // 
        std::unordered_map<uint32_t, std::vector<std::shared_ptr<Queue>>> queues = {};
        std::vector<uint32_t> queueFamilyIndices = {};

    public: // 
        Device(const std::shared_ptr<tom::Instance>& instance, const std::shared_ptr<PhysicalDevice>& physical): instance(instance), physical(physical) { // 
            this->constructor();
        };

        // 
        virtual std::shared_ptr<Device> constructor();

        // 
        virtual inline vk::DispatchLoaderDynamic& getDispatch() { return dispatch; };
        virtual inline vk::Device& getDevice() { return device; };
        virtual inline vk::DescriptorPool& getDescriptorPool() { return descriptorPool; };
        virtual inline std::vector<uint32_t>& getQueueFamilyIndices() { return queueFamilyIndices; };
        virtual inline std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) { return queues.at(queueFamilyIndex)[index]; };
        virtual inline std::shared_ptr<Instance>& getInstance() { return instance; };
        virtual inline std::shared_ptr<PhysicalDevice>& getPhysicalDevice(const uint32_t& deviceId = 0u) { return physical; };
        virtual std::shared_ptr<DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer);
        virtual std::shared_ptr<DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory);
        virtual std::shared_ptr<DeviceMemory> allocateMemoryObject(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});

        // 
        virtual inline const vk::DispatchLoaderDynamic& getDispatch() const { return dispatch; };
        virtual inline const vk::Device& getDevice() const { return device; };
        virtual inline const vk::DescriptorPool& getDescriptorPool() const { return descriptorPool; };
        virtual inline const std::vector<uint32_t>& getQueueFamilyIndices() const { return queueFamilyIndices; };
        virtual inline const std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) const { return queues.at(queueFamilyIndex)[index]; };
        virtual inline const std::shared_ptr<Instance>& getInstance() const { return instance; };
        virtual inline const std::shared_ptr<PhysicalDevice>& getPhysicalDevice(const uint32_t& deviceId = 0u) const { return physical; };
        virtual std::shared_ptr<DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer) const;
        virtual std::shared_ptr<DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const;

        //
        //virtual std::shared_ptr<MemoryAllocator>& createAllocator();
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
        virtual std::shared_ptr<MemoryAllocator> allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY);
        virtual std::shared_ptr<MemoryAllocator> allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY);

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
        virtual std::shared_ptr<MemoryAllocator> allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) override;
        virtual std::shared_ptr<MemoryAllocator> allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) override;
    };



};
