#include "hepch.h"
#include "Shader.h"

#include "Vulkan/VulkanShader.h"

namespace House {
	MEM::Ref<Shader> House::Shader::Create(const std::string& vertexPath, const std::string& fragmentPath)
	{
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return MEM::Ref<VulkanShader>::Create(vertexPath, fragmentPath);
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}
	ShaderUniform::ShaderUniform(std::string name, ShaderUniformType type, uint32_t size, uint32_t offset)
		:_Name(name), _Type(type), _Size(size), _Offset(offset)
	{

	}
}