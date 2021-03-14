#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"

namespace tom {

    // 
    class Buffer: public std::enable_shared_from_this<Buffer> {
        protected:
        std::shared_ptr<tom::Device> device = {};
        vk::Buffer buffer = {};
        vk::DeviceMemory memory = {};

        public: 
        Buffer() {
            
        }
    };

};
