#pragma once

// 
#include <tommy/internal/core.hpp>
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/buffer.hpp>


// 
namespace tom {

    // 
    const std::vector<const char*> defaultLayers = {};
    const std::vector<const char*> defaultExtensions = { "VK_KHR_swapchain", "VK_KHR_buffer_device_address" };

    // 
    std::shared_ptr<Device> Device::constructor() {
        this->descriptions = std::make_shared<tom::DescriptorSetSource>();
        this->descriptorSets = std::make_shared<tom::DescriptorSet>();
        this->descriptorSetLayouts = std::make_shared<tom::DescriptorSetLayouts>();

        // 
        std::vector<const char*> preferedLayers = defaultLayers;
        std::vector<const char*> preferedExtensions = defaultExtensions;

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
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {};

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
        this->dispatch = vk::DispatchLoaderDynamic( this->getInstance()->getInstance(), vkGetInstanceProcAddr, this->device = this->physical->getPhysicalDevice().createDevice(vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceFeatures2>{
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
        for (int i=0;i<7;i++) {
            this->descriptions->typedImageViews.push_back({});
        };

        //
        return shared_from_this();
    };


    // 
    std::shared_ptr<DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer) const {
        return (this->buffers.find(buffer) != this->buffers.end()) ? this->buffers.at(buffer) : std::shared_ptr<DeviceBuffer>{};
    };

    // 
    std::shared_ptr<DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const {
        return (this->memories.find(deviceMemory) != this->memories.end()) ? this->memories.at(deviceMemory) : std::shared_ptr<DeviceMemory>{};
    };

    // 
    std::shared_ptr<ImageView> Device::getImageViewObject(const ImageViewKey& imageViewKey) const {
        return (this->imageViews.find(imageViewKey) != this->imageViews.end()) ? this->imageViews.at(imageViewKey) : std::shared_ptr<ImageView>{};
    };

    // 
    std::shared_ptr<DeviceMemory> Device::allocateMemoryObject(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {}) {
        auto deviceMemoryObj = std::make_shared<DeviceMemory>(shared_from_this())->allocate(allocator, info);
        auto deviceMemory = deviceMemoryObj->getMemory(); // determine a key
        return (this->memories[deviceMemory] = deviceMemoryObj);
    };


    // 
    std::shared_ptr<ImageView> Device::getImageViewObject(const ImageViewKey& imageViewKey) {
        std::shared_ptr<ImageView> imageView = {};

        if (this->imageViews.find(imageViewKey) != this->imageViews.end()) {
            imageView = this->imageViews.at(imageViewKey);
        };

        if (!imageView) { 
            this->imageViews[imageViewKey] = (imageView = std::make_shared<ImageView>(shared_from_this(), imageView));
        };

        return imageView;
    };

    // 
    std::shared_ptr<DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer) {
        std::shared_ptr<DeviceBuffer> deviceBuffer = {};

        if (this->buffers.find(buffer) != this->buffers.end()) {
            deviceBuffer = this->buffers.at(buffer);
        };

        if (!deviceBuffer) { 
            this->buffers[buffer] = (deviceBuffer = std::make_shared<DeviceBuffer>(shared_from_this(), buffer));
        };

        return deviceBuffer;
    };

    // 
    std::shared_ptr<DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) {
        std::shared_ptr<DeviceMemory> deviceMemoryObj = {};

        if (this->memories.find(deviceMemory) != this->memories.end()) {
            deviceMemoryObj = this->memories.at(deviceMemory);
        };

        if (!deviceMemoryObj) { 
            this->memories[deviceMemory] = (deviceMemoryObj = std::make_shared<DeviceMemory>(shared_from_this(), deviceMemory));
        };

        return deviceMemoryObj;
    };

    //
    std::shared_ptr<BufferAllocation> Device::getBufferAllocationObject(const vk::DeviceAddress& deviceAddress = 0ull) {
        std::shared_ptr<BufferAllocation> bufferAllocation = {};

        if (this->bufferAllocations.find(deviceAddress) != this->bufferAllocations.end()) {
            bufferAllocation = this->bufferAllocations.at(deviceAddress);
        };

        if (!bufferAllocation) {
            this->bufferAllocations[deviceAddress] = (bufferAllocation = std::make_shared<BufferAllocation>());
            bufferAllocation->getDeviceAddressDefined() = deviceAddress;
        };

        return bufferAllocation;
    };



    // 
    vk::Buffer Device::setDeviceBufferObject(const std::shared_ptr<DeviceBuffer>& deviceBuffer = {}) {
        vk::Buffer buffer = deviceBuffer->getBuffer(); // determine key
        if (this->buffers.find(buffer) == this->buffers.end()) {
            this->buffers[buffer] = deviceBuffer;
        };
        return buffer;
    };

    // 
    vk::DeviceMemory Device::setDeviceMemoryObject(const std::shared_ptr<DeviceMemory>& deviceMemoryObj = {}) {
        vk::DeviceMemory deviceMemory = deviceMemoryObj->getMemory(); // determine key
        if (this->memories.find(deviceMemory) == this->memories.end()) { 
            this->memories[deviceMemory] = deviceMemoryObj;
        };
        return deviceMemory;
    };

    // 
    ImageViewKey Device::setImageViewObject(const std::shared_ptr<ImageView>& imageViewObj = {}) {
        ImageViewKey imageViewKey = {}; // determine key
        if (this->imageViews.find(imageViewKey) == this->imageViews.end()) { 
            this->imageViews[imageViewKey] = imageViewObj;
        };
        return imageViewKey;
    };

    //
    vk::DeviceAddress Device::setBufferAllocationObject(const std::shared_ptr<BufferAllocation>& bufferAllocation = {}) {
        vk::DeviceAddress deviceAddress = bufferAllocation->getDeviceAddressDefined(); // determine key
        if (this->bufferAllocations.find(deviceAddress) == this->bufferAllocations.end()) {
            this->bufferAllocations[deviceAddress] = bufferAllocation;
        };
        return deviceAddress;
    };

};
