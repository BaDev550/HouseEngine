#include "hepch.h"
#include "Texture.h"

#include "Vulkan/VulkanTexture.h"

namespace House {
    MEM::Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec)
    {
        switch (RenderAPI::CurrentAPI())
        {
        case GrapichsAPI::Vulkan: return MEM::Ref<VulkanTexture>::Create(spec);
        case GrapichsAPI::OpenGL: return nullptr;
        }
    }
    MEM::Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec, const std::string& path) {
        switch (RenderAPI::CurrentAPI())
        {
        case GrapichsAPI::Vulkan: return MEM::Ref<VulkanTexture>::Create(spec, path);
        case GrapichsAPI::OpenGL: return nullptr;
        }
    }
    MEM::Ref<Texture2D> Texture2D::Create(const TextureSpecification& spec, DataBuffer data) {
        switch (RenderAPI::CurrentAPI())
        {
        case GrapichsAPI::Vulkan: return MEM::Ref<VulkanTexture>::Create(spec, data);
        case GrapichsAPI::OpenGL: return nullptr;
        }
    }
}