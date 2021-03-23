#pragma once

//
#include <tommy/internal/core.hpp>
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>

// 
namespace tom {

    //
    std::shared_ptr<PhysicalDevice> PhysicalDevice::constructor() {
        this->properties = PhysicalDeviceProperties{};
        this->features = PhysicalDeviceFeatures{};

        // 
        auto propertiesChain = PhysicalDevicePropertiesChain(this->properties.properties);
        auto featuresChain = PhysicalDeviceFeaturesChain(this->features.features, this->features.bufferDeviceAddressFeatures);

        // 
        physicalDevice.getProperties2(this->propertiesChain = propertiesChain);
        physicalDevice.getFeatures2(this->featuresChain = featuresChain);

        // 
        this->memoryProperties = physicalDevice.getMemoryProperties2();
        this->queueFamilyProperties = physicalDevice.getQueueFamilyProperties2();
        this->extensionProperties = physicalDevice.enumerateDeviceExtensionProperties();
        this->layerProperties = physicalDevice.enumerateDeviceLayerProperties();

        //
        return shared_from_this();
    };

    //
    std::unordered_map<uint32_t, SurfaceProperties> PhysicalDevice::getSurfaceInfo(const vk::SurfaceKHR& surface) const {
        std::unordered_map<uint32_t, SurfaceProperties> surfaceInfo = {};
        uint32_t I=0u; for (auto& property : queueFamilyProperties) { const uint32_t i = I++;
            SurfaceProperties props = {};
            props.supported = physicalDevice.getSurfaceSupportKHR(i, surface);
            props.capabilities = physicalDevice.getSurfaceCapabilities2KHR(vk::PhysicalDeviceSurfaceInfo2KHR{ .surface = surface });
            props.formats = physicalDevice.getSurfaceFormats2KHR(vk::PhysicalDeviceSurfaceInfo2KHR{ .surface = surface });
            props.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
            surfaceInfo[i] = props;
        };
        return surfaceInfo;
    };

    //
    uint32_t PhysicalDevice::getMemoryType(const uint32_t& memoryTypeBitsRequirement, const vk::MemoryPropertyFlags& requiredProperties = vk::MemoryPropertyFlagBits::eDeviceLocal) const {
        const uint32_t memoryCount = memoryProperties.memoryProperties.memoryTypeCount;
        for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) {
            const uint32_t memoryTypeBits = (1 << memoryIndex);
            const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;
            const auto properties = memoryProperties.memoryProperties.memoryTypes[memoryIndex].propertyFlags;
            const bool hasRequiredProperties = (properties & requiredProperties) == requiredProperties;
            if (isRequiredMemoryType && hasRequiredProperties) return static_cast<uint32_t>(memoryIndex);
        };
        return 0u;
    };


};
