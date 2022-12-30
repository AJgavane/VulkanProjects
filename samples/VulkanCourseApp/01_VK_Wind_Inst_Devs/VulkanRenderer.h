#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <set>

#include <iostream>

#include "Utilities.h"
#include "VulkanValidation.h"


class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow *newWindow);
	void cleanUp();
	
	
	~VulkanRenderer();

private:
	GLFWwindow *m_window;

	// Vulkan Components
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;

	// phy and logical devices
	struct
	{
		VkPhysicalDevice m_physicalDevice;
		VkDevice		 m_logicalDevice;
	} m_mainDevice;

	VkQueue m_graphicsQueue;
	VkQueue m_presentationQueue;
	VkSurfaceKHR m_surface;

	



	// Vulkan functions
	// -Create functions
	void createInstance();
	void createDebugCallback();
	void createLogicalDevice();
	void createSurface();


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
	QueueFamiliesIndices getQueueFamilies(VkPhysicalDevice device);
	SwapchainDetails	 getSwapchainDetails(VkPhysicalDevice device);

	// -Destroy functions
	void destroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);
};


