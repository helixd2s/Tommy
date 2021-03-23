#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/buffer.hpp>


// 
namespace tom {

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

};
