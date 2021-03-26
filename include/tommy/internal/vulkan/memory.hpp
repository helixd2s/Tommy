

#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "./device.hpp"

#include "../memory.hpp"

// 
namespace tom {

    // 
    namespace vulkan {

        // 
        class DeviceMemoryData: public DeviceMemoryBase { public: 
            vk::DeviceMemory memory = {};
            vk::MemoryAllocateInfo info = {};
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
            DeviceMemory(const std::shared_ptr<tom::Device>& device, const vk::DeviceMemory& memory = {}) : tom::DeviceMemory(device, std::make_shared<DeviceMemoryData>()) {
                auto data = this->getDataTyped();
                data->memory = memory;
            };

            // 
            ~DeviceMemory() {
                auto data = this->getDataTyped();
                if (this->destructor) { this->destructor(); };
                if (data->memory) { data->memory = vk::DeviceMemory{}; };
            };

            // 
            std::shared_ptr<tom::DeviceMemory> allocate(const std::shared_ptr<tom::MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});

            // 
            virtual inline void*& getAllocation() { return allocation; };
            virtual inline void*& getMapped() { return mapped; };

            // 
            virtual inline void* const& getAllocation() const { return allocation; };
            virtual inline void* const& getMapped() const { return mapped; };
        };

        // 
        class MemoryAllocation: public tom::MemoryAllocation {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma; friend DeviceMemory; friend MemoryAllocation;

        public: // 
            MemoryAllocation(const std::shared_ptr<tom::DeviceMemory>& deviceMemory = {}, const vk::DeviceSize& memoryOffset = 0ull): tom::MemoryAllocation(deviceMemory, memoryOffset, std::make_shared<MemoryAllocationData>()) {
            };

            // 
            MemoryAllocation(const std::shared_ptr<tom::Device>& device = {}) : tom::MemoryAllocation(device) {

            };

            // 
            ~MemoryAllocation() {
                if (this->destructor) { this->destructor(); };
                if (this->deviceMemory) { this->deviceMemory = {}; };
            };
        };

    };

};
