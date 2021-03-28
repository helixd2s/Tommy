#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "../instance.hpp"

#include "./core.hpp"

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

            // 
            static std::shared_ptr<PhysicalDeviceData> makeShared(const vk::PhysicalDevice& physicalDevice = {}) {
                std::shared_ptr<PhysicalDeviceData> data = {};
                data->physicalDevice = physicalDevice;
                return data;
            };
        };

        // 
        class InstanceData: public InstanceBase { public: 
            vk::Instance instance = {};
            vk::DispatchLoaderDynamic dispatch = {};

            // 
            std::unordered_map<uintptr_t, std::weak_ptr<Device>> deviceMap = {};
            
        };

        // 
        class Context: public std::enable_shared_from_this<Context> { public: 
            bool initialized = false;
            vk::DispatchLoaderDynamic dispatch = {};
            std::unordered_map<uintptr_t, std::shared_ptr<Instance>> instanceMap = {};
            std::vector<vk::ExtensionProperties> extensionProperties = {};
            std::vector<vk::LayerProperties> layerProperties = {};

        public: 
            // 
            Context() { this->initialize(); };

            virtual void initialize();
        };




        // 
        class Instance: public tom::Instance {
        protected:  //
            static std::shared_ptr<Context> context;

            virtual inline std::shared_ptr<InstanceData> getDataTyped() { return std::dynamic_pointer_cast<InstanceData>(this->data); };
            virtual inline std::shared_ptr<InstanceData> getDataTyped() const { return std::dynamic_pointer_cast<InstanceData>(this->data); };

        public: 
            // 
            Instance() : tom::Instance() {};

            // 
            virtual std::shared_ptr<tom::Instance> constructor() override;

            // 
            virtual inline std::vector<std::shared_ptr<tom::PhysicalDevice>>& enumeratePhysicalDevices() override;
            virtual inline const std::vector<std::shared_ptr<tom::PhysicalDevice>>& enumeratePhysicalDevices() const override;
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
        class PhysicalDevice: public tom::PhysicalDevice {
        protected:  // 
            virtual inline std::shared_ptr<PhysicalDeviceData> getDataTyped() { return std::dynamic_pointer_cast<PhysicalDeviceData>(this->data); };
            virtual inline std::shared_ptr<PhysicalDeviceData> getDataTyped() const { return std::dynamic_pointer_cast<PhysicalDeviceData>(this->data); };

        public: 
            // legacy
            //PhysicalDevice(const std::shared_ptr<tom::Instance>& instance, const vk::PhysicalDevice& physicalDevice): tom::PhysicalDevice(instance, PhysicalDeviceData::makeShared(physicalDevice)) 
            //{};

            //
            virtual std::shared_ptr<tom::PhysicalDevice> constructor() override;
            virtual std::unordered_map<uint32_t, SurfaceProperties> getSurfaceInfo(const vk::SurfaceKHR& surface) const;
            virtual uint32_t getMemoryType(const uint32_t& memoryTypeBitsRequirement, const vk::MemoryPropertyFlags& requiredProperties = vk::MemoryPropertyFlagBits::eDeviceLocal) const;
        };

    };

};
