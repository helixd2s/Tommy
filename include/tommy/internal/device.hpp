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
    class Queue: public std::enable_shared_from_this<Queue> {
    protected:  //
        //std::shared_ptr<tom::Device> device = {};
        std::weak_ptr<tom::Device> device = {};

        // 
        vk::Queue queue = {};
        vk::CommandPool commandPool = {};
        uint32_t queueFamilyIndex = 0u;

    public: // 
        Queue(const std::shared_ptr<tom::Device>& device, const vk::Queue& queue = {}, const uint32_t& queueFamilyIndex = 0u) : device(device), queue(queue), queueFamilyIndex(queueFamilyIndex) {
            
        };

        // 
        const uint32_t& getQueueFamilyIndex() const { return queueFamilyIndex; };
        const vk::Queue& getQueue() const { return queue; };
        const vk::CommandPool& getCommandPool() const { return commandPool; };

        // 
        uint32_t& getQueueFamilyIndex() { return queueFamilyIndex; };
        vk::Queue& getQueue() { return queue; };
        vk::CommandPool& getCommandPool() { return commandPool; };
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

        // 
        std::unordered_map<uint32_t, std::vector<std::shared_ptr<Queue>>> queues = {};
        std::vector<uint32_t> queueFamilyIndices = {};


    public: // 
        Device(const std::shared_ptr<tom::Instance>& instance, const std::shared_ptr<PhysicalDevice>& physical): instance(instance), physical(physical) { // 
            this->descriptions = std::make_shared<tom::DescriptorSetSource>();

            // 
            std::vector<vk::DeviceQueueCreateInfo> queue_create_info = {};
            std::vector<const char*> desired_device_level_extensions = { "VK_KHR_swapchain", "VK_KHR_buffer_device_address" };

            // 
            auto& queueFamilyProperties = physical->getQueueFamilyProperties();
            uint32_t I=0u; for (auto& property : queueFamilyProperties) { const uint32_t i = I++;
                std::vector<float> queue_priorities = { 1.f }; // queues per every family
                if (property.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eCompute) {
                    queues[i] = std::vector<std::shared_ptr<Queue>>{};
                    queue_create_info.push_back(vk::DeviceQueueCreateInfo{
                        .queueFamilyIndex = i,
                        .queueCount = queue_priorities.size(),
                        .pQueuePriorities = queue_priorities.data()
                    });
                    queueFamilyIndices.push_back(i);
                };
            };

            // 
            this->device = this->physical->getPhysicalDevice().createDevice(vk::DeviceCreateInfo{
                .queueCreateInfoCount = queue_create_info.size(),
                .pQueueCreateInfos = queue_create_info.data(),
                .enabledExtensionCount = desired_device_level_extensions.size(),
                .ppEnabledExtensionNames = desired_device_level_extensions.data()
            });

            // 
            for (uint32_t i=0u;i<queue_create_info.size();i++) {
                const auto& queueFamilyIndex = queue_create_info[i].queueFamilyIndex;
                for (uint32_t j=0;j<queue_create_info[i].queueCount;j++) {
                    queues[queue_create_info[i].queueFamilyIndex].push_back(std::make_shared<Queue>(shared_from_this(), this->device.getQueue(queueFamilyIndex, j), queueFamilyIndex));
                };
            };
        };

        // 
        virtual std::vector<uint32_t>& getQueueFamilyIndices() { return queueFamilyIndices; };
        virtual std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) { return queues.at(queueFamilyIndex)[index]; };
        virtual std::shared_ptr<Instance>& getInstance() { return instance; };
        virtual std::shared_ptr<DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer);
        virtual std::shared_ptr<DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory);

        // 
        virtual const std::vector<uint32_t>& getQueueFamilyIndices() const { return queueFamilyIndices; };
        virtual const std::shared_ptr<Queue>& getQueueDefined(const uint32_t& queueFamilyIndex = 0u, const uint32_t& index = 0) const { return queues.at(queueFamilyIndex)[index]; };
        virtual const std::shared_ptr<Instance>& getInstance() const { return instance; };
        virtual std::shared_ptr<DeviceBuffer> getDeviceBufferObject(const vk::Buffer& buffer) const;
        virtual std::shared_ptr<DeviceMemory> getDeviceMemoryObject(const vk::DeviceMemory& deviceMemory) const;

    };

};
