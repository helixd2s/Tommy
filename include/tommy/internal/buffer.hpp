#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {
    namespace vktm {
        
        // 
        class DeviceBuffer: public std::enable_shared_from_this<DeviceBuffer> {
        protected: friend MemoryAllocator; friend MemoryAllocatorVma; friend BufferAllocation; // 
            std::weak_ptr<Device> device = {};
            std::shared_ptr<MemoryAllocation> memoryAllocation = {};
            std::function<void()> destructor = {};

            // 
            vk::Buffer buffer = {};
            vk::DeviceAddress address = 0ull;
            void* allocation = nullptr;

            // 
            vk::BufferCreateInfo info = {};


        public: // 
            DeviceBuffer(const std::shared_ptr<Device>& device, const vk::Buffer& buffer = {}): device(device), buffer(buffer) {
                
            };

            // 
            ~DeviceBuffer() {
                auto device = this->getDevice();
                if (this->destructor) { 
                    this->destructor();
                };
                if (this->buffer) {
                    device->getDevice().bindBufferMemory2(vk::BindBufferMemoryInfo{ .buffer = this->buffer, .memory = {}, .memoryOffset = 0ull });
                    device->getDevice().destroyBuffer(this->buffer);
                    this->buffer = vk::Buffer{};
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
            virtual inline vk::Buffer& getBuffer() { return buffer; };

            // 
            virtual inline const std::shared_ptr<MemoryAllocation>& getMemoryAllocation() const { return memoryAllocation; };
            virtual inline std::shared_ptr<Device> getDevice() const { return device.lock(); };
            virtual inline const vk::Buffer& getBuffer() const { return buffer; };
        };



        // abscent class...
        class BufferAllocation: public std::enable_shared_from_this<BufferAllocation> {
        protected:  // 
            friend DeviceBuffer;
            std::shared_ptr<DeviceBuffer> deviceBuffer = {};

            // 
            vk::DescriptorBufferInfo bufferInfo = { vk::Buffer{}, 0ull, VK_WHOLE_SIZE };
            vk::DeviceAddress address = 0ull;

        public: // 
            BufferAllocation(const std::shared_ptr<DeviceBuffer>& deviceBuffer, const vk::DeviceSize& offset = 0ull, const vk::DeviceSize& range = VK_WHOLE_SIZE): deviceBuffer(deviceBuffer) {
                this->constructor(offset, range);
            };

            // 
            virtual std::shared_ptr<BufferAllocation> constructor(const vk::DeviceSize& offset = 0ull, const vk::DeviceSize& range = VK_WHOLE_SIZE) {
                if (deviceBuffer) { bufferInfo.buffer = deviceBuffer->getBuffer(); };
                bufferInfo.offset = offset;
                bufferInfo.range = range;
                return shared_from_this();
            };

            // 
            virtual inline vk::DeviceAddress getDeviceAddress() { address = (address ? address : (deviceBuffer->getDeviceAddress() + bufferInfo.offset)); return deviceBuffer->getDevice()->setBufferAllocationObject(shared_from_this()); };
            virtual inline vk::DescriptorBufferInfo& getBufferInfo() { if (deviceBuffer) { bufferInfo.buffer = deviceBuffer->getBuffer(); }; return bufferInfo; };
            virtual inline std::shared_ptr<DeviceBuffer>& getDeviceBuffer() { return deviceBuffer; };
            virtual inline vk::DeviceSize& getOffset() { return bufferInfo.offset; };
            virtual inline vk::DeviceSize& getRange() { return bufferInfo.range; };
            virtual inline vk::DeviceAddress& getDeviceAddressDefined() { return (address = address ? address : this->getDeviceAddress()); };

            // 
            virtual inline vk::DeviceAddress getDeviceAddress() const { return address ? address : (deviceBuffer->getDeviceAddress() + bufferInfo.offset); };
            virtual inline vk::DescriptorBufferInfo getBufferInfo() const { return vk::DescriptorBufferInfo{ deviceBuffer->getBuffer(), bufferInfo.offset, bufferInfo.range }; };
            virtual inline const std::shared_ptr<DeviceBuffer>& getDeviceBuffer() const { return deviceBuffer; };
            virtual inline const vk::DeviceSize& getOffset() const { return bufferInfo.offset; };
            virtual inline const vk::DeviceSize& getRange() const { return bufferInfo.range; };
            virtual inline const vk::DeviceAddress& getDeviceAddressDefined() const { return address; };
        };

    };

};
