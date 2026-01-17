#pragma once
#include <vector>
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
}