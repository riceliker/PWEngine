#include "render.hpp"
#include "impl.hpp"
#include "stream.hpp"
#include <cstddef>
#include <memory>

// namespace PWEngine::Render 
// {
//     std::unique_ptr<Texture> RenderContext::createTexture(Utils::Image* image)
//     {
//         VkBuffer staging_buffer;
//         VkDeviceMemory staging_buffer_memory;
//         VkImage texture_image;
//         VkDeviceMemory texture_image_memory;

//         /* pixel buffer */
//         size_t image_size = image->size.x * image->size.y * image->depth;

//         VkBufferCreateInfo buffer_info{};
//         buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//         buffer_info.size = image_size;
//         buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//         buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

//         if (vkCreateBuffer(this->self->ptr, &buffer_info, nullptr, &staging_buffer) != VK_SUCCESS) {
//             Stream::log(this->self->p_instance->self->log, Stream::LogType::Warn, Stream::LogFrom::VulkanRender, "failed to create vertex buffer!");
//         }
        
//         allocatorGPUMemory(this, staging_buffer, staging_buffer_memory);

//         void* data;
//         vkMapMemory(this->self->ptr, staging_buffer_memory, 0, image_size, 0, &data);
//             memcpy(data, image->data.data(), image_size);
//         vkUnmapMemory(this->self->ptr, staging_buffer_memory);

//         /* VkImage Buffer*/
//         VkImageCreateInfo image_info{};
//         image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//         image_info.imageType = VK_IMAGE_TYPE_2D;
//         image_info.extent.width = static_cast<uint32_t>(image->size.x);
//         image_info.extent.height = static_cast<uint32_t>(image->size.y);
//         image_info.extent.depth = 1;
//         image_info.mipLevels = 1;
//         image_info.arrayLayers = 1;
//         image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
//         image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
//         image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//         image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//         image_info.samples = VK_SAMPLE_COUNT_1_BIT;
//         image_info.flags = 0; // Optional

//         if (vkCreateImage(this->self->ptr, &image_info, nullptr, &texture_image) != VK_SUCCESS) 
//         {
//             Stream::log(this->self->p_instance->self->log, Stream::LogType::Warn, Stream::LogFrom::VulkanRender, "failed to create image!");
//         }

//         allocatorGPUMemory(this, texture_image, texture_image_memory);

//         std::unique_ptr<Texture> obj;
//         obj->self->staging_buffer = staging_buffer;
//         obj->self->staging_buffer_memory = staging_buffer_memory;
//         obj->size = image->size;
//         return obj;
//     }

//     Texture::~Texture()
//     {
//         vkDestroyBuffer(this->p_device->self->ptr, this->self->staging_buffer, nullptr);
//         vkFreeMemory(this->p_device->self->ptr, this->self->staging_buffer_memory, nullptr);
//     }
// }