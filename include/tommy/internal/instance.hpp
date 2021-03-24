#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"

// 
namespace tom {

    //
    class Context: public std::enable_shared_from_this<Context> { public: 
        bool initialized = false;
        vk::DispatchLoaderDynamic dispatch = {};
        std::unordered_map<uintptr_t, std::shared_ptr<Instance>> instanceMap = {};
        std::vector<vk::ExtensionProperties> extensionProperties = {};
        std::vector<vk::LayerProperties> layerProperties = {};

    public: 
        Context() {
            this->initialize();
        };

        virtual void initialize();
    };

    // 
    class Instance: public std::enable_shared_from_this<Instance> {
    protected:  // 
        vk::Instance instance = {};
        vk::DispatchLoaderDynamic dispatch = {};

        // 
        std::unordered_map<uintptr_t, std::weak_ptr<Device>> deviceMap = {};

        // 
        std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices = {};

        //
        static std::shared_ptr<Context> context;

    public: // 
        Instance() { this->constructor(); };

        //
        virtual std::shared_ptr<Instance> constructor();

        // 
        virtual inline std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices();
        virtual inline vk::Instance& getInstance() { return instance; };
        virtual inline vk::DispatchLoaderDynamic& getDispatch() { return dispatch; };

        // 
        virtual inline const std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices() const;
        virtual inline const vk::Instance& getInstance() const { return instance; };
        virtual inline const vk::DispatchLoaderDynamic& getDispatch() const { return dispatch; };
    };

    //
    void Context::initialize() {
        auto& self = *this;
        if (!self.initialized) {
            self.dispatch = vk::DispatchLoaderDynamic(vkGetInstanceProcAddr);
            self.initialized = true;
            self.instanceMap = {};
            self.extensionProperties = vk::enumerateInstanceExtensionProperties();
            self.layerProperties = vk::enumerateInstanceLayerProperties();
        };
    };

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

        //
        vk::PhysicalDeviceMemoryProperties2 memoryProperties = {};
        std::vector<vk::QueueFamilyProperties2> queueFamilyProperties = {};
        std::vector<vk::ExtensionProperties> extensionProperties = {};
        std::vector<vk::LayerProperties> layerProperties = {};

    public: // 
        PhysicalDevice(const std::shared_ptr<tom::Instance>& instance, const vk::PhysicalDevice& physicalDevice): instance(instance), physicalDevice(physicalDevice) {
            this->constructor();
        };

        //
        virtual std::shared_ptr<PhysicalDevice> constructor();
        virtual std::unordered_map<uint32_t, SurfaceProperties> getSurfaceInfo(const vk::SurfaceKHR& surface) const;
        virtual uint32_t getMemoryType(const uint32_t& memoryTypeBitsRequirement, const vk::MemoryPropertyFlags& requiredProperties = vk::MemoryPropertyFlagBits::eDeviceLocal) const;

        // 
        virtual std::shared_ptr<Instance> getInstance() { return instance.lock(); };
        virtual inline std::vector<vk::QueueFamilyProperties2>& getQueueFamilyProperties() { return queueFamilyProperties; };
        virtual inline PhysicalDeviceProperties& getPropertiesDefined() { return properties; };
        virtual inline PhysicalDeviceFeatures& getFeaturesDefined() { return features; };
        virtual inline PhysicalDevicePropertiesChain& getPropertiesChainDefined() { return propertiesChain; };
        virtual inline PhysicalDeviceFeaturesChain& getFeaturesChainDefined() { return featuresChain; };
        virtual inline vk::PhysicalDevice& getPhysicalDevice() { return physicalDevice; };
        virtual inline vk::PhysicalDeviceMemoryProperties2& getMemoryPropertiesDefined() { return memoryProperties; };
        virtual inline std::vector<vk::ExtensionProperties>& getExtensionPropertiesDefined() { return extensionProperties; };
        virtual inline std::vector<vk::LayerProperties>& getLayerPropertiesDefined() { return layerProperties; };

        // 
        virtual std::shared_ptr<Instance> getInstance() const { return instance.lock(); };
        virtual inline const std::vector<vk::QueueFamilyProperties2>& getQueueFamilyProperties() const { return queueFamilyProperties; };
        virtual inline const PhysicalDeviceProperties& getPropertiesDefined() const { return properties; };
        virtual inline const PhysicalDeviceFeatures& getFeaturesDefined() const { return features; };
        virtual inline const PhysicalDevicePropertiesChain& getPropertiesChainDefined() const { return propertiesChain; };
        virtual inline const PhysicalDeviceFeaturesChain& getFeaturesChainDefined() const { return featuresChain; };
        virtual inline const vk::PhysicalDevice& getPhysicalDevice() const { return physicalDevice; };
        virtual inline const vk::PhysicalDeviceMemoryProperties2& getMemoryPropertiesDefined() const { return memoryProperties; };
        virtual inline const std::vector<vk::ExtensionProperties>& getExtensionPropertiesDefined() const { return extensionProperties; };
        virtual inline const std::vector<vk::LayerProperties>& getLayerPropertiesDefined() const { return layerProperties; };
    };



};
