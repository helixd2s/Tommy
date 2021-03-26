#pragma once

// 
#include <tommy/internal/vulkan/instance.hpp>
#include <tommy/internal/vulkan/device.hpp>
#include <tommy/internal/vulkan/memory.hpp>
#include <tommy/internal/vulkan/buffer.hpp>


// 
namespace tom {

    namespace vulkan {
        //
        vk::Fence Queue::submitCmds(const std::vector<vk::CommandBuffer>& commandBuffers, vk::SubmitInfo2KHR submitInfo) const {
            if (commandBuffers.size() <= 0) return vk::Fence{};

            // 
            vk::Fence fence = this->getDevice()->getData()->device.createFence(vk::FenceCreateInfo{});

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
            data->queue.submit2KHR(submitInfo, fence);
            return fence;
        };

        //
        std::future<vk::Result> Queue::submitOnce(const std::function<void(const vk::CommandBuffer&)>& cmdFn, const vk::SubmitInfo2KHR& submitInfo) const {
            auto device = this->getDevice()->getData()->device;
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
                return result;
            });
        };

    };
    
};
