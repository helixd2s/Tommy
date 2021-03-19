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
        void* allocation = nullptr;

    public: // 
        DeviceBuffer(const std::shared_ptr<tom::Device>& device, const vk::Buffer& buffer = {}): device(device), buffer(buffer) {
            
        };

        // 
        virtual void bindMemory(const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
            if (memoryAllocation) {
                this->memoryAllocation = memoryAllocation;
                this->device->getDevice().bindBufferMemory2(vk::BindBufferMemoryInfo{
                    .buffer = this->buffer,
                    .memory = this->memoryAllocation->getDeviceMemory()->getMemory(),
                    .memoryOffset = this->memoryAllocation->getOffset()
                });
            };
        };

        // 
        virtual void create(const vk::BufferCreateInfo& info = {}, const std::shared_ptr<tom::MemoryAllocation>& memoryAllocation = {}) {
            this->buffer = this->device->getDevice().createBuffer(info);
            this->bindMemory(memoryAllocation);
        };


        // TODO: correct create info
        virtual vk::Result vmaAllocate(const vk::BufferCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
            VmaAllocationCreateInfo allocCreateInfo = {};
            allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
            };

            // 
            vk::Result result = vk::Result(vmaCreateBuffer(this->device->getAllocator(), (const VkBufferCreateInfo*)&info, &allocCreateInfo, (VkBuffer*)&buffer, &((VmaAllocation&)this->allocation), nullptr));

            // get allocation info
            VmaAllocationInfo allocInfo = {};
            vmaGetAllocationInfo(this->device->getAllocator(), ((VmaAllocation&)this->allocation), &allocInfo);

            // wrap device memory
            auto deviceMemory = this->device->getDeviceMemoryObject(allocInfo.deviceMemory);
            this->memoryAllocation = std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset);
            this->memoryAllocation->getAllocation() = allocation;
            deviceMemory->getMapped() = allocInfo.pMappedData;
            //deviceMemory->getAllocation() = allocation; // not sure...

            //
            return result;
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
