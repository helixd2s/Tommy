#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"

// 
namespace tom {

    // 
    class Instance: public std::enable_shared_from_this<Instance> {
    protected:  // 
        std::shared_ptr<InstanceBase> data = {};
        std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices = {};
        std::vector<std::shared_ptr<Device>> devices = {};
        
    public: // 
        Instance(const std::shared_ptr<InstanceBase>& data = {}) : data(data) { this->constructor(); };

        // 
        virtual std::shared_ptr<Instance> constructor() {
            if (!this->data) { this->data = std::make_shared<InstanceBase>(); };
            return shared_from_this();
        };

        // 
        virtual inline uintptr_t getKey() { return 0ull; };
        virtual inline std::shared_ptr<InstanceBase>& getData() { return data; };
        virtual inline std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices();

        // 
        virtual inline uintptr_t getKey() const { return 0ull; };
        virtual inline const std::shared_ptr<InstanceBase>& getData() const { return data; };
        virtual inline const std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices() const;
    };

    // 
    class PhysicalDevice: public std::enable_shared_from_this<PhysicalDevice> {
    protected:  // 
        std::weak_ptr<Instance> instance = {};
        std::shared_ptr<PhysicalDeviceBase> data = {};

    public: // 
        PhysicalDevice(const std::shared_ptr<Instance>& instance, const std::shared_ptr<PhysicalDeviceBase> data = {}): instance(instance), data(data) 
        { this->constructor(); };

        //
        virtual std::shared_ptr<PhysicalDevice> constructor() {
            if (!this->data) { this->data = std::make_shared<PhysicalDeviceBase>(); };
            return shared_from_this();
        };

        // 
        virtual inline uintptr_t getKey() { return 0ull; };
        virtual inline std::shared_ptr<Instance> getInstance() { return instance.lock(); };
        virtual inline std::shared_ptr<PhysicalDeviceBase>& getData() { return data; };

        // 
        virtual inline uintptr_t getKey() const { return 0ull; };
        virtual inline std::shared_ptr<Instance> getInstance() const { return instance.lock(); };
        virtual inline const std::shared_ptr<PhysicalDeviceBase>& getData() const { return data; };
    };

};
