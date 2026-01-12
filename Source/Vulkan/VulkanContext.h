#pragma once
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanPipelineConfig {
	VulkanPipelineConfig() {}
	VulkanPipelineConfig(const VulkanPipelineConfig&) = delete;
	VulkanPipelineConfig& operator=(VulkanPipelineConfig&) = delete;

	VkPipelineInputAssemblyStateCreateInfo InputAssembyCreateInfo{};
	VkPipelineViewportStateCreateInfo	   ViewportStateCreateInfo{};
	VkPipelineRasterizationStateCreateInfo ResterizationStateCreateInfo{};
	VkPipelineMultisampleStateCreateInfo   MultisampleStateCreateInfo{};
	VkPipelineColorBlendAttachmentState    ColorBlendAttachment{};
	VkPipelineColorBlendStateCreateInfo    ColorBlendStateCreateInfo{};

	std::vector<VkDynamicState> DynamicStateEnables;
	VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo{};

	VkRenderPass RenderPass = VK_NULL_HANDLE;
};

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

class Window;
class VulkanContext
{
public:
	static void DefaultPipelineConfigInfo(VulkanPipelineConfig& config);
	VulkanContext();
	VulkanContext(const VulkanContext&) = delete;
	VulkanContext& operator=(const VulkanContext&) = delete;
	~VulkanContext();

	VkDevice GetDevice() const { return _Device; }
	VkPhysicalDevice GetPhysicalDevice() const { return _PhysicalDevice; }
	VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const { return _PhysicalDeviceProperties; }
	VkQueue GetGraphicsQueue() const { return _GraphicsQueue; }
	VkQueue GetPresentQueue() const { return _PresentQueue; }
	VkCommandPool GetCommandPool() const { return _CommandPool; }
	VkSurfaceKHR GetSurface() const { return _Surface; }
	void WaitToDeviceIdle();
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryflags, VkImage& image, VkDeviceMemory& memory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	QueueFamilyIndices FindPhysicalDeviceQueueFamilies() { return FindQueueFamilies(_PhysicalDevice); }
	SwapChainSupportDetails QuerySwapChainSupportOnPhysicalDevice() { return QuerySwapChainSupport(_PhysicalDevice); }
private:
	void CreateInstance();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSurface();
	void CreateContextCommandPool();
	bool IsPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool HasRequiredDeviceExtensions(VkPhysicalDevice device);
	bool CheckValidationLayerSupport();
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer cmd);
	std::vector<const char*> GetRequiredExtensions();
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
private:
	VkInstance _Instance;
	VkPhysicalDeviceProperties _PhysicalDeviceProperties;
	VkPhysicalDevice _PhysicalDevice;
	VkDevice _Device;

	VkCommandPool _CommandPool;
	VkQueue _GraphicsQueue;
	VkQueue _PresentQueue;
	VkDebugUtilsMessengerEXT _DebugMessenger;

	VkSurfaceKHR _Surface;
	Window& _Window;

	const std::vector<const char*> _ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> _DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef _DEBUG
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif
};

