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

	for (const auto& [set, bt] : shader->GetReflectData()) {
		for (const auto& [binding, name] : bt) {
			RenderPassInputDeclaration declaration{};
			declaration.Binding = binding;
			declaration.Set = set;
			//declaration.Type = type;
			declaration.Name = name;

			_InputDeclarations[set][binding] = declaration;
			_Pool->Allocate(Renderer::GetGlobalDescriptorLayout()->GetDescriptorSetLayout(), _DescriptorSets[name]);
		}
	}
}

void DescriptorAllocator::WriteInput(std::string_view name, MEM::Ref<VulkanBuffer> buffer)
{
	std::string nameStr(name);
	VkDescriptorBufferInfo bufferInfo = buffer->DescriptorInfo();
	VulkanDescriptorWriter(*Renderer::GetGlobalDescriptorLayout(), *Renderer::GetDescriptorPool())
		.WriteBuffer(0, &bufferInfo)
		.Build(_DescriptorSets[nameStr]);
}

void DescriptorAllocator::WriteInput(std::string_view name, MEM::Ref<VulkanTexture> texture, uint32_t index)
{
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

VkDescriptorSet& DescriptorAllocator::GetDescriptorSet(const std::string& name) { return _DescriptorSets[name]; }
const std::map<uint32_t, std::map<uint32_t, RenderPassInputDeclaration>>& DescriptorAllocator::GetInputDeclarations() const { return _InputDeclarations; }

//const RenderPassInputDeclaration* DescriptorAllocator::GetInputDeclaration(std::string_view name) const
//{
//	std::string nameStr(name);
//	if (_InputDeclarations.find(nameStr) == _InputDeclarations.end())
//		return nullptr;
//
//	const RenderPassInputDeclaration& decl = _InputDeclarations.at(nameStr);
//	return &decl;
//}
