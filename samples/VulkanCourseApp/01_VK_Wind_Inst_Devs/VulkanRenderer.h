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

	// phy and logical devices
	struct
	{
		VkPhysicalDevice m_physicalDevice;
		VkDevice		 m_logicalDevice;
	} m_mainDevice;

	VkQueue m_graphicsQueue;

	// Vulkan functions
	// -Create functions
	void createInstance();
	void createLogicalDevice();

	// - Get Functions
	void getPhysicalDevice();

	// - Support functions
	// -- checker functions
	bool checkInstanceExtensionSupport(std::vector<const char*> *checkExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice device);

	// -- Getter functions
	QueueFamiliesIndices getQueueFamilies(VkPhysicalDevice device);
};


