#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>

// 
namespace tom {

    namespace vulkan { 
        // TODO: VMA allocation
        std::shared_ptr<tom::DeviceMemory> DeviceMemory::allocate(const std::shared_ptr<tom::MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {}) {
            auto device = std::dynamic_pointer_cast<DeviceData>(this->getDevice()->getData())->device;
            auto data = this->getDataTyped();
            data->info = info;
            data->memory = device.allocateMemory(info);
            this->destructor = [data, device](){
                auto& memory = data->memory;
                if (memory) { device.freeMemory(); };
                memory = vk::DeviceMemory{};
            };

    #ifdef _WIN32
            auto handle = device.getMemoryWin32HandleKHR(vk::MemoryGetWin32HandleInfoKHR{.memory = data->memory, .handleType = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32});
    #else
            auto handle = device.getMemoryFdKHR(vk::VkMemoryGetFdInfoKHR{.memory = data->memory, .handleType = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd});
    #endif

/*
    #ifdef OPENGL
            glCreateMemoryObjectsEXT(1u, &this->glHandle);
    #ifdef _WIN32
            glImportMemoryWin32HandleEXT(this->glHandle, data->info.allocationSize, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, handle);
    #else
            glImportMemoryFdEXT(this->glHandle, data->info.allocationSize, HANDLE_TYPE_OPAQUE_FD_EXT, handle);
    #endif
    #endif
*/

            return shared_from_this();
        };

    };

};
