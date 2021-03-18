#pragma once

//#include <tommy/core.hpp>
#include "./core.hpp"
#include "./device.hpp"
#include "./memory.hpp"

// 
namespace tom {

    // 
    class DeviceBuffer: public std::enable_shared_from_this<DeviceBuffer> {
    protected:  // 
        std::shared_ptr<tom::Device> device = {};
        std::shared_ptr<tom::MemoryAllocation> memoryAllocation = {};

        // 
        vk::Buffer buffer = {};

    public: // 
        DeviceBuffer(const std::shared_ptr<tom::Device>& device): device(device) {
            
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

    // 
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
        virtual void constructor(const vk::DeviceSize& offset = 0ull, const vk::DeviceSize& range = VK_WHOLE_SIZE) {
            if (deviceBuffer) { bufferInfo.buffer = deviceBuffer->getBuffer(); };
            bufferInfo.offset = offset;
            bufferInfo.range = range;
        };

        // 
        virtual inline std::shared_ptr<tom::DeviceBuffer>& getDeviceBuffer() { return deviceBuffer; };
        virtual inline vk::DescriptorBufferInfo& getBufferInfo() { if (deviceBuffer) { bufferInfo.buffer = deviceBuffer->getBuffer(); }; return bufferInfo; };
        virtual inline vk::DeviceSize& getOffset() { return bufferInfo.offset; };
        virtual inline vk::DeviceSize& getRange() { return bufferInfo.range; };

        //
        virtual inline const std::shared_ptr<tom::DeviceBuffer>& getDeviceBuffer() const { return deviceBuffer; };
        virtual inline const vk::DescriptorBufferInfo& getBufferInfo() const { return bufferInfo; };
        virtual inline const vk::DeviceSize& getOffset() const { return bufferInfo.offset; };
        virtual inline const vk::DeviceSize& getRange() const { return bufferInfo.range; };
    };

};
