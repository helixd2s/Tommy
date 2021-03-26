#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"

// 
namespace tom {

    // 
    namespace vulkan {

        // 
        class PhysicalDeviceData: public PhysicalDeviceBase { public: 
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
        };

        //
        class InstanceData: public InstanceBase { public: 
            vk::Instance instance = {};
            vk::DispatchLoaderDynamic dispatch = {};

            // 
            std::unordered_map<uintptr_t, std::weak_ptr<Device>> deviceMap = {};
            std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices = {};
        };

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
            std::shared_ptr<InstanceData> data = {};

            //
            static std::shared_ptr<Context> context;

        public: // 
            Instance() { this->constructor(); };

            //
            virtual std::shared_ptr<Instance> constructor();

            // 
            virtual inline std::shared_ptr<InstanceData> getData() { return data; };
            virtual inline std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices();
            //virtual inline vk::Instance& getInstance() { return instance; };
            //virtual inline vk::DispatchLoaderDynamic& getDispatch() { return dispatch; };

            // 
            virtual inline const std::shared_ptr<InstanceData> getData() const { return data; };
            virtual inline const std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices() const;
            //virtual inline const vk::Instance& getInstance() const { return instance; };
            //virtual inline const vk::DispatchLoaderDynamic& getDispatch() const { return dispatch; };
        };

        //
        void Context::initialize() {
            auto& data = *this;
            if (!data.initialized) {
                data.dispatch = vk::DispatchLoaderDynamic(vkGetInstanceProcAddr);
                data.initialized = true;
                data.instanceMap = {};
                data.extensionProperties = vk::enumerateInstanceExtensionProperties();
                data.layerProperties = vk::enumerateInstanceLayerProperties();
            };
        };

        // 
        class PhysicalDevice: public std::enable_shared_from_this<PhysicalDevice> {
        protected:  // 
            std::weak_ptr<Instance> instance = {};
            std::shared_ptr<PhysicalDeviceData> data = {};

        public: // 
            PhysicalDevice(const std::shared_ptr<Instance>& instance, const vk::PhysicalDevice& physicalDevice): instance(instance) {
                data = std::make_shared<PhysicalDeviceData>();
                data->physicalDevice = physicalDevice;
                this->constructor();
            };

            //
            virtual std::shared_ptr<PhysicalDevice> constructor();
            virtual std::unordered_map<uint32_t, SurfaceProperties> getSurfaceInfo(const vk::SurfaceKHR& surface) const;
            virtual uint32_t getMemoryType(const uint32_t& memoryTypeBitsRequirement, const vk::MemoryPropertyFlags& requiredProperties = vk::MemoryPropertyFlagBits::eDeviceLocal) const;

            // 
            virtual std::shared_ptr<Instance> getInstance() { return instance.lock(); };
            virtual std::shared_ptr<PhysicalDeviceData> getData() { return data; };

            // 
            virtual std::shared_ptr<Instance> getInstance() const { return instance.lock(); };
            virtual std::shared_ptr<PhysicalDeviceData> getData() const { return data; };
        };

    };

};
