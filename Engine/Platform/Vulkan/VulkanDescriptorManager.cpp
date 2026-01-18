#include "hepch.h"
#include "VulkanDescriptorManager.h"
#include "Core/Application.h"

namespace House {
	DescriptorManager::DescriptorManager(DescriptorManagerSpecification& spec)
	{
		Invalidate(spec);
	}

	DescriptorManager::~DescriptorManager()
	{
		_DescriptorSets.clear();
		_InputDeclarations.clear();
		_StoredResources.clear();
		_Writers.clear();
	}

	void DescriptorManager::WriteInput(std::string_view name, MEM::Ref<VulkanBuffer> buffer)
	{
		const RenderPassInputDeclaration* decl = GetInputDeclaration(name);
		if (decl) {
			_StoredResources[decl->Set][decl->Binding].Set(buffer);
		}
	}

	void DescriptorManager::WriteInput(std::string_view name, MEM::Ref<VulkanTexture> texture, uint32_t index)
	{
		const RenderPassInputDeclaration* decl = GetInputDeclaration(name);
		if (decl) {
			_StoredResources[decl->Set][decl->Binding].Set(texture);
		}
	}

	void DescriptorManager::UpdateSets(VkCommandBuffer cmd, uint32_t frameIndex, VkPipelineLayout layout)
	{
		for (auto& [setIndex, bindings] : _StoredResources) {
			_Writers[setIndex]->Clear();
			bool hasData = false;

			std::vector<VkDescriptorImageInfo> imageInfos;
			imageInfos.reserve(bindings.size());

			std::vector<VkDescriptorBufferInfo> bufferInfos;
			bufferInfos.reserve(bindings.size());

			for (auto& [bindingIndex, resource] : bindings) {
				uint32_t dataIndex = (resource.Data.size() > 1) ? frameIndex : 0;
				auto& currentData = resource.Data[dataIndex];
				if (!currentData) continue;

				switch (resource.Type)
				{
				case ShaderReflectionDataType::UniformBuffer: {
					bufferInfos.push_back(currentData.As<VulkanBuffer>()->DescriptorInfo());
					_Writers[setIndex]->WriteBuffer(bindingIndex, &bufferInfos.back());
					hasData = true;
					break;
				}
				case ShaderReflectionDataType::Sampler2D: {
					imageInfos.push_back(currentData.As<VulkanTexture>()->GetImageDescriptorInfo());
					_Writers[setIndex]->WriteImage(bindingIndex, &imageInfos.back());
					hasData = true;
					break;
				}
				}
			}

			if (hasData) {
				_Writers[setIndex]->Overwrite(_DescriptorSets[frameIndex][setIndex]);
			}
			vkCmdBindDescriptorSets(
				cmd,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				layout,
				setIndex,
				1,
				&_DescriptorSets[frameIndex][setIndex],
				0, nullptr
			);
		}
	}

	void DescriptorManager::Invalidate(DescriptorManagerSpecification& spec)
	{
		_Pool = VulkanDescriptorPool::Builder()
			.SetMaxSets(1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
			.SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
			.Build();

		auto& pipeline = spec.Pipeline;
		auto vulkanPipeline = pipeline.As<VulkanPipeline>();
		auto vulkanShader = vulkanPipeline->GetShader().As<VulkanShader>();
		auto& reflectionData = vulkanShader->GetReflectData();

		uint32_t maxSet = 0;
		for (auto const& [set, bindings] : reflectionData) {
			if (set > maxSet) maxSet = set;
		}

		_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			_DescriptorSets[i].resize(maxSet + 1);
		}

		for (const auto& [set, bindings] : reflectionData) {
			auto& layout = vulkanShader->GetDescriptorLayout(set);

			for (const auto& [binding, input] : bindings) {
				RenderPassInputDeclaration declaration{};
				declaration.Binding = binding;
				declaration.Set = set;
				declaration.Name = input.Name;
				declaration.Type = input.Type;
				_InputDeclarations[input.Name] = declaration;
			}

			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				_DescriptorSets[i][set] = Allocate(layout);
			}

			_Writers[set] = MEM::Ref<VulkanDescriptorWriter>::Create(*vulkanShader->GetDescriptorLayout(set), *_Pool);
		}

	}

	VkDescriptorSet DescriptorManager::Allocate(MEM::Ref<VulkanDescriptorSetLayout>& layout)
	{
		VkDescriptorSet set;
		VkDescriptorSetLayout l = layout->GetDescriptorSetLayout();

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _Pool->GetDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &l;

		CHECKF(vkAllocateDescriptorSets(Application::Get()->GetRenderContext<VulkanContext>().GetDevice(), &allocInfo, &set) != VK_SUCCESS, "Failed to allocate descriptor set");
		return set;
	}

	VkDescriptorSet DescriptorManager::GetDescriptorSet(uint32_t frameIndex, uint32_t setIndex)
	{
		return _DescriptorSets[frameIndex][setIndex];
	}

	std::vector<VkDescriptorSet> DescriptorManager::GetDescriptorSets(uint32_t frameIndex)
	{
		return _DescriptorSets[frameIndex];
	}

	const std::map<std::string, RenderPassInputDeclaration>& DescriptorManager::GetInputDeclarations() const { return _InputDeclarations; }
	const RenderPassInputDeclaration* DescriptorManager::GetInputDeclaration(std::string_view name) const
	{
		std::string nameStr(name);
		if (_InputDeclarations.find(nameStr) == _InputDeclarations.end())
			return nullptr;

		const RenderPassInputDeclaration& decl = _InputDeclarations.at(nameStr);
		return &decl;
	}
}