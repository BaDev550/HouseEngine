#include "hepch.h"
#include "VulkanDescriptorAllocator.h"
#include "Core/Application.h"

DescriptorAllocator::DescriptorAllocator(DescriptorAllocatorSpecification& spec)
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

	const auto& shader = spec.Pipeline->GetShader();
	const auto& reflectionData = shader->GetReflectData();
	
	uint32_t maxSet = 0;
	for (auto const& [set, bindings] : reflectionData) {
		if (set > maxSet) maxSet = set;
	}

	_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		_DescriptorSets[i].resize(maxSet + 1);
	}

	for (const auto& [set, bindings] : reflectionData) {
		auto& layout = spec.Pipeline->GetShader()->GetDescriptorLayout(set);

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
	}

}

void DescriptorAllocator::WriteInput(std::string_view name, MEM::Ref<VulkanBuffer> buffer)
{
	const RenderPassInputDeclaration* decl = GetInputDeclaration(name);
	if (decl) {
		_StoredResources[decl->Set][decl->Binding] = { buffer };
	}
}

void DescriptorAllocator::WriteInput(std::string_view name, MEM::Ref<VulkanTexture> texture, uint32_t index)
{
	const RenderPassInputDeclaration* decl = GetInputDeclaration(name);
	if (decl) {
		_StoredResources[decl->Set][decl->Binding] = { texture };
	}
}

void DescriptorAllocator::UpdateSets(uint32_t frameIndex)
{
	for (auto& [setIndex, bindings] : _StoredResources) {
		_Writer->Clear();
		bool hasData = false;

		for (auto& [bindingIndex, resource] : bindings) {
			uint32_t dataIndex = (resource.Data.size() > 1) ? frameIndex : 0;
			auto& currentData = resource.Data[dataIndex];
			if (!currentData) continue;

			if (resource.Type == ShaderReflectionDataType::UniformBuffer) {
				VkDescriptorBufferInfo info = currentData.As<VulkanBuffer>()->DescriptorInfo();
				_Writer->WriteBuffer(bindingIndex, &info);
				hasData = true;
			}
			else if (resource.Type == ShaderReflectionDataType::Sampler2D) {
				VkDescriptorImageInfo info = currentData.As<VulkanTexture>()->GetImageDescriptorInfo();
				_Writer->WriteImage(bindingIndex, &info);
				hasData = true;
			}
		}
		if (hasData) {
			_Writer->Overwrite(_DescriptorSets[frameIndex][setIndex]);
			_Writer->Clear();
		}
	}
}

VkDescriptorSet DescriptorAllocator::Allocate(MEM::Ref<VulkanDescriptorSetLayout>& layout)
{
	VkDescriptorSet set;
	VkDescriptorSetLayout l = layout->GetDescriptorSetLayout();

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _Pool->GetDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &l;

	CHECKF(vkAllocateDescriptorSets(Application::Get()->GetVulkanContext().GetDevice(), &allocInfo, &set) != VK_SUCCESS, "Failed to allocate descriptor set");
	return set;
}

VkDescriptorSet& DescriptorAllocator::GetDescriptorSet(uint32_t frameIndex, const std::string& name) { return _DescriptorSets[frameIndex][name]; }
const std::map<std::string, RenderPassInputDeclaration>& DescriptorAllocator::GetInputDeclarations() const { return _InputDeclarations; }

const RenderPassInputDeclaration* DescriptorAllocator::GetInputDeclaration(std::string_view name) const
{
	std::string nameStr(name);
	if (_InputDeclarations.find(nameStr) == _InputDeclarations.end())
		return nullptr;

	const RenderPassInputDeclaration& decl = _InputDeclarations.at(nameStr);
	return &decl;
}
