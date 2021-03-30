#pragma once

//#include <tommy/core.hpp>
#include "../core.hpp"
#include "../buffer.hpp"

#include "./core.hpp"
#include "./device.hpp"
#include "./memory.hpp"



// 
namespace tom {

    // 
    namespace vulkan {

        // 
        class DeviceBufferData : public DeviceBufferBase { public: 
            vk::Buffer buffer = {};
            vk::DeviceAddress address = 0ull;
            vk::BufferCreateInfo info = {};

            // 
            static std::shared_ptr<DeviceBufferData> makeShared(const vk::Buffer& buffer = {}) {
                std::shared_ptr<DeviceBufferData> data = {};
                data->buffer = buffer;
                return data;
            };
        };

        // 
        class BufferAllocationData : public BufferAllocationBase { public: 
            vk::DescriptorBufferInfo bufferInfo = { vk::Buffer{}, 0ull, VK_WHOLE_SIZE };
            vk::DeviceAddress address = 0ull;
        };



        // BASED ON ALLOCATION!!!
        class DeviceBuffer: public tom::DeviceBuffer {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma; friend BufferAllocation; // 
            virtual inline std::shared_ptr<DeviceBufferData> getApiTyped() { return std::dynamic_pointer_cast<DeviceBufferData>(this->api); };
            virtual inline std::shared_ptr<DeviceBufferData> getApiTyped() const { return std::dynamic_pointer_cast<DeviceBufferData>(this->api); };

        public: 
            // 
            virtual uintptr_t& getDeviceAddress();
            virtual uintptr_t getDeviceAddress() const;

            // 
            virtual std::shared_ptr<tom::MemoryAllocation> bindMemory(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) override;
            virtual std::shared_ptr<tom::MemoryAllocation> create(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) override;
        };

        // abscent class...
        class BufferAllocation: public tom::BufferAllocation {
        protected:  // 
            friend DeviceBuffer;
            virtual inline std::shared_ptr<BufferAllocationData> getDataTyped() { return std::dynamic_pointer_cast<BufferAllocationData>(this->data); };
            virtual inline std::shared_ptr<BufferAllocationData> getDataTyped() const { return std::dynamic_pointer_cast<BufferAllocationData>(this->data); };

        public: // 

            // 
            virtual std::shared_ptr<tom::BufferAllocation> constructor(const uintptr_t& offset = 0ull, const uintptr_t& range = VK_WHOLE_SIZE) override {
                if (!this->data) { this->data = std::make_shared<BufferAllocationData>(); };
                auto data = this->getDataTyped();
                if (this->deviceBuffer) { data->bufferInfo.buffer = std::dynamic_pointer_cast<DeviceBufferData>(this->deviceBuffer->getApi())->buffer; };
                data->bufferInfo.offset = offset;
                data->bufferInfo.range = range;
                return shared_from_this();
            };

            // 
            virtual inline uintptr_t getDeviceAddress() override { auto data = this->getDataTyped(); data->address = ((data->address || !this->deviceBuffer) ? data->address : (this->deviceBuffer->getDeviceAddress() + data->bufferInfo.offset)); return std::dynamic_pointer_cast<Device>(deviceBuffer->getDevice())->setBufferAllocationObject(shared_from_this()); };
            virtual inline vk::DescriptorBufferInfo& getBufferInfo() { auto data = this->getDataTyped(); if (this->deviceBuffer) { data->bufferInfo.buffer = std::dynamic_pointer_cast<DeviceBufferData>(this->deviceBuffer->getApi())->buffer; }; return data->bufferInfo; };
            virtual inline vk::DeviceSize& getOffset() { auto data = this->getDataTyped(); return data->bufferInfo.offset; };
            virtual inline vk::DeviceSize& getRange() { auto data = this->getDataTyped(); return data->bufferInfo.range; };

            // 
            virtual inline uintptr_t getDeviceAddress() const override { auto data = this->getDataTyped(); return (data->address || !this->deviceBuffer) ? data->address : (this->deviceBuffer->getDeviceAddress() + data->bufferInfo.offset); };
            virtual inline vk::DescriptorBufferInfo getBufferInfo() const { auto data = this->getDataTyped(); return vk::DescriptorBufferInfo{ std::dynamic_pointer_cast<DeviceBufferData>(this->deviceBuffer->getApi())->buffer, data->bufferInfo.offset, data->bufferInfo.range }; };
            virtual inline const vk::DeviceSize& getOffset() const { auto data = this->getDataTyped(); return data->bufferInfo.offset; };
            virtual inline const vk::DeviceSize& getRange() const { auto data = this->getDataTyped(); return data->bufferInfo.range; };
        };

    };

};
