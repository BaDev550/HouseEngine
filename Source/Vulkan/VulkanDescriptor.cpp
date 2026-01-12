#include "hepch.h"
#include "VulkanDescriptor.h"
#include "Core/Application.h"

VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count)
{
    assert(_Bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    _Bindings[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<VulkanDescriptorSetLayout> VulkanDescriptorSetLayout::Builder::Build() const { return std::make_unique<VulkanDescriptorSetLayout>(_Bindings); }
std::unique_ptr<VulkanDescriptorPool> VulkanDescriptorPool::Builder::Build() const { return std::make_unique<VulkanDescriptorPool>(_MaxSets, _PoolFlags, _PoolSizes); }

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : _Context(*Application::Get()->GetVulkanContext()), _Bindings(bindings)
{
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto kv : _Bindings) {
        setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    CHECKF(vkCreateDescriptorSetLayout(_Context.GetDevice(), &descriptorSetLayoutInfo, nullptr, &_DescriptorSetLayout) != VK_SUCCESS, "Failed to create descriptor set layout");
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(_Context.GetDevice(), _DescriptorSetLayout, nullptr);
}

VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
    _PoolSizes.push_back({ descriptorType, count });
    return *this;
}

VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
{
    _PoolFlags = flags;
    return *this;
}

VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::SetMaxSets(uint32_t count)
{
    _MaxSets = count;
    return *this;
}

VulkanDescriptorPool::VulkanDescriptorPool(uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
    : _Context(*Application::Get()->GetVulkanContext())
{
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;
    CHECKF(vkCreateDescriptorPool(_Context.GetDevice(), &descriptorPoolInfo, nullptr, &_DescriptorPool) != VK_SUCCESS, "Failed to create descriptor pool");
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    vkDestroyDescriptorPool(_Context.GetDevice(), _DescriptorPool, nullptr);
}

bool VulkanDescriptorPool::Allocate(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _DescriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;
    return (vkAllocateDescriptorSets(_Context.GetDevice(), &allocInfo, &descriptor) == VK_SUCCESS);
}

void VulkanDescriptorPool::Free(std::vector<VkDescriptorSet>& descriptors)
{
    vkFreeDescriptorSets(_Context.GetDevice(), _DescriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
}

void VulkanDescriptorPool::Reset()
{
    vkResetDescriptorPool(_Context.GetDevice(), _DescriptorPool, 0);
}

VulkanDescriptorWriter::VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool)
    : _SetLayout(setLayout), _Pool(pool)
{
}

VulkanDescriptorWriter& VulkanDescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
{
    assert(_SetLayout._Bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = _SetLayout._Bindings[binding];
    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    _Writes.push_back(write);
    return *this;
}

VulkanDescriptorWriter& VulkanDescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
{
    assert(_SetLayout._Bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = _SetLayout._Bindings[binding];
    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    _Writes.push_back(write);
    return *this;
}

bool VulkanDescriptorWriter::Build(VkDescriptorSet& set)
{
    bool success = _Pool.Allocate(_SetLayout.GetDescriptorSetLayout(), set);
    if (!success) {
        return false;
    }
    Overwrite(set);
    return true;
}

void VulkanDescriptorWriter::Overwrite(VkDescriptorSet& set)
{
    for (auto& write : _Writes) {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(_Pool._Context.GetDevice(), _Writes.size(), _Writes.data(), 0, nullptr);
}
