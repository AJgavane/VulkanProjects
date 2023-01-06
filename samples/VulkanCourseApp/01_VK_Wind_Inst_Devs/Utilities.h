#pragma once

#include <fstream>

const int MAX_FRAME_DRAWS = 2;

const std::vector<const char*> deviceExtensions ={
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Indices (location) of queue families if they exists at all
struct QueueFamilyIndices
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
	std::vector<VkPresentModeKHR>	presentationModes;		//How images should be presented to the screen
};

struct SwapchainImage
{
	VkImage image;
	VkImageView imageView;
};

 
static std::vector<char> readFile(const std::string &filename)
{
	// Open stream from given file
	// std::ios::binary == tells stream to read file as binary
	// std::ios::ate	== tells stream to start reading file from the end of the file
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	// Check if file stream successfully opened
	if(!file.is_open())
	{
		throw std::runtime_error("Failed to open a FILE!");
	}

	// Get current read position and use to resize the file buffer
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> fileBuffer(fileSize);

	// Move the read position (seek to) to the start of the file.
	file.seekg(0);

	file.read(fileBuffer.data(), fileSize);

	file.close();

	return fileBuffer;
}