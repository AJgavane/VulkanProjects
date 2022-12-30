#pragma once

const std::vector<const char*> deviceExtensions ={
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Indices (location) of queue families if they exists at all
struct QueueFamiliesIndices
{
	int graphicsFamily = -1;		// location of graphics queue family
	int presentationFamily = -1;	// location of presentation queue family

	// Check if queue families are valid
	bool isValid()
	{
		return (graphicsFamily >= 0 && presentationFamily >=0);
	}
};

struct SwapchainDetails
{
	VkSurfaceCapabilitiesKHR		surfaceCapabilities;	//Surface properties: image size/extent, etc.
	std::vector<VkSurfaceFormatKHR>	formats;			//Surface image formats: RGBA and bits for each
	std::vector<VkPresentModeKHR>		presentationModes;		//How images should be presented to the screen
};