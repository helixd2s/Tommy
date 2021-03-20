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
    class MemoryAllocator;
    class MemoryAllocatorVma;

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
        virtual void constructor() {
            this->descriptions = std::make_shared<tom::DescriptorSetSource>();
            this->descriptorSets = std::make_shared<tom::DescriptorSet>();
            this->descriptorSetLayouts = std::make_shared<tom::DescriptorSetLayouts>();

            // 
            std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {};

            // 
            std::vector<const char*> preferedLayers = {};
            std::vector<const char*> preferedExtensions = { "VK_KHR_swapchain", "VK_KHR_buffer_device_address" };

            //
            std::vector<const char*> excludedLayers = {};
            std::vector<const char*> excludedExtensions = {};

#ifdef NDEBUG
            preferedLayers.push_back("VK_LAYER_LUNARG_standard_validation");
            preferedExtensions.push_back("VK_EXT_debug_report");
#endif

            // 
            const auto& extensions = physical->getExtensionPropertiesDefined();
            for (uintptr_t i=0;i<preferedExtensions.size();i++) {
                const auto& name = preferedExtensions[i];
                bool found = false;
                for (auto& prop : extensions) {
                    if (found = found || (strcmp(name, prop.extensionName) == 0)) break;
                };
                if (!found) { 
                    excludedExtensions.push_back(name);
                    preferedExtensions.erase(preferedExtensions.begin()+i); 
                };
            };

            // 
            const auto& layers = physical->getLayerPropertiesDefined();
            for (uintptr_t i=0;i<preferedLayers.size();i++) {
                const auto& name = preferedLayers[i];
                bool found = false;
                for (auto& prop : layers) {
                    if (found = found || (strcmp(name, prop.layerName) == 0)) break;
                };
                if (!found) { 
                    excludedLayers.push_back(name);
                    preferedLayers.erase(preferedLayers.begin()+i);
                };
            };

            // 
            auto& queueFamilyProperties = physical->getQueueFamilyProperties();
            uint32_t I=0u; for (auto& property : queueFamilyProperties) { const uint32_t i = I++;
                std::vector<float> queue_priorities = { 1.f }; // queues per every family
                if (property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute) {
                    queues[i] = std::vector<std::shared_ptr<Queue>>{};
                    queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{
                        .queueFamilyIndex = i,
                        .queueCount = static_cast<uint32_t>(queue_priorities.size()),
                        .pQueuePriorities = queue_priorities.data()
                    });
                    queueFamilyIndices.push_back(i);
                };
            };

            // 
            this->dispatch = vk::DispatchLoaderDynamic( this->instance->getInstance(), vkGetInstanceProcAddr, this->device = this->physical->getPhysicalDevice().createDevice(vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceFeatures2>{
                vk::DeviceCreateInfo{
                    .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                    .pQueueCreateInfos = queueCreateInfos.data(),
                    .enabledLayerCount = static_cast<uint32_t>(preferedLayers.size()),
                    .ppEnabledLayerNames = preferedLayers.data(),
                    .enabledExtensionCount = static_cast<uint32_t>(preferedExtensions.size()),
                    .ppEnabledExtensionNames = preferedExtensions.data()
                }, this->physical->getFeaturesChainDefined().get<vk::PhysicalDeviceFeatures2>()
            }.get<vk::DeviceCreateInfo>()), vkGetDeviceProcAddr );

            // 
            for (uint32_t i=0u;i<queueCreateInfos.size();i++) {
                const auto& info = queueCreateInfos[i];
                const auto& queueFamilyIndex = info.queueFamilyIndex;
                for (uint32_t j=0;j<info.queueCount;j++) {
                    queues[info.queueFamilyIndex].push_back(std::make_shared<Queue>(shared_from_this(), this->device.getQueue(queueFamilyIndex, j), queueFamilyIndex));
                };
            };

            // 
            auto dps = std::vector<vk::DescriptorPoolSize>{
                vk::DescriptorPoolSize{.type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = 256u },
                vk::DescriptorPoolSize{.type = vk::DescriptorType::eSampledImage, .descriptorCount = 256u },
                vk::DescriptorPoolSize{.type = vk::DescriptorType::eSampler, .descriptorCount = 256u },
                vk::DescriptorPoolSize{.type = vk::DescriptorType::eStorageBuffer, .descriptorCount = 1024u },
                vk::DescriptorPoolSize{.type = vk::DescriptorType::eUniformBuffer, .descriptorCount = 256u },
                vk::DescriptorPoolSize{.type = vk::DescriptorType::eStorageTexelBuffer, .descriptorCount = 256u },
                vk::DescriptorPoolSize{.type = vk::DescriptorType::eUniformTexelBuffer, .descriptorCount = 256u },
                vk::DescriptorPoolSize{.type = vk::DescriptorType::eAccelerationStructureKHR, .descriptorCount = 256u },
            };

            // 
            this->descriptorPool = this->device.createDescriptorPool(vk::StructureChain<vk::DescriptorPoolCreateInfo, vk::DescriptorPoolInlineUniformBlockCreateInfoEXT>(
                vk::DescriptorPoolCreateInfo{
                    .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind,
                    .maxSets = 256u,
                    .poolSizeCount = static_cast<uint32_t>(dps.size()),
                    .pPoolSizes = dps.data()
                }, 
                vk::DescriptorPoolInlineUniformBlockCreateInfoEXT{
                    .maxInlineUniformBlockBindings = 32u
                }
            ).get<vk::DescriptorPoolCreateInfo>());

            
        };


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
        virtual std::shared_ptr<MemoryAllocatorVma> createAllocatorVma();
    };

    //
    class MemoryAllocator : public std::enable_shared_from_this<MemoryAllocator> { protected: // 
        std::weak_ptr<Device> device = {};
        void* allocator = nullptr;

    public:
        MemoryAllocator(const std::shared_ptr<Device>& device): device(device) {
            this->constructor();
        };

        // 
        virtual inline void constructor() { 

        };

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
        MemoryAllocatorVma(const std::shared_ptr<Device>& device): device(device) {
            this->constructor();
        };

        //
        virtual inline void constructor() override { //
            auto& instanceDispatch = this->device->getInstance()->getDispatch();
            auto& deviceDispatch = this->device->getDispatch();

            // redirect Vulkan API functions
            VmaVulkanFunctions func = {};
            func.vkAllocateMemory = deviceDispatch.vkAllocateMemory;
            func.vkBindBufferMemory = deviceDispatch.vkBindBufferMemory;
            func.vkBindBufferMemory2KHR = deviceDispatch.vkBindBufferMemory2;
            func.vkBindImageMemory = deviceDispatch.vkBindImageMemory;
            func.vkBindImageMemory2KHR = deviceDispatch.vkBindImageMemory2;
            func.vkCmdCopyBuffer = deviceDispatch.vkCmdCopyBuffer;
            func.vkCreateBuffer = deviceDispatch.vkCreateBuffer;
            func.vkCreateImage = deviceDispatch.vkCreateImage;
            func.vkDestroyBuffer = deviceDispatch.vkDestroyBuffer;
            func.vkDestroyImage = deviceDispatch.vkDestroyImage;
            func.vkFlushMappedMemoryRanges = deviceDispatch.vkFlushMappedMemoryRanges;
            func.vkFreeMemory = deviceDispatch.vkFreeMemory;
            func.vkGetBufferMemoryRequirements = deviceDispatch.vkGetBufferMemoryRequirements;
            func.vkGetBufferMemoryRequirements2KHR = deviceDispatch.vkGetBufferMemoryRequirements2;
            func.vkGetImageMemoryRequirements = deviceDispatch.vkGetImageMemoryRequirements;
            func.vkGetImageMemoryRequirements2KHR = deviceDispatch.vkGetImageMemoryRequirements2;
            func.vkGetPhysicalDeviceMemoryProperties = instanceDispatch.vkGetPhysicalDeviceMemoryProperties;
            func.vkGetPhysicalDeviceMemoryProperties2KHR = instanceDispatch.vkGetPhysicalDeviceMemoryProperties2;
            func.vkGetPhysicalDeviceProperties = instanceDispatch.vkGetPhysicalDeviceProperties;
            func.vkInvalidateMappedMemoryRanges = deviceDispatch.vkInvalidateMappedMemoryRanges;
            func.vkMapMemory = deviceDispatch.vkMapMemory;
            func.vkUnmapMemory = deviceDispatch.vkUnmapMemory;

            // 
            VmaAllocatorCreateInfo vmaInfo = {};
            vmaInfo.pVulkanFunctions = &func;
            vmaInfo.device = this->device->getDevice();
            vmaInfo.instance = this->device->getInstance()->getInstance();
            vmaInfo.physicalDevice = this->device->getPhysicalDevice()->getPhysicalDevice();
            vmaInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

            // 
            vk::throwResultException(vk::Result(vmaCreateAllocator(&vmaInfo, &(VmaAllocator&)this->allocator)), "Failed to create VMA allocator...");
        };
    };

    // 
    std::shared_ptr<MemoryAllocatorVma> Device::createAllocatorVma() {
        if (!this->allocator) {
            this->allocator = std::pointer_dynamic_cast<MemoryAllocator>(std::make_shared<MemoryAllocatorVma>(shared_from_this()));
        };
        return std::pointer_dynamic_cast<MemoryAllocatorVma>(this->allocator);
    };

    //
    vk::Fence Queue::submitCmds(const std::vector<vk::CommandBuffer>& commandBuffers, vk::SubmitInfo2KHR submitInfo) const {
        if (commandBuffers.size() <= 0) return vk::Fence{};

        // 
        vk::Fence fence = this->device.lock()->getDevice().createFence(vk::FenceCreateInfo{});

        // 
        std::vector<vk::CommandBufferSubmitInfoKHR> commandInfos = {};
        for (auto& commandBuffer : commandBuffers) {
            commandInfos.push_back(vk::CommandBufferSubmitInfoKHR{
                .commandBuffer = commandBuffer,
                .deviceMask = ~0x0u
            });
        };

        // 
        submitInfo.commandBufferInfoCount = commandInfos.size();
        submitInfo.pCommandBufferInfos = commandInfos.data();

        // 
        this->queue.submit2KHR(submitInfo, fence);
        return fence;
    };

    //
    std::future<vk::Result> Queue::submitOnce(const std::function<void(const vk::CommandBuffer&)>& cmdFn, const vk::SubmitInfo2KHR& submitInfo) const {
        auto vkDevice = this->device.lock()->getDevice();
        auto commandBuffers = vkDevice.allocateCommandBuffers(vk::CommandBufferAllocateInfo{
            .commandPool = commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1
        });
        cmdFn(commandBuffers[0]); // execute command constructor
        auto fence = this->submitCmds(commandBuffers, submitInfo);
        return std::async(std::launch::async | std::launch::deferred, [this, fence, commandBuffers, vkDevice](){
            auto result = vkDevice.waitForFences({fence}, true, 30ull * 1000ull * 1000ull * 1000ull);
            vkDevice.destroyFence(fence);
            vkDevice.freeCommandBuffers(commandPool, commandBuffers);
            return result;
        });
    };

};
