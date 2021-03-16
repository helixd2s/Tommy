#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"

// 
namespace tom {

    //
    using PhysicalDeviceFeaturesChain = vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceBufferDeviceAddressFeatures>;
    using PhysicalDevicePropertiesChain = vk::StructureChain<vk::PhysicalDeviceProperties2>;

    //
    struct PhysicalDeviceFeatures {
        vk::PhysicalDeviceFeatures2 features = {};
        vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
    };

    //
    struct PhysicalDeviceProperties {
        vk::PhysicalDeviceProperties2 properties = {};
    };

    // 
    class Instance: public std::enable_shared_from_this<Instance> {
    protected:  // 
        vk::Instance instance = {};
        std::unordered_map<uintptr_t, std::weak_ptr<Device>> deviceMap = {};

        // 
        std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices = {};

    public: // 
        Instance() {
            vk::ApplicationInfo application_info = vk::ApplicationInfo{
                .pApplicationName = "Tommy Based App",
                .applicationVersion = VK_MAKE_VERSION(1,0,0),
                .pEngineName = "Tommy Library",
                .engineVersion = VK_MAKE_VERSION(1,0,0),
                .apiVersion = VK_MAKE_VERSION(1,2,0)
            };

#ifdef NDEBUG
            std::vector<const char*> desired_validation_layers = {};
            std::vector<const char*> desired_instance_level_extensions = { "VK_KHR_surface","VK_KHR_win32_surface" };
#else
            std::vector<const char*> desired_validation_layers = { "VK_LAYER_LUNARG_standard_validation" };
            std::vector<const char*> desired_instance_level_extensions = { "VK_KHR_surface","VK_KHR_win32_surface","VK_EXT_debug_report" };
#endif

            vk::InstanceCreateInfo instance_create_info = vk::InstanceCreateInfo{
                .pApplicationInfo = &application_info,
                .enabledLayerCount = desired_validation_layers.size(),
                .ppEnabledLayerNames = desired_validation_layers.data(),
                .enabledExtensionCount = desired_instance_level_extensions.size(),
                .ppEnabledExtensionNames = desired_instance_level_extensions.data()
            };

            instance = vk::createInstance(instance_create_info);
        };

        // 
        std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices();
        vk::Instance& getInstance() { return instance; };

        // 
        const std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices() const;
        const vk::Instance& getInstance() const { return instance; };
    };

    // 
    std::unordered_map<uintptr_t, std::shared_ptr<Instance>> instanceMap = {};

    // 
    class PhysicalDevice: public std::enable_shared_from_this<PhysicalDevice> {
    protected:  // 
        std::weak_ptr<tom::Instance> instance = {};
        
        //
        vk::PhysicalDevice physicalDevice = {};

        // MEMORY
        PhysicalDeviceProperties properties = {};
        PhysicalDeviceFeatures features = {};

        // CHAIN (linked)
        PhysicalDevicePropertiesChain propertiesChain = {};
        PhysicalDeviceFeaturesChain featuresChain = {};

    public: // 
        PhysicalDevice(const std::shared_ptr<tom::Instance>& instance, const vk::PhysicalDevice& physicalDevice): instance(instance), physicalDevice(physicalDevice) {
            this->properties = PhysicalDeviceProperties{};
            this->features = PhysicalDeviceFeatures{};

            // 
            auto propertiesChain = PhysicalDevicePropertiesChain(this->properties.properties);
            auto featuresChain = PhysicalDeviceFeaturesChain(this->features.features, this->features.bufferDeviceAddressFeatures);

            // 
            physicalDevice.getProperties2(this->propertiesChain = propertiesChain);
            physicalDevice.getFeatures2(this->featuresChain = featuresChain);
        };

        //
        PhysicalDeviceProperties& getPropertiesDefined() { return properties; };
        PhysicalDeviceFeatures& getFeaturesDefined() { return features; };
        PhysicalDevicePropertiesChain& getPropertiesChainDefined() { return propertiesChain; };
        PhysicalDeviceFeaturesChain& getFeaturesChainDefined() { return featuresChain; };
        vk::PhysicalDevice& getPhysicalDevice() { return physicalDevice; };

        // 
        const PhysicalDeviceProperties& getPropertiesDefined() const { return properties; };
        const PhysicalDeviceFeatures& getFeaturesDefined() const { return features; };
        const PhysicalDevicePropertiesChain& getPropertiesChainDefined() const { return propertiesChain; };
        const PhysicalDeviceFeaturesChain& getFeaturesChainDefined() const { return featuresChain; };
        const vk::PhysicalDevice& getPhysicalDevice() const { return physicalDevice; };
        
    };


    //
    std::vector<std::shared_ptr<PhysicalDevice>>& Instance::enumeratePhysicalDevices() {
        if (physicalDevices.size() > 0) {
            std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
            for (auto& device : devices) {
                physicalDevices.push_back(std::make_shared<PhysicalDevice>(shared_from_this(), device));
            };
        };
        return physicalDevices;
    };

    //
    const std::vector<std::shared_ptr<PhysicalDevice>>& Instance::enumeratePhysicalDevices() const {
        return physicalDevices;
    };

    

};
