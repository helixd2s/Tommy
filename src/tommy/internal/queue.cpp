#pragma once

// 
#include <tommy/internal/instance.hpp>
#include <tommy/internal/device.hpp>
#include <tommy/internal/memory.hpp>
#include <tommy/internal/buffer.hpp>


// 
namespace tom {

    //
    vk::Fence Queue::submitCmds(const std::vector<vk::CommandBuffer>& commandBuffers, vk::SubmitInfo2KHR submitInfo) const {
        if (commandBuffers.size() <= 0) return vk::Fence{};

        // 
        vk::Fence fence = this->device.lock()->getDevice().createFence(vk::FenceCreateInfo{});

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
        this->queue.submit2KHR(submitInfo, fence);
        return fence;
    };

    //
    std::future<vk::Result> Queue::submitOnce(const std::function<void(const vk::CommandBuffer&)>& cmdFn, const vk::SubmitInfo2KHR& submitInfo) const {
        auto device = this->getDevice();
        auto commandBuffers = device->getDevice().allocateCommandBuffers(vk::CommandBufferAllocateInfo{
            .commandPool = commandPool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1
        });
        cmdFn(commandBuffers[0]); // execute command constructor
        auto fence = this->submitCmds(commandBuffers, submitInfo);
        return std::async(std::launch::async | std::launch::deferred, [self = shared_from_this(), fence, commandBuffers, device](){
            auto result = device->getDevice().waitForFences({fence}, true, 30ull * 1000ull * 1000ull * 1000ull);
            device->getDevice().destroyFence(fence);
            device->getDevice().freeCommandBuffers(self->commandPool, commandBuffers);
            return result;
        });
    };
    
};
