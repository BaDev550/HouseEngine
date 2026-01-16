#include "hepch.h"
#include "Texture.h"

#include "Vulkan/VulkanTexture.h"

namespace House {
    MEM::Ref<Texture2D> Texture2D::Create(const std::string& path) {
        switch (RenderAPI::CurrentAPI())
        {
        case GrapichsAPI::Vulkan: return MEM::Ref<VulkanTexture>::Create(path);
        case GrapichsAPI::OpenGL: return nullptr;
        }
    }
    MEM::Ref<Texture2D> Texture2D::Create(uint32_t* data, uint32_t width, uint32_t height) {
        switch (RenderAPI::CurrentAPI())
        {
        case GrapichsAPI::Vulkan: return MEM::Ref<VulkanTexture>::Create(data, width, height);
        case GrapichsAPI::OpenGL: return nullptr;
        }
    }
}