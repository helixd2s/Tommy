

#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"

// 
namespace tom {

    //
    class DeviceMemory: public std::enable_shared_from_this<DeviceMemory> {
    protected:
        std::shared_ptr<tom::Device> device = {};

        // 
        vk::DeviceMemory memory = {};
        void* allocation = nullptr;
        void* mapped = nullptr;

    public: // 
        DeviceMemory(const std::shared_ptr<tom::Device>& device, const vk::DeviceMemory& memory = {}): device(device), memory(memory) {
            
        };

        // 
        virtual inline std::shared_ptr<tom::Device>& getDevice() { return device; };
        virtual inline vk::DeviceMemory& getMemory() { return memory; };
        virtual inline void*& getMapped() { return mapped; };
        virtual inline void*& getAllocation() { return allocation; };

        //
        virtual inline const std::shared_ptr<tom::Device>& getDevice() const { return device; };
        virtual inline const vk::DeviceMemory& getMemory() const { return memory; };
        virtual inline void* const& getMapped() const { return mapped; };
        virtual inline void* const& getAllocation() const { return allocation; };
    };

    // 
    class MemoryAllocation: public std::enable_shared_from_this<MemoryAllocation> {
    protected:
        std::shared_ptr<tom::DeviceMemory> deviceMemory = {};

        // 
        vk::DeviceSize memoryOffset = 0ull;
        void* allocation = nullptr;
        void* mapped = nullptr;

    public: 
        MemoryAllocation(const std::shared_ptr<tom::DeviceMemory>& deviceMemory = {}, const vk::DeviceSize& memoryOffset = 0ull): deviceMemory(deviceMemory), memoryOffset(memoryOffset) {
            
        };

        // 
        virtual inline std::shared_ptr<tom::DeviceMemory>& getDeviceMemory() { return deviceMemory; };
        virtual inline vk::DeviceSize& getOffset() { return memoryOffset; };
        virtual inline void* getMapped() { return ((uint8_t*)deviceMemory->getMapped() + memoryOffset); };
        virtual inline void*& getAllocation() { return allocation; };

        // 
        virtual inline const std::shared_ptr<tom::DeviceMemory>& getDeviceMemory() const { return deviceMemory; };
        virtual inline const vk::DeviceSize& getOffset() const { return memoryOffset; };
        virtual inline const void* getMapped() const { return ((const uint8_t*)deviceMemory->getMapped() + memoryOffset); };
        virtual inline void* const& getAllocation() const { return allocation; };
    };

};
