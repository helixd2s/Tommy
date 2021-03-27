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

    public: // 
        Instance() { this->constructor(); };

        // 
        virtual std::shared_ptr<Instance> constructor() {
            if (!this->data) { this->data = std::make_shared<InstanceBase>(); };
            return shared_from_this();
        };

        // 
        virtual inline std::shared_ptr<InstanceBase>& getData() { return data; };
        virtual inline std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices();

        // 
        virtual inline const std::shared_ptr<InstanceBase>& getData() const { return data; };
        virtual inline const std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices() const;
    };

    // 
    class PhysicalDevice: public std::enable_shared_from_this<PhysicalDevice> {
    protected:  // 
        std::weak_ptr<Instance> instance = {};
        std::shared_ptr<PhysicalDeviceBase> data = {};

    public: // 
        PhysicalDevice(const std::shared_ptr<Instance>& instance, const std::shared_ptr<PhysicalDeviceBase> data = {}): instance(instance), data(data) {
            this->constructor();
        };

        //
        virtual std::shared_ptr<PhysicalDevice> constructor() {
            if (!this->data) { this->data = std::make_shared<PhysicalDeviceBase>(); };
            return shared_from_this();
        };

        // 
        virtual std::shared_ptr<Instance> getInstance() { return instance.lock(); };
        virtual std::shared_ptr<PhysicalDeviceBase>& getData() { return data; };

        // 
        virtual std::shared_ptr<Instance> getInstance() const { return instance.lock(); };
        virtual const std::shared_ptr<PhysicalDeviceBase>& getData() const { return data; };
    };

};
