#include "render.hpp"
#include "impl.hpp"

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

        if (vkCreateSemaphore(this->self->ptr, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(this->self->ptr, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(this->self->ptr, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
            Stream::log(this->self->p_instance->self->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create synchronization objects for a frame!");
        }

        Sync* self = new Sync();
        self->imageAvailableSemaphore = imageAvailableSemaphore;
        self->renderFinishedSemaphore = renderFinishedSemaphore;
        self->inFlightFence = inFlightFence;
        self->p_device = this;
        this->self->syncs.push_back(self);
        return self;
    }

    Sync::~Sync()
    {
        vkDestroySemaphore(this->p_device->self->ptr, renderFinishedSemaphore, nullptr);
        vkDestroySemaphore(this->p_device->self->ptr, imageAvailableSemaphore, nullptr);
        vkDestroyFence(this->p_device->self->ptr, inFlightFence, nullptr);
    }

    void Sync::wait(Device* device)
    {
        vkWaitForFences(device->self->ptr, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device->self->ptr, 1, &inFlightFence);
    }
}