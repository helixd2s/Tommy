#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"

namespace tom {

    // 
    class Instance: public std::enable_shared_from_this<Instance> {
        protected:
        vk::Instance instance = {};
        std::unordered_map<uintptr_t, std::weak_ptr<Device>> deviceMap = {};

        public: 
        Instance() {
            
        }
    };

    // 
    std::unordered_map<uintptr_t, std::weak_ptr<Instance>> instanceMap = {};

};
