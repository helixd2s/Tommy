#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>

// 
namespace tom {

    namespace vulkan { 
        // TODO: VMA allocation
        std::shared_ptr<DeviceMemory> DeviceMemory::allocate(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {}) {
            auto device = this->getDevice()->getData()->device;
            data->info = info;
            data->memory = device.allocateMemory(info);
            this->destructor = [self = shared_from_this(), device](){
                auto& memory = self->getData()->memory;
                if (memory) { device.freeMemory(); };
                memory = vk::DeviceMemory{};
            };

    #ifdef _WIN32
            auto handle = device.getMemoryWin32HandleKHR(vk::MemoryGetWin32HandleInfoKHR{.memory = data->memory, .handleType = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32});
    #else
            auto handle = device.getMemoryFdKHR(vk::VkMemoryGetFdInfoKHR{.memory = data->memory, .handleType = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd});
    #endif

    #ifdef OPENGL
            glCreateMemoryObjectsEXT(1u, &this->glHandle);
    #ifdef _WIN32
            glImportMemoryWin32HandleEXT(this->glHandle, data->info.allocationSize, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, handle);
    #else
            glImportMemoryFdEXT(this->glHandle, data->info.allocationSize, HANDLE_TYPE_OPAQUE_FD_EXT, handle);
    #endif
    #endif

            return shared_from_this();
        };

    };

};
