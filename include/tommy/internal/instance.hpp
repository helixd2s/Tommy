#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"

// 
namespace tom {

    // 
    class Instance: public std::enable_shared_from_this<Instance> {
    protected:  // 
        std::shared_ptr<InstanceBase> data = {};

    public: // 
        Instance() { this->constructor(); };

        //
        virtual std::shared_ptr<Instance> constructor();

        // 
        virtual inline std::shared_ptr<InstanceBase> getData() { return data; };
        virtual inline std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices();
        //virtual inline vk::Instance& getInstance() { return instance; };
        //virtual inline vk::DispatchLoaderDynamic& getDispatch() { return dispatch; };

        // 
        virtual inline const std::shared_ptr<InstanceBase> getData() const { return data; };
        virtual inline const std::vector<std::shared_ptr<PhysicalDevice>>& enumeratePhysicalDevices() const;
        //virtual inline const vk::Instance& getInstance() const { return instance; };
        //virtual inline const vk::DispatchLoaderDynamic& getDispatch() const { return dispatch; };
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
        virtual std::shared_ptr<PhysicalDevice> constructor();
        //virtual std::unordered_map<uint32_t, SurfaceProperties> getSurfaceInfo(const vk::SurfaceKHR& surface) const;

        

        // 
        virtual std::shared_ptr<Instance> getInstance() { return instance.lock(); };
        virtual std::shared_ptr<PhysicalDeviceBase> getData() { return data; };

        // 
        virtual std::shared_ptr<Instance> getInstance() const { return instance.lock(); };
        virtual std::shared_ptr<PhysicalDeviceBase> getData() const { return data; };
    };

};
