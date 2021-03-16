#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./instance.hpp"

// 
namespace tom {

    // 
    class DescriptorSetSource: public std::enable_shared_from_this<DescriptorSetSource> {
        std::vector<vk::DescriptorBufferInfo> buffers = {};
        std::vector<vk::DescriptorImageInfo> textures2d = {};
    };

    // 
    class Device: public std::enable_shared_from_this<Device> {
    protected:  // 
        std::shared_ptr<Instance> instance = {};
        std::shared_ptr<PhysicalDevice> physical = {};

        // 
        vk::Device device = {};

        // 
        std::unordered_map<uintptr_t, std::weak_ptr<Buffer>> bufferAllocations = {};
        std::unordered_map<vk::Buffer, std::weak_ptr<DeviceBuffer>> buffers = {};
        std::unordered_map<vk::DeviceMemory, std::weak_ptr<DeviceMemory>> memories = {};

        // 
        std::shared_ptr<tom::DescriptorSetSource> descriptions = {};

    public: // 
        Device(const std::shared_ptr<tom::Instance>& instance, const std::shared_ptr<PhysicalDevice>& physical): instance(instance), physical(physical) { // 
            this->descriptions = std::make_shared<tom::DescriptorSetSource>();

            
        };

        // 
        std::shared_ptr<DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer);
        std::shared_ptr<DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory);

        // 
        std::shared_ptr<DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer) const;
        std::shared_ptr<DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const;

    };

};
