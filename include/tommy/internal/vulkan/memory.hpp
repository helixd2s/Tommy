

#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "./device.hpp"

// 
namespace tom {

    namespace vulkan {

        //
        class DeviceMemoryData: public DeviceMemoryBase { public: 
            vk::DeviceMemory memory = {};
            vk::MemoryAllocateInfo info = {};
        };

        //
        class MemoryAllocationData: public MemoryAllocationBase { public: 
            vk::DeviceSize memoryOffset = 0ull;
        };

        //
        class DeviceMemory: public std::enable_shared_from_this<DeviceMemory> {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma;
            std::weak_ptr<Device> device = {};
            std::shared_ptr<DeviceMemoryData> data = {};
            std::function<void()> destructor = {};

            // 
            void* allocation = nullptr;
            void* mapped = nullptr;

            //
            uint32_t glHandle = 0u;

        public: // 
            // legacy
            DeviceMemory(const std::shared_ptr<Device>& device, const vk::DeviceMemory& memory = {}): device(device) {
                data = std::make_shared<DeviceMemoryData>();
                data->memory = memory;
            };

            // 
            ~DeviceMemory() {
                if (this->destructor) { this->destructor(); };
                if (data->memory) { data->memory = vk::DeviceMemory{}; };
            };

            // 
            std::shared_ptr<DeviceMemory> allocate(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {});

            // 
            virtual inline std::shared_ptr<DeviceMemoryData> getData() { return data; };
            virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
            virtual inline void*& getAllocation() { return allocation; };
            virtual inline void*& getMapped() { return mapped; };

            // 
            virtual inline std::shared_ptr<DeviceMemoryData> getData() const { return data; };
            virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
            virtual inline void* const& getAllocation() const { return allocation; };
            virtual inline void* const& getMapped() const { return mapped; };
        };

        // 
        class MemoryAllocation: public std::enable_shared_from_this<MemoryAllocation> {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma;
            std::shared_ptr<DeviceMemory> deviceMemory = {};
            std::shared_ptr<MemoryAllocationData> data = {};
            std::function<void()> destructor = {};

            // 
            void* allocation = nullptr;
            void* mapped = nullptr;

        public: 
            MemoryAllocation(const std::shared_ptr<DeviceMemory>& deviceMemory = {}, const vk::DeviceSize& memoryOffset = 0ull): deviceMemory(deviceMemory) {
                data = std::make_shared<MemoryAllocationData>();
                data->memoryOffset = memoryOffset;
            };

            // 
            ~MemoryAllocation() {
                if (this->destructor) { this->destructor(); };
                if (this->deviceMemory) { this->deviceMemory = {}; };
            };

            // 
            virtual inline std::shared_ptr<MemoryAllocationData> getData() { return data; };
            virtual inline std::shared_ptr<DeviceMemory>& getDeviceMemory() { return deviceMemory; };
            virtual inline void* getMapped() { return mapped ? mapped : ((uint8_t*)deviceMemory->getMapped() + data->memoryOffset); };
            virtual inline void*& getAllocation() { return allocation; };
            virtual inline void*& getMappedDefined() { return mapped; };

            // 
            virtual inline std::shared_ptr<MemoryAllocationData> getData() const { return data; };
            virtual inline const std::shared_ptr<DeviceMemory>& getDeviceMemory() const { return deviceMemory; };
            virtual inline const void* getMapped() const { return mapped ? mapped : ((const uint8_t*)deviceMemory->getMapped() + data->memoryOffset); };
            virtual inline void* const& getAllocation() const { return allocation; };
            virtual inline void* const& getMappedDefined() const { return mapped; };
        };

    };

};
