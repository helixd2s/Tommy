#pragma once

//
#include <tommy/internal/core.hpp>
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>

// 
namespace tom {

    // 
    namespace vulkan {
        //
        std::shared_ptr<tom::PhysicalDevice> PhysicalDevice::constructor() {
            auto data = this->getDataTyped();

            // 
            data->properties = PhysicalDeviceProperties{};
            data->features = PhysicalDeviceFeatures{};

            // 
            auto propertiesChain = PhysicalDevicePropertiesChain(data->properties.properties);
            auto featuresChain = PhysicalDeviceFeaturesChain(data->features.features, data->features.bufferDeviceAddressFeatures);

            // 
            data->physicalDevice.getProperties2(data->propertiesChain = propertiesChain);
            data->physicalDevice.getFeatures2(data->featuresChain = featuresChain);

            // 
            data->memoryProperties = data->physicalDevice.getMemoryProperties2();
            data->queueFamilyProperties = data->physicalDevice.getQueueFamilyProperties2();
            data->extensionProperties = data->physicalDevice.enumerateDeviceExtensionProperties();
            data->layerProperties = data->physicalDevice.enumerateDeviceLayerProperties();

            //
            return shared_from_this();
        };

        //
        std::unordered_map<uint32_t, SurfaceProperties> PhysicalDevice::getSurfaceInfo(const vk::SurfaceKHR& surface) const {
            auto data = this->getDataTyped();
            std::unordered_map<uint32_t, SurfaceProperties> surfaceInfo = {};
            uint32_t I=0u; for (auto& property : data->queueFamilyProperties) { const uint32_t i = I++;
                SurfaceProperties props = {};
                props.supported = data->physicalDevice.getSurfaceSupportKHR(i, surface);
                props.capabilities = data->physicalDevice.getSurfaceCapabilities2KHR(vk::PhysicalDeviceSurfaceInfo2KHR{ .surface = surface });
                props.formats = data->physicalDevice.getSurfaceFormats2KHR(vk::PhysicalDeviceSurfaceInfo2KHR{ .surface = surface });
                props.presentModes = data->physicalDevice.getSurfacePresentModesKHR(surface);
                surfaceInfo[i] = props;
            };
            return surfaceInfo;
        };

        //
        uint32_t PhysicalDevice::getMemoryType(const uint32_t& memoryTypeBitsRequirement, const vk::MemoryPropertyFlags& requiredProperties = vk::MemoryPropertyFlagBits::eDeviceLocal) const {
            auto data = this->getDataTyped();
            const uint32_t memoryCount = data->memoryProperties.memoryProperties.memoryTypeCount;
            for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) {
                const uint32_t memoryTypeBits = (1 << memoryIndex);
                const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;
                const auto properties = data->memoryProperties.memoryProperties.memoryTypes[memoryIndex].propertyFlags;
                const bool hasRequiredProperties = (properties & requiredProperties) == requiredProperties;
                if (isRequiredMemoryType && hasRequiredProperties) return static_cast<uint32_t>(memoryIndex);
            };
            return 0u;
        };
    };

};
