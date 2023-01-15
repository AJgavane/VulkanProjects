#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <set>

// imported for creating swapchain
#include <algorithm>	

// imported for renderpass::subpass
#include <array>

#include "Mesh.h"

#include <iostream>

#include "Utilities.h"
#include "VulkanValidation.h"


class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow *newWindow);
	void draw();
	void cleanUp();
	
	~VulkanRenderer();

private:
	GLFWwindow *m_window;
	int m_currFrame = 0;

	// Scene Objects
	Mesh firstMesh;

	// Vulkan Components
	// -- Main
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
		//	-- phy and logical devices
	struct
	{
		VkPhysicalDevice m_physicalDevice;
		VkDevice		 m_logicalDevice;
	} m_mainDevice;

	VkQueue			m_graphicsQueue;
	VkQueue			m_presentationQueue;
	VkSurfaceKHR	m_surface;
	VkSwapchainKHR	m_swapchain;

	std::vector<SwapchainImage>		m_swapchainImages;
	std::vector<VkFramebuffer>		m_swapchainFramebuffers;
	std::vector<VkCommandBuffer>	m_commandBuffers;


	// -- Pipeline
	VkPipeline		 m_graphicsPipeline;
	VkPipelineLayout m_pipelineLayout;
	VkRenderPass	 m_renderPass;

	// -- Pools 
	VkCommandPool m_graphicsCmdPool;

	// -- Utility
	VkFormat		m_swapchainImageFormat;
	VkExtent2D		m_swapchainExtent;

	// -- Synchronization
	std::vector<VkSemaphore> m_semaphoreImageAvailable;
	std::vector<VkSemaphore> m_semaphoreRenderFinished;
	std::vector<VkFence>	 m_drawFences;

	
	// Vulkan functions
	// -Create functions
	void createInstance();
	void createDebugCallback();
	void createLogicalDevice();
	void createSurface();
	void createSwapchain();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSynchronization();


	// - Record Function
	void recordCommands();

	// -Set Functions
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult createDebugUtilsMessengerEXT(
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator
	);

	// - Get Functions
	void getPhysicalDevice();


	// - Support functions
	// -- checker functions
	bool checkInstanceExtensionSupport(std::vector<const char*> *checkExtensions);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool checkValidationLayerSupport();					// Validation layer
	bool checkDeviceSuitable(VkPhysicalDevice device);

	// -- Getter functions
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
	SwapchainDetails	 getSwapchainDetails(VkPhysicalDevice device);

	// -Destroy functions
	void destroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);

	// -- Choose Functions
	VkSurfaceFormatKHR	chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
	VkPresentModeKHR	chooseBestPresentationMode(const std::vector<VkPresentModeKHR> &presentationModes);
	VkExtent2D			choseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities);

	// -- Create functions
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectflags);
	VkShaderModule createShaderModule(const std::vector<char> &code);
};




