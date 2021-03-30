#pragma once

// 
#include <tommy/internal/core.hpp>
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>
#include <tommy/internal/vulkan/buffer.hpp>


// 
namespace tom {

    // 
    namespace vulkan {
        // 
        const std::vector<const char*> defaultLayers = {};
        const std::vector<const char*> defaultExtensions = { 
            "VK_KHR_swapchain",
            "VK_KHR_buffer_device_address",
            "VK_KHR_copy_commands2",
            "VK_KHR_create_renderpass2",
            "VK_KHR_bind_memory2",
            "VK_KHR_synchronization2",
            "VK_KHR_shader_float16_int8",
            "VK_KHR_shader_draw_parameters",
            "VK_KHR_shader_subgroup_extended_types",
            "VK_KHR_vulkan_memory_model",
            "VK_KHR_workgroup_memory_explicit_layout",
            "VK_KHR_zero_initialize_workgroup_memory",
            "VK_KHR_get_memory_requirements2",
            "VK_KHR_relaxed_block_layout",
            "VK_KHR_shader_atomic_int64",
            "VK_KHR_variable_pointers",
            "VK_KHR_dedicated_allocation",
            "VK_KHR_timeline_semaphore",

            "VK_KHR_acceleration_structure",
            "VK_KHR_ray_tracing_pipeline",
            "VK_KHR_ray_query",
            "VK_KHR_pipeline_library",
            "VK_KHR_pipeline_executable_properties",
            "VK_KHR_deferred_host_operations",

            "VK_EXT_subgroup_size_control",
            "VK_EXT_transform_feedback",
            "VK_EXT_validation_cache",
            "VK_EXT_scalar_block_layout",
            "VK_EXT_sampler_filter_minmax",
            "VK_EXT_sample_locations",
            "VK_EXT_queue_family_foreign",
            "VK_EXT_index_type_uint8",
            "VK_EXT_depth_range_unrestricted",
            "VK_EXT_depth_clip_enable",
            "VK_EXT_descriptor_indexing",
            "VK_EXT_conservative_rasterization",
            "VK_EXT_extended_dynamic_state"
        };

        // 
        std::shared_ptr<tom::Device> Device::constructor() {
            auto data = this->getDataTyped();
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
            auto physical = data->physicalDevices[0];

            // 
            const auto& extensions = std::dynamic_pointer_cast<PhysicalDeviceData>(physical->getData())->extensionProperties;//getExtensionPropertiesDefined();
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
            const auto& layers = std::dynamic_pointer_cast<PhysicalDeviceData>(physical->getData())->layerProperties;
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
            auto& queueFamilyProperties = std::dynamic_pointer_cast<PhysicalDeviceData>(physical->getData())->queueFamilyProperties;
            uint32_t I=0u; for (auto& property : queueFamilyProperties) { const uint32_t i = I++;
                std::vector<float> queue_priorities = { 1.f }; // queues per every family
                if (property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute) {
                    data->queues[i] = std::vector<std::shared_ptr<tom::Queue>>{};
                    queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{
                        .queueFamilyIndex = i,
                        .queueCount = static_cast<uint32_t>(queue_priorities.size()),
                        .pQueuePriorities = queue_priorities.data()
                    });
                    data->queueFamilyIndices.push_back(i);
                };
            };

            // 
            data->dispatch = vk::DispatchLoaderDynamic( 
                std::dynamic_pointer_cast<InstanceData>(this->getInstance()->getData())->instance, vkGetInstanceProcAddr, 
                data->device = std::dynamic_pointer_cast<PhysicalDeviceData>(physical->getData())->physicalDevice.createDevice(vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceFeatures2>{
                vk::DeviceCreateInfo{
                    .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                    .pQueueCreateInfos = queueCreateInfos.data(),
                    .enabledLayerCount = static_cast<uint32_t>(preferedLayers.size()),
                    .ppEnabledLayerNames = preferedLayers.data(),
                    .enabledExtensionCount = static_cast<uint32_t>(preferedExtensions.size()),
                    .ppEnabledExtensionNames = preferedExtensions.data()
                }, std::dynamic_pointer_cast<PhysicalDeviceData>(physical->getData())->featuresChain.get<vk::PhysicalDeviceFeatures2>()
            }.get<vk::DeviceCreateInfo>()), vkGetDeviceProcAddr );

            // 
            for (uint32_t i=0u;i<queueCreateInfos.size();i++) {
                const auto& info = queueCreateInfos[i];
                const auto& queueFamilyIndex = info.queueFamilyIndex;
                for (uint32_t j=0;j<info.queueCount;j++) {
                    data->queues[info.queueFamilyIndex].push_back(std::make_shared<Queue>(shared_from_this(), QueueData::makeShared(data->device.getQueue(queueFamilyIndex, j), queueFamilyIndex)));
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
        std::shared_ptr<tom::BufferAllocation> Device::getBufferAllocationObject(const uintptr_t& address) const {
            auto data = this->getDataTyped();
            return (data->bufferAllocations.find(address) != data->bufferAllocations.end()) ? data->bufferAllocations.at(address) : std::shared_ptr<tom::BufferAllocation>{};
        };

        // 
        std::shared_ptr<tom::DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer) const {
            auto data = this->getDataTyped();
            return (data->buffers.find(buffer) != data->buffers.end()) ? data->buffers.at(buffer) : std::shared_ptr<tom::DeviceBuffer>{};
        };

        // 
        std::shared_ptr<tom::DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const {
            auto data = this->getDataTyped();
            return (data->memories.find(deviceMemory) != data->memories.end()) ? data->memories.at(deviceMemory) : std::shared_ptr<tom::DeviceMemory>{};
        };

        // 
        std::shared_ptr<tom::ImageView> Device::getImageViewObject(const ImageViewKey& imageViewKey) const {
            auto data = this->getDataTyped();
            return (data->imageViews.find(imageViewKey) != data->imageViews.end()) ? std::dynamic_pointer_cast<tom::ImageView>(data->imageViews.at(imageViewKey)) : std::shared_ptr<tom::ImageView>{};
        };

        // 
        std::shared_ptr<tom::DeviceMemory> Device::allocateMemoryObject(const std::shared_ptr<tom::MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {}) {
            auto data = this->getDataTyped();
            auto deviceMemoryObj = std::make_shared<DeviceMemory>(shared_from_this())->allocate(std::dynamic_pointer_cast<MemoryAllocator>(allocator), info);
            auto deviceMemory = std::dynamic_pointer_cast<DeviceMemoryApi>(deviceMemoryObj->getApi())->memory; // determine a key
            return (data->memories[deviceMemory] = deviceMemoryObj);
        };


        // 
        std::shared_ptr<tom::ImageView> Device::getImageViewObject(const ImageViewKey& imageViewKey) {
            auto data = this->getDataTyped();
            std::shared_ptr<ImageView> imageView = {};

            if (data->imageViews.find(imageViewKey) != data->imageViews.end()) {
                imageView = data->imageViews.at(imageViewKey);
            };

            if (!imageView) { 
                //data->imageViews[imageViewKey] = (imageView = std::make_shared<ImageView>(std::make_shared<DeviceImage>(shared_from_this(), image)));
            };

            return std::dynamic_pointer_cast<tom::ImageView>(imageView);
        };

        // 
        std::shared_ptr<tom::DeviceBuffer> Device::getDeviceBufferObject(const vk::Buffer& buffer) {
            auto data = this->getDataTyped();
            std::shared_ptr<DeviceBuffer> deviceBuffer = {};

            if (data->buffers.find(buffer) != data->buffers.end()) {
                deviceBuffer = data->buffers.at(buffer);
            };

            if (!deviceBuffer) { 
                data->buffers[buffer] = (deviceBuffer = std::make_shared<DeviceBuffer>(shared_from_this(), DeviceBufferData::makeShared(buffer)));
            };

            return deviceBuffer;
        };

        // 
        std::shared_ptr<tom::DeviceMemory> Device::getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) {
            auto data = this->getDataTyped();
            std::shared_ptr<DeviceMemory> deviceMemoryObj = {};

            if (data->memories.find(deviceMemory) != data->memories.end()) {
                deviceMemoryObj = data->memories.at(deviceMemory);
            };

            if (!deviceMemoryObj) { 
                data->memories[deviceMemory] = (deviceMemoryObj = std::make_shared<DeviceMemory>(shared_from_this(), DeviceMemoryApi::makeShared(deviceMemory)));
            };

            return deviceMemoryObj;
        };

        //
        std::shared_ptr<tom::BufferAllocation> Device::getBufferAllocationObject(const uintptr_t& deviceAddress = 0ull) {
            auto data = this->getDataTyped();
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
        vk::Buffer Device::setDeviceBufferObject(const std::shared_ptr<tom::DeviceBuffer>& deviceBuffer = {}) {
            auto data = this->getDataTyped();
            vk::Buffer buffer = std::dynamic_pointer_cast<DeviceBufferData>(deviceBuffer->getApi())->buffer; // determine key
            if (data->buffers.find(buffer) == data->buffers.end()) {
                data->buffers[buffer] = deviceBuffer;
            };
            return buffer;
        };

        // 
        vk::DeviceMemory Device::setDeviceMemoryObject(const std::shared_ptr<tom::DeviceMemory>& deviceMemoryObj = {}) {
            auto data = this->getDataTyped();
            vk::DeviceMemory deviceMemory = std::dynamic_pointer_cast<DeviceMemoryApi>(deviceMemoryObj->getApi())->memory; // determine key
            if (data->memories.find(deviceMemory) == data->memories.end()) {
                data->memories[deviceMemory] = deviceMemoryObj;
            };
            return deviceMemory;
        };

        // 
        ImageViewKey Device::setImageViewObject(const std::shared_ptr<tom::ImageView>& imageViewObj = {}) {
            auto data = this->getDataTyped();
            ImageViewKey imageViewKey = {}; // TODO: determine key
            if (data->imageViews.find(imageViewKey) == data->imageViews.end()) {
                data->imageViews[imageViewKey] = imageViewObj;
            };
            return imageViewKey;
        };

        //
        uintptr_t Device::setBufferAllocationObject(const std::shared_ptr<tom::BufferAllocation>& bufferAllocation = {}) {
            auto data = this->getDataTyped();
            auto deviceAddress = std::dynamic_pointer_cast<BufferAllocation>(bufferAllocation)->getDeviceAddressDefined(); // determine key
            if (data->bufferAllocations.find(deviceAddress) == data->bufferAllocations.end()) {
                data->bufferAllocations[deviceAddress] = bufferAllocation;
            };
            return deviceAddress;
        };

    };

};
