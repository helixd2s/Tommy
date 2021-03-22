#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {

    // 
    class DeviceBuffer: public std::enable_shared_from_this<DeviceBuffer> {
    protected: friend MemoryAllocator; friend MemoryAllocatorVma; // 
        std::shared_ptr<tom::Device> device = {};
        std::shared_ptr<tom::MemoryAllocation> memoryAllocation = {};

        // 
        vk::Buffer buffer = {};
        vk::DeviceAddress address = 0ull;
        void* allocation = nullptr;

        // 
        vk::BufferCreateInfo info = {};

        // 
        std::function<void()> destructor = {};

    public: // 
        DeviceBuffer(const std::shared_ptr<tom::Device>& device, const vk::Buffer& buffer = {}): device(device), buffer(buffer) {
            
        };

        // 
        ~DeviceBuffer() {
            auto& device = this->device->getDevice();
            if (this->destructor) { 
                this->destructor();
            };
            if (this->buffer) {
                device.bindBufferMemory2(vk::BindBufferMemoryInfo{ .buffer = this->buffer, .memory = {}, .memoryOffset = 0ull });
                device.destroyBuffer(this->buffer);
                this->buffer = vk::Buffer{};
            };
            this->destructor = {};
        };

        // 
        virtual vk::DeviceAddress& getDeviceAddress() {
            return (address = this->device->getDevice().getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = this->buffer }));
        };

        // 
        virtual vk::DeviceAddress getDeviceAddress() const {
            return (address ? address : this->device->getDevice().getBufferAddress(vk::BufferDeviceAddressInfo{ .buffer = this->buffer }));
        };

        // 
        virtual std::shared_ptr<DeviceBuffer> bindMemory(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
            if (memoryAllocation) {
                this->memoryAllocation = memoryAllocation;
                this->device->getDevice().bindBufferMemory2(vk::BindBufferMemoryInfo{
                    .buffer = this->buffer,
                    .memory = this->memoryAllocation->getDeviceMemory()->getMemory(),
                    .memoryOffset = this->memoryAllocation->getOffset()
                });
            };
            return shared_from_this();
        };

        // 
        virtual std::shared_ptr<DeviceBuffer> create(const vk::BufferCreateInfo& info = {}, const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
            this->buffer = this->device->getDevice().createBuffer((this->info = info).setQueueFamilyIndices(this->device->getQueueFamilyIndices()));
            this->bindMemory(memoryAllocation);
            return shared_from_this();
        };

        // 
        virtual inline std::shared_ptr<tom::MemoryAllocation>& getMemoryAllocation() { return memoryAllocation; };
        virtual inline std::shared_ptr<tom::Device>& getDevice() { return device; };
        virtual inline vk::Buffer& getBuffer() { return buffer; };

        // 
        virtual inline const std::shared_ptr<tom::MemoryAllocation>& getMemoryAllocation() const { return memoryAllocation; };
        virtual inline const std::shared_ptr<tom::Device>& getDevice() const { return device; };
        virtual inline const vk::Buffer& getBuffer() const { return buffer; };
    };



    // abscent class...
    class BufferAllocation: public std::enable_shared_from_this<BufferAllocation> {
    protected:  // 
        std::shared_ptr<tom::DeviceBuffer> deviceBuffer = {};

        // 
        vk::DescriptorBufferInfo bufferInfo = { vk::Buffer{}, 0ull, VK_WHOLE_SIZE };
        vk::DeviceAddress address = 0ull;

    public: // 
        BufferAllocation(const std::shared_ptr<tom::DeviceBuffer>& deviceBuffer, const vk::DeviceSize& offset = 0ull, const vk::DeviceSize& range = VK_WHOLE_SIZE): deviceBuffer(deviceBuffer) {
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
        virtual inline vk::DeviceAddress getDeviceAddress() { return address ? address : (deviceBuffer->getDeviceAddress() + bufferInfo.offset); };
        virtual inline std::shared_ptr<tom::DeviceBuffer>& getDeviceBuffer() { return deviceBuffer; };
        virtual inline vk::DescriptorBufferInfo& getBufferInfo() { if (deviceBuffer) { bufferInfo.buffer = deviceBuffer->getBuffer(); }; return bufferInfo; };
        virtual inline vk::DeviceSize& getOffset() { return bufferInfo.offset; };
        virtual inline vk::DeviceSize& getRange() { return bufferInfo.range; };
        virtual inline vk::DeviceAddress& getDeviceAddressDefined() { return address; };

        // 
        virtual inline vk::DeviceAddress getDeviceAddress() const { return address ? address : (deviceBuffer->getDeviceAddress() + bufferInfo.offset); };
        virtual inline const std::shared_ptr<tom::DeviceBuffer>& getDeviceBuffer() const { return deviceBuffer; };
        virtual inline const vk::DescriptorBufferInfo& getBufferInfo() const { return bufferInfo; };
        virtual inline const vk::DeviceSize& getOffset() const { return bufferInfo.offset; };
        virtual inline const vk::DeviceSize& getRange() const { return bufferInfo.range; };
        virtual inline const vk::DeviceAddress& getDeviceAddressDefined() const { return address; };
    };

};
