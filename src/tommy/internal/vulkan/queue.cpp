#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>
#include <tommy/internal/vulkan/buffer.hpp>
#include <tommy/internal/vulkan/image.hpp>

// 
namespace tom {

    // 
    namespace vulkan {

        //
        vk::Fence Queue::submitCmds(const std::vector<vk::CommandBuffer>& commandBuffers, vk::SubmitInfo2KHR submitInfo) const {
            if (commandBuffers.size() <= 0) return vk::Fence{};

            // 
            vk::Fence fence = std::dynamic_pointer_cast<DeviceData>(this->getDevice()->getData())->device.createFence(vk::FenceCreateInfo{});

            // 
            std::vector<vk::CommandBufferSubmitInfoKHR> commandInfos = {};
            for (auto& commandBuffer : commandBuffers) {
                commandInfos.push_back(vk::CommandBufferSubmitInfoKHR{
                    .commandBuffer = commandBuffer,
                    .deviceMask = ~0x0u
                });
            };

            // 
            submitInfo.commandBufferInfoCount = commandInfos.size();
            submitInfo.pCommandBufferInfos = commandInfos.data();

            // 
            this->getDataTyped()->queue.submit2KHR(submitInfo, fence);
            return fence;
        };

        // 
        vk::Fence Queue::submitCmds(const vk::SubmitInfo2KHR& submitInfo) const {
            auto data = std::dynamic_pointer_cast<QueueData>(this->data);
            auto fence = this->submitCmds(data->commandBuffers, submitInfo);
            for (auto& commandBuffer : data->commandBuffers) {
                
            };
            data->commandBuffers = {};
            return fence;
        };


        // 
        std::future<ApiResult> Queue::copyDeviceBuffers(const std::shared_ptr<tom::BufferAllocation>& src_, const std::shared_ptr<tom::BufferAllocation>& dst_) {
            auto src = std::dynamic_pointer_cast<BufferAllocation>(src_);
            auto dst = std::dynamic_pointer_cast<BufferAllocation>(dst_);

            //auto srcData = std::dynamic_pointer_cast<BufferAllocationData>(src->getData());
            //auto dstData = std::dynamic_pointer_cast<BufferAllocationData>(dst->getData());

            auto srcInfo = src->getBufferInfo();
            auto dstInfo = dst->getBufferInfo();

            return this->submitOnce([src, dst, srcInfo, dstInfo](const vk::CommandBuffer& cmd) {
                vk::BufferCopy2KHR bufferCopy = {
                    .srcOffset = srcInfo.offset,
                    .dstOffset = dstInfo.offset,
                    .size = std::min(srcInfo.range, dstInfo.range)
                };

                cmd.copyBuffer2KHR(vk::CopyBufferInfo2KHR{
                    .srcBuffer = srcInfo.buffer,
                    .dstBuffer = dstInfo.buffer,
                    .regionCount = 1,
                    .pRegions = &bufferCopy
                });
            });
        };

        // 
        std::future<ApiResult> Queue::copyDeviceBufferToImage(const std::shared_ptr<tom::BufferAllocation>& src_, const std::shared_ptr<tom::ImageView>& dst_, const ImageRegion& dstRegion = {}) {
            auto src = std::dynamic_pointer_cast<BufferAllocation>(src_);
            auto dst = std::dynamic_pointer_cast<ImageView>(dst_);

            auto srcInfo = src->getBufferInfo();
            auto dstInfo = std::dynamic_pointer_cast<ImageViewData>(dst->getData());

            return this->submitOnce([src, dst, srcInfo, dstInfo, dstRegion](const vk::CommandBuffer& cmd) {
                vk::BufferImageCopy2KHR bufferImageCopy = {
                    .bufferOffset = srcInfo.offset,
                    .imageSubresource = vk::ImageSubresourceLayers{ 
                        .aspectMask = dstInfo->imageViewInfo.subresourceRange.aspectMask, 
                        .mipLevel = dstInfo->imageViewInfo.subresourceRange.baseMipLevel + dstRegion.mipLevel,
                        .baseArrayLayer = dstInfo->imageViewInfo.subresourceRange.baseArrayLayer,
                        .layerCount = dstInfo->imageViewInfo.subresourceRange.layerCount
                    },
                    .imageOffset = (vk::Offset3D&)dstRegion.offset,
                    .imageExtent = (vk::Extent3D&)dstRegion.extent
                };

                cmd.copyBufferToImage2KHR(vk::CopyBufferToImageInfo2KHR{
                    .srcBuffer = srcInfo.buffer,
                    .dstImage = std::dynamic_pointer_cast<DeviceImageData>(dst->getDeviceImage()->getApi())->image,
                    .dstImageLayout = dstInfo->layoutHistory.back(),
                    .regionCount = 1,
                    .pRegions = &bufferImageCopy
                });
            });
        };

        // 
        std::future<ApiResult> Queue::copyDeviceImageToBuffer(const std::shared_ptr<tom::ImageView>& src_, const std::shared_ptr<tom::BufferAllocation>& dst_, const ImageRegion& srcRegion = {}) {
            auto dst = std::dynamic_pointer_cast<BufferAllocation>(dst_);
            auto src = std::dynamic_pointer_cast<ImageView>(src_);

            auto dstInfo = dst->getBufferInfo();
            auto srcInfo = std::dynamic_pointer_cast<ImageViewData>(src->getData());

            return this->submitOnce([src, dst, srcInfo, dstInfo, srcRegion](const vk::CommandBuffer& cmd) {
                vk::BufferImageCopy2KHR bufferImageCopy = {
                    .bufferOffset = dstInfo.offset,
                    .imageSubresource = vk::ImageSubresourceLayers{ 
                        .aspectMask = srcInfo->imageViewInfo.subresourceRange.aspectMask, 
                        .mipLevel = srcInfo->imageViewInfo.subresourceRange.baseMipLevel + srcRegion.mipLevel,
                        .baseArrayLayer = srcInfo->imageViewInfo.subresourceRange.baseArrayLayer,
                        .layerCount = srcInfo->imageViewInfo.subresourceRange.layerCount
                    },
                    .imageOffset = (vk::Offset3D&)srcRegion.offset,
                    .imageExtent = (vk::Extent3D&)srcRegion.extent
                };

                cmd.copyImageToBuffer2KHR(vk::CopyImageToBufferInfo2KHR{
                    .srcImage = std::dynamic_pointer_cast<DeviceImageData>(src->getDeviceImage()->getApi())->image,
                    .srcImageLayout = srcInfo->layoutHistory.back(),
                    .dstBuffer = dstInfo.buffer,
                    .regionCount = 1,
                    .pRegions = &bufferImageCopy
                });
            });
        };


        // 
        std::future<ApiResult> Queue::submitOnce(const std::function<void(const vk::CommandBuffer&)>& cmdFn, const vk::SubmitInfo2KHR& submitInfo) const {
            auto device = std::dynamic_pointer_cast<DeviceData>(this->getDevice()->getData())->device;
            auto data = this->getDataTyped();
            auto commandBuffers = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo{
                .commandPool = data->commandPool,
                .level = vk::CommandBufferLevel::ePrimary,
                .commandBufferCount = 1
            });
            cmdFn(commandBuffers[0]); // execute command constructor
            auto fence = this->submitCmds(commandBuffers, submitInfo);
            return std::async(std::launch::async | std::launch::deferred, [data=data, fence, commandBuffers, device](){
                auto result = device.waitForFences({fence}, true, 30ull * 1000ull * 1000ull * 1000ull);
                device.destroyFence(fence);
                device.freeCommandBuffers(data->commandPool, commandBuffers);
                return ApiResult(result);
            });
        };

    };
    
};
