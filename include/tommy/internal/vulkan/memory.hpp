

#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "../memory.hpp"

#include "./core.hpp"
#include "./device.hpp"

// 
namespace tom {

    // 
    namespace vulkan {

        // 
        class DeviceMemoryApi: public DeviceMemoryApiBase { public: 
            vk::DeviceMemory memory = {};
            vk::MemoryAllocateInfo info = {};

            // 
            static std::shared_ptr<DeviceMemoryApi> makeShared(const vk::DeviceMemory& memory = {}) {
                std::shared_ptr<DeviceMemoryApi> data = {};
                data->memory = memory;
                return data;
            };
        };

        // 
        class DeviceMemoryData: public DeviceMemoryBase { public: 
            
        };

        // 
        class MemoryAllocationData: public MemoryAllocationBase { public: 
            
        };

        // 
        class DeviceMemory: public tom::DeviceMemory {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma;
            virtual inline std::shared_ptr<DeviceMemoryData> getDataTyped() { return std::dynamic_pointer_cast<DeviceMemoryData>(data); };
            virtual inline std::shared_ptr<DeviceMemoryData> getDataTyped() const { return std::dynamic_pointer_cast<DeviceMemoryData>(data); };
            virtual inline std::shared_ptr<DeviceMemoryApi> getApiTyped() { return std::dynamic_pointer_cast<DeviceMemoryApi>(api); };
            virtual inline std::shared_ptr<DeviceMemoryApi> getApiTyped() const { return std::dynamic_pointer_cast<DeviceMemoryApi>(api); };

        public: // 
            // legacy
            //DeviceMemory(const std::shared_ptr<tom::Device>& device, const vk::DeviceMemory& memory = {}) : tom::DeviceMemory(device, std::make_shared<DeviceMemoryData>(), DeviceMemoryApi::makeShared(memory)) 
            //{};

            // 
            virtual std::shared_ptr<tom::DeviceMemory> constructor() override {
                if (!this->data) { this->data = std::make_shared<DeviceMemoryData>(); }; 
                return shared_from_this();
            };

            // 
            virtual std::shared_ptr<tom::DeviceMemory> allocate(const std::shared_ptr<tom::MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});
        };

        // 
        class MemoryAllocation: public tom::MemoryAllocation {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma; friend DeviceMemory; friend MemoryAllocation;

        public: // 
            MemoryAllocation(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}): tom::MemoryAllocation(deviceMemory, data) 
            {};

            // 
            MemoryAllocation(const std::shared_ptr<Device>& device = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}) : tom::MemoryAllocation(device, data)
            {};

            // 
            ~MemoryAllocation() {
                this->deviceMemory = {};
                this->data = {};
            };
        };

    };

};
