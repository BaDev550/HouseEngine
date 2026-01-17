#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"

namespace House {
	struct FramebufferTextureSpecification {
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(TextureImageFormat format) : Format(format) {}

		TextureImageFormat Format;
	};

	struct FramebufferAttachmentSpecification {
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification>& attachments) : Attachments(attachments) {}
		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification {
		uint32_t Width = 0;
		uint32_t Height = 0;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		float DepthClearValue = 0.0f;
		bool UsingSwapChain = false;
		FramebufferAttachmentSpecification Attachments;
	};

	class Framebuffer : public MEM::RefCounted {
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetAttachmentCount() const = 0;

		virtual MEM::Ref<Texture2D> GetAttachmentTexture(uint32_t index = 0) const = 0;
		virtual MEM::Ref<Texture2D> GetDepthTextureAttachment() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static MEM::Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}