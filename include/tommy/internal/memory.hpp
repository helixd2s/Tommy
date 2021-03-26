

#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"

// 
namespace tom {

    namespace vktm {
        //
        class DeviceMemory: public std::enable_shared_from_this<DeviceMemory> {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma;
            std::weak_ptr<Device> device = {};
            std::function<void()> destructor = {};

            // 
            vk::DeviceMemory memory = {};
            void* allocation = nullptr;
            void* mapped = nullptr;

            //
            uint32_t glHandle = 0u;

            //
            vk::MemoryAllocateInfo info = {};

        public: // 
            DeviceMemory(const std::shared_ptr<Device>& device, const vk::DeviceMemory& memory = {}): device(device), memory(memory) {
                
            };

            // 
            ~DeviceMemory() {
                if (this->destructor) { this->destructor(); };
                if (this->memory) { this->memory = vk::DeviceMemory{}; };
            };

            // 
            std::shared_ptr<DeviceMemory> allocate(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});

            // 
            virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
            virtual inline vk::DeviceMemory& getMemory() { return memory; };
            virtual inline void*& getMapped() { return mapped; };
            virtual inline void*& getAllocation() { return allocation; };

            // 
            virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
            virtual inline const vk::DeviceMemory& getMemory() const { return memory; };
            virtual inline void* const& getMapped() const { return mapped; };
            virtual inline void* const& getAllocation() const { return allocation; };
        };

        // 
        class MemoryAllocation: public std::enable_shared_from_this<MemoryAllocation> {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma;
            std::shared_ptr<DeviceMemory> deviceMemory = {};
            std::function<void()> destructor = {};

            // 
            vk::DeviceSize memoryOffset = 0ull;
            void* allocation = nullptr;
            void* mapped = nullptr;

        public: 
            MemoryAllocation(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const vk::DeviceSize& memoryOffset = 0ull): deviceMemory(deviceMemory), memoryOffset(memoryOffset) {
                
            };

            // 
            ~MemoryAllocation() {
                if (this->destructor) { this->destructor(); };
                if (this->deviceMemory) { this->deviceMemory = {}; };
            };

            // 
            virtual inline std::shared_ptr<DeviceMemory>& getDeviceMemory() { return deviceMemory; };
            virtual inline vk::DeviceSize& getOffset() { return memoryOffset; };
            virtual inline void* getMapped() { return mapped ? mapped : ((uint8_t*)deviceMemory->getMapped() + memoryOffset); };
            virtual inline void*& getAllocation() { return allocation; };
            virtual inline void*& getMappedDefined() { return mapped; };

            // 
            virtual inline const std::shared_ptr<DeviceMemory>& getDeviceMemory() const { return deviceMemory; };
            virtual inline const vk::DeviceSize& getOffset() const { return memoryOffset; };
            virtual inline const void* getMapped() const { return mapped ? mapped : ((const uint8_t*)deviceMemory->getMapped() + memoryOffset); };
            virtual inline void* const& getAllocation() const { return allocation; };
            virtual inline void* const& getMappedDefined() const { return mapped; };
        };

    };

};
