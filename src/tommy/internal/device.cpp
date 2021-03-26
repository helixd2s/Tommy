#pragma once

// 
#include <tommy/internal/core.hpp>
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/buffer.hpp>


// 
namespace tom {
    namespace vktm {
        // 
        const std::vector<const char*> defaultLayers = {};
        const std::vector<const char*> defaultExtensions = { "VK_KHR_swapchain", "VK_KHR_buffer_device_address" };

        // 
        std::shared_ptr<Device> Device::constructor() {
            data->descriptions = std::make_shared<DescriptorSetSource>();
            data->descriptorSets = std::make_shared<DescriptorSet>();
            data->descriptorSetLayouts = std::make_shared<DescriptorSetLayouts>();

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
            const auto& extensions = physical->getData()->extensionProperties;//getExtensionPropertiesDefined();
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
            const auto& layers = physical->getData()->layerProperties;
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
            auto& queueFamilyProperties = physical->getData()->queueFamilyProperties;
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
            data->dispatch = vk::DispatchLoaderDynamic( this->getInstance()->getData()->instance, vkGetInstanceProcAddr, data->device = this->physical->getData()->physicalDevice.createDevice(vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceFeatures2>{
                vk::DeviceCreateInfo{
                    .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                    .pQueueCreateInfos = queueCreateInfos.data(),
                    .enabledLayerCount = static_cast<uint32_t>(preferedLayers.size()),
                    .ppEnabledLayerNames = preferedLayers.data(),
                    .enabledExtensionCount = static_cast<uint32_t>(preferedExtensions.size()),
                    .ppEnabledExtensionNames = preferedExtensions.data()
                }, this->physical->getData()->featuresChain.get<vk::PhysicalDeviceFeatures2>()
            }.get<vk::DeviceCreateInfo>()), vkGetDeviceProcAddr );

            // 
            for (uint32_t i=0u;i<queueCreateInfos.size();i++) {
                const auto& info = queueCreateInfos[i];
                const auto& queueFamilyIndex = info.queueFamilyIndex;
                for (uint32_t j=0;j<info.queueCount;j++) {
                    queues[info.queueFamilyIndex].push_back(std::make_shared<Queue>(shared_from_this(), data->device.getQueue(queueFamilyIndex, j), queueFamilyIndex));
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
            data->descriptorPool = data->device.createDescriptorPool(vk::StructureChain<vk::DescriptorPoolCreateInfo, vk::DescriptorPoolInlineUniformBlockCreateInfoEXT>(
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
                data->descriptions->typedImageViews.push_back({});
            };

            //
            return shared_from_this();
        };


        // 
        std::shared_ptr<DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer) const {
            return (data->buffers.find(buffer) != data->buffers.end()) ? data->buffers.at(buffer) : std::shared_ptr<DeviceBuffer>{};
        };

        // 
        std::shared_ptr<DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const {
            return (data->memories.find(deviceMemory) != data->memories.end()) ? data->memories.at(deviceMemory) : std::shared_ptr<DeviceMemory>{};
        };

        // 
        std::shared_ptr<ImageView> Device::getImageViewObject(const ImageViewKey& imageViewKey) const {
            return (data->imageViews.find(imageViewKey) != data->imageViews.end()) ? data->imageViews.at(imageViewKey) : std::shared_ptr<ImageView>{};
        };

        // 
        std::shared_ptr<DeviceMemory> Device::allocateMemoryObject(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {}) {
            auto deviceMemoryObj = std::make_shared<DeviceMemory>(shared_from_this())->allocate(allocator, info);
            auto deviceMemory = deviceMemoryObj->getMemory(); // determine a key
            return (data->memories[deviceMemory] = deviceMemoryObj);
        };


        // 
        std::shared_ptr<ImageView> Device::getImageViewObject(const ImageViewKey& imageViewKey) {
            std::shared_ptr<ImageView> imageView = {};

            if (data->imageViews.find(imageViewKey) != data->imageViews.end()) {
                imageView = data->imageViews.at(imageViewKey);
            };

            if (!imageView) { 
                data->imageViews[imageViewKey] = (imageView = std::make_shared<ImageView>(shared_from_this(), imageView));
            };

            return imageView;
        };

        // 
        std::shared_ptr<DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer) {
            std::shared_ptr<DeviceBuffer> deviceBuffer = {};

            if (data->buffers.find(buffer) != data->buffers.end()) {
                deviceBuffer = data->buffers.at(buffer);
            };

            if (!deviceBuffer) { 
                data->buffers[buffer] = (deviceBuffer = std::make_shared<DeviceBuffer>(shared_from_this(), buffer));
            };

            return deviceBuffer;
        };

        // 
        std::shared_ptr<DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) {
            std::shared_ptr<DeviceMemory> deviceMemoryObj = {};

            if (data->memories.find(deviceMemory) != data->memories.end()) {
                deviceMemoryObj = data->memories.at(deviceMemory);
            };

            if (!deviceMemoryObj) { 
                data->memories[deviceMemory] = (deviceMemoryObj = std::make_shared<DeviceMemory>(shared_from_this(), deviceMemory));
            };

            return deviceMemoryObj;
        };

        //
        std::shared_ptr<BufferAllocation> Device::getBufferAllocationObject(const vk::DeviceAddress& deviceAddress = 0ull) {
            std::shared_ptr<BufferAllocation> bufferAllocation = {};

            if (data->bufferAllocations.find(deviceAddress) != data->bufferAllocations.end()) {
                bufferAllocation = data->bufferAllocations.at(deviceAddress);
            };

            if (!bufferAllocation) {
                data->bufferAllocations[deviceAddress] = (bufferAllocation = std::make_shared<BufferAllocation>());
                bufferAllocation->getDeviceAddressDefined() = deviceAddress;
            };

            return bufferAllocation;
        };



        // 
        vk::Buffer Device::setDeviceBufferObject(const std::shared_ptr<DeviceBuffer>& deviceBuffer = {}) {
            vk::Buffer buffer = deviceBuffer->getBuffer(); // determine key
            if (data->buffers.find(buffer) == data->buffers.end()) {
                data->buffers[buffer] = deviceBuffer;
            };
            return buffer;
        };

        // 
        vk::DeviceMemory Device::setDeviceMemoryObject(const std::shared_ptr<DeviceMemory>& deviceMemoryObj = {}) {
            vk::DeviceMemory deviceMemory = deviceMemoryObj->getMemory(); // determine key
            if (data->memories.find(deviceMemory) == data->memories.end()) { 
                data->memories[deviceMemory] = deviceMemoryObj;
            };
            return deviceMemory;
        };

        // 
        ImageViewKey Device::setImageViewObject(const std::shared_ptr<ImageView>& imageViewObj = {}) {
            ImageViewKey imageViewKey = {}; // determine key
            if (data->imageViews.find(imageViewKey) == data->imageViews.end()) { 
                data->imageViews[imageViewKey] = imageViewObj;
            };
            return imageViewKey;
        };

        //
        vk::DeviceAddress Device::setBufferAllocationObject(const std::shared_ptr<BufferAllocation>& bufferAllocation = {}) {
            vk::DeviceAddress deviceAddress = bufferAllocation->getDeviceAddressDefined(); // determine key
            if (data->bufferAllocations.find(deviceAddress) == data->bufferAllocations.end()) {
                data->bufferAllocations[deviceAddress] = bufferAllocation;
            };
            return deviceAddress;
        };

    };

};
