#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./instance.hpp"

namespace tom {

    // 
    class DescriptorSetSource: public std::enable_shared_from_this<DescriptorSetSource> {
        std::vector<vk::DescriptorBufferInfo> buffers = {};
        std::vector<vk::DescriptorImageInfo> textures2d = {};
    };

    // 
    class Device: public std::enable_shared_from_this<Device> {
        protected:
        std::shared_ptr<tom::Instance> instance = {};
        vk::PhysicalDevice physical = {};
        vk::Device device = {};
        std::unordered_map<uintptr_t, std::weak_ptr<Buffer>> bufferMap = {};

        // 
        std::shared_ptr<tom::DescriptorSetSource> descriptions = {};
        

        public: 
        Device() {
            
        }
    };

};
