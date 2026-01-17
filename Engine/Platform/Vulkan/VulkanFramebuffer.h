#pragma once
#include "Renderer/Framebuffer.h"

namespace House {
	class VulkanFramebuffer : public Framebuffer {
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer();
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;
		virtual uint32_t GetAttachmentCount() const override { return static_cast<uint32_t>(_AttachmentImages.size()); }

		virtual MEM::Ref<Texture2D> GetAttachmentTexture(uint32_t index = 0) const override;
		virtual MEM::Ref<Texture2D> GetDepthTextureAttachment() const override;

		virtual const FramebufferSpecification& GetSpecification() const override;
		void Invalidate();
	private:
		FramebufferSpecification _Specification;
		uint32_t _Width, _Height;
		std::vector<MEM::Ref<Texture2D>> _AttachmentImages;
		MEM::Ref<Texture2D> _DepthAttachmentImage;
	};
}