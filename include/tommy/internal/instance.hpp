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
    struct SurfaceProperties {
        vk::Bool32 supported = false;
        vk::SurfaceCapabilities2KHR capabilities = {};
        std::vector<vk::PresentModeKHR> presentModes = {};
        std::vector<vk::SurfaceFormat2KHR> formats = {};
    };

    //
    class InstanceStatic: public std::enable_shared_from_this<InstanceStatic> { public: 
        bool initialized = false;
        vk::DispatchLoaderDynamic dispatch = {};
        std::unordered_map<uintptr_t, std::shared_ptr<Instance>> instanceMap;
        std::vector<vk::ExtensionProperties> extensionProperties = {};
        std::vector<vk::LayerProperties> layerProperties = {};
        virtual void initialize();
        //std::function<void()> initialize = {};

        InstanceStatic() {
            this->initialize();
        };
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
        static std::shared_ptr<InstanceStatic> context;

    public: // 
        Instance() { this->constructor(); };

        //
        virtual void constructor() {
            vk::ApplicationInfo application_info = vk::ApplicationInfo{
                .pApplicationName = "Tommy Based App",
                .applicationVersion = VK_MAKE_VERSION(1,0,0),
                .pEngineName = "Tommy Library",
                .engineVersion = VK_MAKE_VERSION(1,0,0),
                .apiVersion = VK_MAKE_VERSION(1,2,0)
            };

            // 
            std::vector<const char*> preferedLayers = {};
            std::vector<const char*> preferedExtensions = { "VK_KHR_surface","VK_KHR_win32_surface" };

#ifdef NDEBUG
            preferedLayers.push_back("VK_LAYER_LUNARG_standard_validation");
            preferedExtensions.push_back("VK_EXT_debug_report");
#endif

            //
            if (!context) { context = std::make_shared<InstanceStatic>(); };

            // 
            for (uintptr_t i=0;i<preferedExtensions.size();i++) {
                const auto& name = preferedExtensions[i];
                bool found = false;
                for (auto& prop : context->extensionProperties) {
                    if (found = found || (strcmp(name, prop.extensionName) == 0)) break;
                };
                if (!found) { preferedExtensions.erase(preferedExtensions.begin()+i); };
            };

            // 
            for (uintptr_t i=0;i<preferedLayers.size();i++) {
                const auto& name = preferedLayers[i];
                bool found = false;
                for (auto& prop : context->layerProperties) {
                    if (found = found || (strcmp(name, prop.layerName) == 0)) break;
                };
                if (!found) { preferedLayers.erase(preferedLayers.begin()+i); };
            };

            // 
            this->dispatch = vk::DispatchLoaderDynamic(this->instance = vk::createInstance(vk::InstanceCreateInfo{
                .pApplicationInfo = &application_info,
                .enabledLayerCount = preferedLayers.size(),
                .ppEnabledLayerNames = preferedLayers.data(),
                .enabledExtensionCount = preferedExtensions.size(),
                .ppEnabledExtensionNames = preferedExtensions.data()
            }), vkGetInstanceProcAddr);
        };

        // 
        virtual std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices();
        virtual vk::Instance& getInstance() { return instance; };

        // 
        virtual const std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices() const;
        virtual const vk::Instance& getInstance() const { return instance; };
    };

    //
    void InstanceStatic::initialize() {
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
        virtual void constructor() {
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
        };

        //
        virtual std::unordered_map<uint32_t, SurfaceProperties> getSurfaceInfo(const vk::SurfaceKHR& surface) const {
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
