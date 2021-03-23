#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>

// 
namespace tom {

    // TODO: VMA allocation
    std::shared_ptr<DeviceMemory> DeviceMemory::allocate(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {}) {
        this->memory = this->device->getDevice().allocateMemory(info);
        this->destructor = [this](){
            auto& memory = this->getMemory();
            if (memory) { this->device->getDevice().freeMemory(); };
            memory = vk::DeviceMemory{};
        };
        return shared_from_this();
    };

};
