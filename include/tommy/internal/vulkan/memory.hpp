

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
        class DeviceMemoryData: public DeviceMemoryBase { public: 
            vk::DeviceMemory memory = {};
            vk::MemoryAllocateInfo info = {};

            // 
            static std::shared_ptr<DeviceMemoryData> makeShared(const vk::DeviceMemory& memory = {}) {
                std::shared_ptr<DeviceMemoryData> data = {};
                data->memory = memory;
                return data;
            };
        };

        // 
        class MemoryAllocationData: public MemoryAllocationBase { public: 
            
        };

        // 
        class DeviceMemory: public tom::DeviceMemory {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma;
            virtual inline std::shared_ptr<DeviceMemoryData> getDataTyped() { return std::dynamic_pointer_cast<DeviceMemoryData>(data); };
            virtual inline std::shared_ptr<DeviceMemoryData> getDataTyped() const { return std::dynamic_pointer_cast<DeviceMemoryData>(data); };

        public: // 
            // legacy
            DeviceMemory(const std::shared_ptr<tom::Device>& device, const vk::DeviceMemory& memory = {}) : tom::DeviceMemory(device, DeviceMemoryData::makeShared(memory)) {
            };

            // 
            ~DeviceMemory() {
                auto data = this->getDataTyped();
                if (data->memory) { data->memory = vk::DeviceMemory{}; };
            };

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
            MemoryAllocation(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}): tom::MemoryAllocation(deviceMemory, data) {
            };

            MemoryAllocation(const std::shared_ptr<Device>& device = {}, const std::shared_ptr<MemoryAllocationBase>& data = {}) : tom::MemoryAllocation(device, data)  {
            };

            // 
            ~MemoryAllocation() {
                this->deviceMemory = {};
                this->data = {};
            };
        };

    };

};
