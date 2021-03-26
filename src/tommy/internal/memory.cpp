#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>

// 
namespace tom {

    namespace vktm { 
        // TODO: VMA allocation
        std::shared_ptr<DeviceMemory> DeviceMemory::allocate(const std::shared_ptr<MemoryAllocator>& allocator, const vk::MemoryAllocateInfo& info = {}) {
                auto device = this->getDevice();
                this->info = info;
                this->memory = device->getDevice().allocateMemory(info);
                this->destructor = [self = shared_from_this(), device](){
                auto& memory = self->getMemory();
                if (memory) { device->getDevice().freeMemory(); };
                memory = vk::DeviceMemory{};
                };

        #ifdef _WIN32
                auto handle = device->getDevice().getMemoryWin32HandleKHR(vk::MemoryGetWin32HandleInfoKHR{.memory = this->memory, .handleType = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32});
        #else
                auto handle = device->getDevice().getMemoryFdKHR(vk::VkMemoryGetFdInfoKHR{.memory = this->memory, .handleType = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd});
        #endif

        #ifdef OPENGL
                glCreateMemoryObjectsEXT(1u, &this->glHandle);
        #ifdef _WIN32
                glImportMemoryWin32HandleEXT(this->glHandle, this->info.allocationSize, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, handle);
        #else
                glImportMemoryFdEXT(this->glHandle, this->info.allocationSize, HANDLE_TYPE_OPAQUE_FD_EXT, handle);
        #endif
        #endif

                return shared_from_this();
        };

    };

};
