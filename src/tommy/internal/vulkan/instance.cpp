#pragma once

//
#include <tommy/internal/core.hpp>
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>

// 
namespace tom {
    namespace vulkan {

        // 
        const std::vector<const char*> defaultExtensions = { "VK_KHR_surface","VK_KHR_win32_surface" };
        const std::vector<const char*> defaultLayers = {  };

        // 
        std::shared_ptr<Instance> Instance::constructor() {
            vk::ApplicationInfo application_info = vk::ApplicationInfo{
                .pApplicationName = "Tommy Based App",
                .applicationVersion = VK_MAKE_VERSION(1,0,0),
                .pEngineName = "Tommy Library",
                .engineVersion = VK_MAKE_VERSION(1,0,0),
                .apiVersion = VK_MAKE_VERSION(1,2,0)
            };

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
            if (!context) { context = std::make_shared<Context>(); };

            // 
            for (uintptr_t i=0;i<preferedExtensions.size();i++) {
                const auto& name = preferedExtensions[i];
                bool found = false;
                for (auto& prop : context->extensionProperties) {
                    if (found = found || (strcmp(name, prop.extensionName) == 0)) break;
                };
                if (!found) {
                    excludedExtensions.push_back(name);
                    preferedExtensions.erase(preferedExtensions.begin()+i);
                };
            };

            // 
            for (uintptr_t i=0;i<preferedLayers.size();i++) {
                const auto& name = preferedLayers[i];
                bool found = false;
                for (auto& prop : context->layerProperties) {
                    if (found = found || (strcmp(name, prop.layerName) == 0)) break;
                };
                if (!found) {
                    excludedLayers.push_back(name);
                    preferedLayers.erase(preferedLayers.begin()+i);
                };
            };

            // 
            data->dispatch = vk::DispatchLoaderDynamic(data->instance = vk::createInstance(vk::InstanceCreateInfo{
                .pApplicationInfo = &application_info,
                .enabledLayerCount = static_cast<uint32_t>(preferedLayers.size()),
                .ppEnabledLayerNames = preferedLayers.data(),
                .enabledExtensionCount = static_cast<uint32_t>(preferedExtensions.size()),
                .ppEnabledExtensionNames = preferedExtensions.data()
            }), vkGetInstanceProcAddr);

            // 
            return shared_from_this();
        };

        //
        std::vector<std::shared_ptr<PhysicalDevice>>& Instance::enumeratePhysicalDevices() {
            if (data->physicalDevices.size() > 0) {
                std::vector<vk::PhysicalDevice> devices = data->instance.enumeratePhysicalDevices();
                for (auto& device : devices) {
                    data->physicalDevices.push_back(std::make_shared<PhysicalDevice>(shared_from_this(), device));
                };
            };
            return data->physicalDevices;
        };

        //
        const std::vector<std::shared_ptr<PhysicalDevice>>& Instance::enumeratePhysicalDevices() const {
            return data->physicalDevices;
        };

    };

};
