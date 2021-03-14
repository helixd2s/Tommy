#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"

namespace tom {

    // 
    class Queue: public std::enable_shared_from_this<Queue> {
        protected:
        std::shared_ptr<tom::Device> device = {};
        vk::Queue queue = {};
        

        public: 
        Queue() {
            
        }
    };

};
