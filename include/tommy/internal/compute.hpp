#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"

// 
namespace tom {

    // 
    namespace vulkan {
        // 
        class ComputePipeline: public std::enable_shared_from_this<ComputePipeline> { protected:
            std::weak_ptr<tom::Device> device = {};
            std::shared_ptr<ComputePipelineBase> data = {};

        public: 
            ComputePipeline() {
                
            };
        };
    };
};
