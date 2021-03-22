#pragma once

// 
#include <tommy/internal/device.hpp>
#include <tommy/internal/buffer.hpp>
#include <tommy/internal/memory.hpp>

// 
#include <vulkan/vk_mem_alloc.h>

// 
namespace tom {

    // 
    std::shared_ptr<DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer) const {
        std::shared_ptr<DeviceBuffer> deviceBuffer = {};

        if (buffers.find(buffer) != buffers.end()) {
            deviceBuffer = buffers.at(buffer).lock();
        };

        return deviceBuffer;
    };

    // 
    std::shared_ptr<DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const {
        std::shared_ptr<DeviceMemory> deviceMemoryObj = {};

        if (memories.find(deviceMemory) != memories.end()) {
            deviceMemoryObj = memories.at(deviceMemory).lock();
        };

        return deviceMemoryObj;
    };

    // 
    std::shared_ptr<DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer) {
        std::shared_ptr<DeviceBuffer> deviceBuffer = {};

        if (buffers.find(buffer) != buffers.end()) {
            deviceBuffer = buffers.at(buffer).lock();
        };

        if (!deviceBuffer) { 
            deviceBuffer = std::make_shared<DeviceBuffer>(shared_from_this(), buffer);
            buffers.insert(std::make_pair<vk::Buffer, std::weak_ptr<DeviceBuffer>>(vk::Buffer(buffer), std::weak_ptr<DeviceBuffer>(deviceBuffer)));
        };

        return deviceBuffer;
    };

    // 
    std::shared_ptr<DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) {
        std::shared_ptr<DeviceMemory> deviceMemoryObj = {};

        if (memories.find(deviceMemory) != memories.end()) {
            deviceMemoryObj = memories.at(deviceMemory).lock();
        };

        if (!deviceMemoryObj) { 
            deviceMemoryObj = std::make_shared<DeviceMemory>(shared_from_this(), deviceMemory);
            memories.insert(std::make_pair<vk::DeviceMemory, std::weak_ptr<DeviceMemory>>(vk::DeviceMemory(deviceMemory), std::weak_ptr<DeviceMemory>(deviceMemoryObj)));
        };

        return deviceMemoryObj;
    };

    // 
    std::shared_ptr<DeviceMemory> Device::allocateMemoryObject(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {}) {
        std::shared_ptr<DeviceMemory> deviceMemoryObj = std::make_shared<DeviceMemory>(shared_from_this());

        if (!deviceMemoryObj) {
            deviceMemoryObj->allocate(allocator, info);
            memories.insert(std::make_pair<vk::DeviceMemory, std::weak_ptr<DeviceMemory>>(vk::DeviceMemory(deviceMemoryObj->getMemory()), std::weak_ptr<DeviceMemory>(deviceMemoryObj)));
        };

        return deviceMemoryObj;
    };



    // 
    std::shared_ptr<MemoryAllocator> MemoryAllocator::allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = buffer;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        if (self->buffer) { // 
            auto memoryRequirements = vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirementsKHR>{ vk::MemoryRequirements2{  }, vk::MemoryDedicatedRequirementsKHR{} };
            device->getDevice().getBufferMemoryRequirements2(vk::BufferMemoryRequirementsInfo2{ .buffer = self->buffer }, memoryRequirements);

            // 
            const auto& memReqs = memoryRequirements.get<vk::MemoryRequirements2>().memoryRequirements;

            // 
            self->bindMemory(std::make_shared<tom::MemoryAllocation>(device->allocateMemoryObject(allocator, vk::StructureChain<vk::MemoryAllocateInfo, vk::MemoryDedicatedAllocateInfoKHR, vk::ExternalMemoryBufferCreateInfo>{
                vk::MemoryAllocateInfo{
                    .allocationSize = memReqs.size,
                    .memoryTypeIndex = device->getPhysicalDevice()->getMemoryType(memReqs.memoryTypeBits),
                },
                vk::MemoryDedicatedAllocateInfoKHR{ .buffer = self->buffer },
                vk::ExternalMemoryBufferCreateInfo{}
            }.get<vk::MemoryAllocateInfo>()), 0ull));
        };

        return shared_from_this();
    };

    //
    std::shared_ptr<MemoryAllocator> MemoryAllocator::allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        this->allocateBuffer(buffer->create(info), memUsage);
        return shared_from_this();
    };



    //
    std::shared_ptr<MemoryAllocator> MemoryAllocatorVma::allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = buffer;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        if (self->buffer) { // 
            VmaAllocationInfo allocInfo = {};
            VmaAllocationCreateInfo allocCreateInfo = { .usage = memUsage };
            if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
            };

            // 
            vk::throwResultException(vk::Result(vmaAllocateMemoryForBuffer((const VmaAllocator&)allocator->getAllocator(), self->buffer, &allocCreateInfo, &((VmaAllocation&)self->allocation), &allocInfo)), "VMA buffer allocation failed...");

            // TODO: use native allocation
            auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory); // TODO: IMPORTANT!!!
            self->bindMemory(std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset));
            self->memoryAllocation->getAllocation() = self->allocation;
            //self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData; // not sure...
            deviceMemory->getMapped() = allocInfo.pMappedData;

            //
            self->destructor = [self, allocator](){
                vmaFreeMemory((const VmaAllocator&)allocator->getAllocator(), (VmaAllocation&)self->getMemoryAllocation());
            };
        };

        return shared_from_this();
    };

    //
    std::shared_ptr<MemoryAllocator> MemoryAllocatorVma::allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = buffer;
        auto allocator = shared_from_this();
        auto device = this->device.lock();

        // 
        self->info = info;

        // 
        VmaAllocationCreateInfo allocCreateInfo = { .usage = memUsage };
        if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
            allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
        };

        // 
        vk::throwResultException(vk::Result(vmaCreateBuffer((const VmaAllocator&)allocator->getAllocator(), (const VkBufferCreateInfo*)&info, &allocCreateInfo, (VkBuffer*)&self->buffer, &((VmaAllocation&)self->allocation), nullptr)), "VMA buffer allocation failed...");

        // get allocation info
        VmaAllocationInfo allocInfo = {};
        vmaGetAllocationInfo((const VmaAllocator&)allocator->getAllocator(), ((VmaAllocation&)self->allocation), &allocInfo);

        // wrap device memory
        auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory);
        self->memoryAllocation = std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset);
        self->memoryAllocation->getAllocation() = self->allocation;
        //self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData; // not sure...
        deviceMemory->getMapped() = allocInfo.pMappedData;

        //deviceMemory->getAllocation() = allocation; // not sure...

        self->destructor = [self, allocator](){
            vmaDestroyBuffer((const VmaAllocator&)allocator->getAllocator(), self->getBuffer(), (VmaAllocation&)self->getMemoryAllocation());
            self->getBuffer() = vk::Buffer{};
            self->getMemoryAllocation() = nullptr;
        };

        return shared_from_this();
    };



    // 
    std::shared_ptr<MemoryAllocator>& Device::createAllocatorVma() {
        if (!this->allocator) {
            this->allocator = std::dynamic_pointer_cast<MemoryAllocator>(std::make_shared<MemoryAllocatorVma>(shared_from_this()));
        };
        return this->allocator;
        //return std::dynamic_pointer_cast<MemoryAllocatorVma>(this->allocator);
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



    //
    std::shared_ptr<MemoryAllocator> MemoryAllocatorVma::constructor() { //
        auto device = this->device.lock();
        auto& instanceDispatch = device->getInstance()->getDispatch();
        auto& deviceDispatch = device->getDispatch();

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
        vmaInfo.device = device->getDevice();
        vmaInfo.instance = device->getInstance()->getInstance();
        vmaInfo.physicalDevice = device->getPhysicalDevice()->getPhysicalDevice();
        vmaInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        // 
        vk::throwResultException(vk::Result(vmaCreateAllocator(&vmaInfo, &(VmaAllocator&)this->allocator)), "Failed to create VMA allocator...");

        //
        return shared_from_this();
    };



    //
    std::shared_ptr<Device> Device::constructor() {
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

        //
        return shared_from_this();
    };

};
