#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {
    namespace vktm {

        // 
        class DeviceBufferData : public DeviceBufferBase { public: 
            vk::Buffer buffer = {};
            vk::DeviceAddress address = 0ull;
            vk::BufferCreateInfo info = {};
        };

        // 
        class BufferAllocationData : public BufferAllocationBase { public: 
            vk::DescriptorBufferInfo bufferInfo = { vk::Buffer{}, 0ull, VK_WHOLE_SIZE };
            vk::DeviceAddress address = 0ull;
        };



        // 
        class DeviceBuffer: public std::enable_shared_from_this<DeviceBuffer> {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma; friend BufferAllocation; // 
            std::weak_ptr<Device> device = {};
            std::shared_ptr<MemoryAllocation> memoryAllocation = {};
            std::shared_ptr<DeviceBufferData> data = {};
            std::function<void()> destructor = {};
            void* allocation = nullptr;


        public: // 
            DeviceBuffer(const std::shared_ptr<Device>& device, const vk::Buffer& buffer = {}): device(device) {
                data = std::make_shared<DeviceBufferData>();
                data->buffer = buffer;
            };

            // 
            ~DeviceBuffer() {
                auto device = this->getDevice()->getData();
                if (this->destructor) { 
                    this->destructor();
                };
                if (data->buffer) {
                    device->device.bindBufferMemory2(vk::BindBufferMemoryInfo{ .buffer = data->buffer, .memory = {}, .memoryOffset = 0ull });
                    device->device.destroyBuffer(data->buffer);
                    data->buffer = vk::Buffer{};
                };
                this->destructor = {};
            };

            // 
            virtual vk::DeviceAddress& getDeviceAddress();
            virtual vk::DeviceAddress getDeviceAddress() const;

            // 
            virtual std::shared_ptr<DeviceBuffer> bindMemory(const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});
            virtual std::shared_ptr<DeviceBuffer> create(const vk::BufferCreateInfo& info = {}, const std::shared_ptr<MemoryAllocation>& memoryAllocation = {});

            // 
            virtual inline std::shared_ptr<MemoryAllocation>& getMemoryAllocation() { return memoryAllocation; };
            virtual inline std::shared_ptr<Device> getDevice() { return device.lock(); };
            virtual inline std::shared_ptr<DeviceBufferData> getData() { return data; };

            // 
            virtual inline const std::shared_ptr<MemoryAllocation>& getMemoryAllocation() const { return memoryAllocation; };
            virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
            virtual inline std::shared_ptr<DeviceBufferData> getData() { return data; };
        };



        // abscent class...
        class BufferAllocation: public std::enable_shared_from_this<BufferAllocation> {
        protected:  // 
            friend DeviceBuffer;
            std::shared_ptr<DeviceBuffer> deviceBuffer = {};
            std::shared_ptr<BufferAllocationData> data = {};

        public: // 
            BufferAllocation(const std::shared_ptr<DeviceBuffer>& deviceBuffer, const vk::DeviceSize& offset = 0ull, const vk::DeviceSize& range = VK_WHOLE_SIZE): deviceBuffer(deviceBuffer) {
                data = std::make_shared<BufferAllocationData>();
                this->constructor(offset, range);
            };

            // 
            virtual std::shared_ptr<BufferAllocation> constructor(const vk::DeviceSize& offset = 0ull, const vk::DeviceSize& range = VK_WHOLE_SIZE) {
                if (deviceBuffer) { data->bufferInfo.buffer = deviceBuffer->getData()->buffer; };
                data->bufferInfo.offset = offset;
                data->bufferInfo.range = range;
                return shared_from_this();
            };

            // 
            virtual inline vk::DeviceAddress getDeviceAddress() { data->address = (data->address ? data->address : (deviceBuffer->getDeviceAddress() + data->bufferInfo.offset)); return deviceBuffer->getDevice()->setBufferAllocationObject(shared_from_this()); };
            virtual inline vk::DescriptorBufferInfo& getBufferInfo() { if (deviceBuffer) { data->bufferInfo.buffer = deviceBuffer->getData()->buffer; }; return data->bufferInfo; };
            virtual inline std::shared_ptr<DeviceBuffer>& getDeviceBuffer() { return deviceBuffer; };
            virtual inline vk::DeviceSize& getOffset() { return data->bufferInfo.offset; };
            virtual inline vk::DeviceSize& getRange() { return data->bufferInfo.range; };
            virtual inline vk::DeviceAddress& getDeviceAddressDefined() { return (data->address = data->address ? data->address : this->getDeviceAddress()); };

            // 
            virtual inline vk::DeviceAddress getDeviceAddress() const { return data->address ? data->address : (deviceBuffer->getDeviceAddress() + data->bufferInfo.offset); };
            virtual inline vk::DescriptorBufferInfo getBufferInfo() const { return vk::DescriptorBufferInfo{ deviceBuffer->getData()->buffer, data->bufferInfo.offset, data->bufferInfo.range }; };
            virtual inline const std::shared_ptr<DeviceBuffer>& getDeviceBuffer() const { return deviceBuffer; };
            virtual inline const vk::DeviceSize& getOffset() const { return data->bufferInfo.offset; };
            virtual inline const vk::DeviceSize& getRange() const { return data->bufferInfo.range; };
            virtual inline const vk::DeviceAddress& getDeviceAddressDefined() const { return data->address; };
        };

    };

};
