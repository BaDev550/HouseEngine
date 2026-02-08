#pragma once
#include "Utilities/Memory.h"
#include "Renderer/Material.h"
#include "VulkanDescriptor.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorManager.h"
#include <glm/glm.hpp>

namespace House {
	class VulkanMaterial : public Material {
	public:
		VulkanMaterial(MEM::Ref<Shader>& shader);
		~VulkanMaterial();

		virtual void Build() override;
		void Bind(VkCommandBuffer cmd, VkPipelineLayout layout);

		virtual void Set(const std::string& name, float value) override;
		virtual void Set(const std::string& name, int value) override;
		virtual void Set(const std::string& name, bool value) override;
		virtual void Set(const std::string& name, const glm::vec2& value) override;
		virtual void Set(const std::string& name, const glm::vec3& value) override;
		virtual void Set(const std::string& name, const glm::vec4& value) override;
		virtual void Set(const std::string& name, const MEM::Ref<Texture2D>& value) override;

		virtual float& GetFloat(const std::string& name) override;
		virtual glm::vec2& GetVector2(const std::string& name) override;
		virtual glm::vec3& GetVector3(const std::string& name) override;

		virtual void MaterialDataChanged() override;

		template<typename T>
		void Set(const std::string& name, const T& value) {
			auto decl = FindUniformDeclaration(name);
			CHECKF(!decl, "Failed to find uniform");
			
			_StorageBuffer.Write((uint8_t*)&value, decl->GetSize(), decl->GetOffset());
		}

		template<typename T>
		T& Get(const std::string& name)
		{
			auto decl = FindUniformDeclaration(name);
			CHECKF(!decl, "Could not find uniform");
			
			auto& buffer = _StorageBuffer;
			return buffer.Read<T>(decl->GetOffset());
		}

		VkDescriptorSet GetDescriptorSet(int frameIndex) const { return _DescriptorSets[frameIndex]; }
	private:
		const ShaderUniform* FindUniformDeclaration(const std::string& name);

		MaterialData _Data;
		MEM::Ref<VulkanShader> _Shader;
		MEM::Ref<VulkanBuffer> _UniformBuffer;

		DescriptorManager _DescriptorManager;
		std::vector<VkDescriptorSet> _DescriptorSets;

		DataBuffer _StorageBuffer;

		friend class Model;
	};
}