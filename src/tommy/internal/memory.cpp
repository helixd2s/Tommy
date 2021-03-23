#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>

// 
namespace tom {

    // TODO: VMA allocation
    std::shared_ptr<DeviceMemory> DeviceMemory::allocate(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {}) {
        auto device = this->getDevice();
        this->memory = device->getDevice().allocateMemory(info);
        this->destructor = [this, device](){
            auto& memory = this->getMemory();
            if (memory) { device->getDevice().freeMemory(); };
            memory = vk::DeviceMemory{};
        };
        return shared_from_this();
    };

};
