#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "./device.hpp"

// 
namespace tom {
    namespace vulkan {
        // 
        class Compute: public std::enable_shared_from_this<Compute> { protected:
            std::weak_ptr<tom::Device> device = {};

            public: 
            Compute() {
                
            };
        };
    };
};
