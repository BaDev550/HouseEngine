#include "hepch.h"
#include "VulkanFramebuffer.h"
#include "VulkanTexture.h"

namespace House {
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: _Specification(spec)
	{
		if (_Specification.Width == 0 || _Specification.Height == 0) {
			_Width = Application::Get()->GetWindow().GetWidth();
			_Height = Application::Get()->GetWindow().GetHeight();
		}
		else {
			_Width = _Specification.Width;
			_Height = _Specification.Height;
		}
		for (auto& attachment : _Specification.Attachments.Attachments) {
			if (IsDepthFormat(attachment.Format)) {
				TextureSpecification spec;
				spec.Format = attachment.Format;
				spec.Width = _Width;
				spec.Height = _Height;
				spec.MipLevels = false;
				spec.Attachment = true;
				_DepthAttachmentImage = Texture2D::Create(spec);
			}
			else {
				TextureSpecification spec;
				spec.Format = attachment.Format;
				spec.Width = _Width;
				spec.Height = _Height;
				spec.MipLevels = false;
				spec.Attachment = true;
				_AttachmentImages.emplace_back(Texture2D::Create(spec));
			}
		}
		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::Invalidate() {

	}

	void VulkanFramebuffer::Bind() const
	{
	}

	void VulkanFramebuffer::Unbind() const
	{
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
	}

	uint32_t VulkanFramebuffer::GetWidth() const { return _Width; }
	uint32_t VulkanFramebuffer::GetHeight() const { return _Height; }
	MEM::Ref<Texture2D> VulkanFramebuffer::GetAttachmentTexture(uint32_t index) const { return _AttachmentImages[index]; }
	MEM::Ref<Texture2D> VulkanFramebuffer::GetDepthTextureAttachment() const { return _DepthAttachmentImage; }
	const FramebufferSpecification& VulkanFramebuffer::GetSpecification() const { return _Specification; }
}
