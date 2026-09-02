#include "render.hpp"

namespace PWEngine::Render 
{
    Sync* Device::createSync()
    {
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(this->ptr, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(this->ptr, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(this->ptr, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }

        Sync* self = new Sync();
        self->imageAvailableSemaphore = imageAvailableSemaphore;
        self->renderFinishedSemaphore = renderFinishedSemaphore;
        self->inFlightFence = inFlightFence;
        self->p_device = this;
        this->syncs.push_back(self);
        return self;
    }
    void Sync::wait(Device* device)
    {
        vkWaitForFences(device->ptr, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device->ptr, 1, &inFlightFence);
    }
}