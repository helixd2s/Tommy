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

        ~DeviceBuffer() {
            if (this->destructor) { 
                this->destructor();
            };
            if (this->buffer) {
                this->device->getDevice().bindBufferMemory2(vk::BindBufferMemoryInfo{ .buffer = this->buffer, .memory = {}, .memoryOffset = 0ull });
                this->device->getDevice().destroyBuffer(this->buffer);
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
            this->buffer = this->device->getDevice().createBuffer(this->info = info);
            this->bindMemory(memoryAllocation);
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
    void MemoryAllocator::allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = buffer;
        auto* allocator = this;
        auto device = this->device.lock();

        if (self->buffer) { // 
            auto memoryRequirements = vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirementsKHR>{ vk::MemoryRequirements2{  }, vk::MemoryDedicatedRequirementsKHR{} };
            device->getDevice().getBufferMemoryRequirements2(vk::BufferMemoryRequirementsInfo2{ .buffer = self->buffer }, memoryRequirements);

            // 
            const auto& memReqs = memoryRequirements.get<vk::MemoryRequirements2>().memoryRequirements;

            // TODO: use native allocation
            auto deviceMemory = device->getDeviceMemoryObject(device->getDevice().allocateMemory(vk::StructureChain<vk::MemoryAllocateInfo, vk::MemoryDedicatedAllocateInfoKHR, vk::ExternalMemoryBufferCreateInfo>{
                vk::MemoryAllocateInfo{
                    .allocationSize = memReqs.size,
                    .memoryTypeIndex = device->getPhysicalDevice()->getMemoryType(memReqs.memoryTypeBits),
                },
                vk::MemoryDedicatedAllocateInfoKHR{ .buffer = self->buffer },
                vk::ExternalMemoryBufferCreateInfo{}
            }.get<vk::MemoryAllocateInfo>()));

            // 
            self->bindMemory(std::make_shared<tom::MemoryAllocation>(deviceMemory, 0ull));
        };
    };

    //
    void MemoryAllocator::allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        buffer->create(info);
        this->allocate(buffer, memUsage);
    };



    //
    void MemoryAllocatorVma::allocateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = buffer;
        auto* allocator = this;
        auto device = this->device.lock();

        if (self->buffer) { // 
            VmaAllocationInfo allocInfo = {};
            VmaAllocationCreateInfo allocCreateInfo = { .usage = memUsage };
            if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
                allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
            };

            // 
            vk::throwResultException(vk::Result(vmaAllocateMemoryForBuffer((const VmaAllocator&)allocator->getAllocator(), self->buffer, &allocCreateInfo, &((VmaAllocation&)self->allocation), &allocInfo)), "VMA buffer allocation failed...");

            // TODO: use native allocation
            auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory); // TODO: IMPORTANT!!!
            self->bindMemory(std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset));
            self->memoryAllocation->getAllocation() = self->allocation;
            //self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData; // not sure...
            deviceMemory->getMapped() = allocInfo.pMappedData;

            //
            self->destructor = [self, allocator](){
                vmaFreeMemory((const VmaAllocator&)allocator->getAllocator(), (VmaAllocation&)self->getMemoryAllocation());
            };
        };
    };

    //
    void MemoryAllocatorVma::allocateAndCreateBuffer(const std::shared_ptr<DeviceBuffer>& buffer, const vk::BufferCreateInfo& info = {}, const VmaMemoryUsage& memUsage = VMA_MEMORY_USAGE_GPU_ONLY) {
        auto self = buffer;
        auto* allocator = this;
        auto device = this->device.lock();

        // 
        self->info = info;

        // 
        VmaAllocationCreateInfo allocCreateInfo = { .usage = memUsage };
        if (allocCreateInfo.usage != VMA_MEMORY_USAGE_GPU_ONLY) { 
            allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; 
        };

        // 
        vk::throwResultException(vk::Result(vmaCreateBuffer((const VmaAllocator&)allocator->getAllocator(), (const VkBufferCreateInfo*)&info, &allocCreateInfo, (VkBuffer*)&self->buffer, &((VmaAllocation&)self->allocation), nullptr)), "VMA buffer allocation failed...");

        // get allocation info
        VmaAllocationInfo allocInfo = {};
        vmaGetAllocationInfo((const VmaAllocator&)allocator->getAllocator(), ((VmaAllocation&)self->allocation), &allocInfo);

        // wrap device memory
        auto deviceMemory = device->getDeviceMemoryObject(allocInfo.deviceMemory);
        self->memoryAllocation = std::make_shared<tom::MemoryAllocation>(deviceMemory, allocInfo.offset);
        self->memoryAllocation->getAllocation() = allocation;
        //self->memoryAllocation->getMappedDefined() = allocInfo.pMappedData; // not sure...
        deviceMemory->getMapped() = allocInfo.pMappedData;

        //deviceMemory->getAllocation() = allocation; // not sure...

        self->destructor = [self, allocator](){
            vmaDestroyBuffer((const VmaAllocator&)allocator->getAllocator(), self->getBuffer(), (VmaAllocation&)self->getMemoryAllocation());
            self->getBuffer() = vk::Buffer{};
            self->getMemoryAllocation() = nullptr;
        };
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
        virtual void constructor(const vk::DeviceSize& offset = 0ull, const vk::DeviceSize& range = VK_WHOLE_SIZE) {
            if (deviceBuffer) { bufferInfo.buffer = deviceBuffer->getBuffer(); };
            bufferInfo.offset = offset;
            bufferInfo.range = range;
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
