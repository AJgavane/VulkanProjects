#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "Utilities.h"


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

	const std::vector<const char*> m_validationLayers = {	"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
	const bool m_enableValidationLayers = false;
#else
	const bool m_enableValidationLayers = true;
#endif

	// Vulkan functions
	// -Create functions
	void createInstance();
	void createLogicalDevice();
	VkResult createDebugUtilsMessengerEXT(
		//VkInstance instance, 
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator);

	// -Set Functions
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	// - Get Functions
	void getPhysicalDevice();

	// - Support functions
	// -- checker functions
	bool checkInstanceExtensionSupport(std::vector<const char*> *checkExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice device);
	bool checkValidationLayerSupport();					// Validation layer

	// -- Getter functions
	QueueFamiliesIndices getQueueFamilies(VkPhysicalDevice device);

	// new static member function
	static VKAPI_ATTR VkBool32 VKAPI_CALL m_debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	// -Destroy functions
	void destroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);
};


