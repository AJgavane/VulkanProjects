#pragma once

#include <fstream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const int MAX_FRAME_DRAWS = 2;

const std::vector<const char*> deviceExtensions ={
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct Vertex
{
	glm::vec3 a_position;	// vertex position
	glm::vec3 a_color;		// vertex color
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


static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties)
{
	// Properties of Physical device
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((allowedTypes & (i << 1))														// index of mem type must match corresponding bit in allowedTypes
			&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)	// Desired properties bit flags are part of memory type's properties flags
		{
			// This mem type is valid so return its index
			return i;
		}
	}
}


static void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize bufferSize,
						VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags bufferProperties,
						VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
	/*-- CREATE VERTEX BUFFER --*/
	// info to create buffer (doesn't include assigning memory)
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSize;									// Size of buffer: size of 1 vertex * size of vertices
	bufferCreateInfo.usage = bufferUsage;								// Multiple type of buffer possible, we want vertex buffer
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;			// Similar to swap chain images, can share vertex buffers

	VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, buffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Vertex Buffer");
	}

	/*-- GET BUFFER MEMORY REQUIREMENTS --*/
	VkMemoryRequirements memRequirements = {};
	vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

	/*-- ALLOCATE MEMORY TO BUFFER --*/
	VkMemoryAllocateInfo memAllocInfo = {};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.allocationSize = memRequirements.size;
	memAllocInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits, bufferProperties);			// Index of memory type on phy device that has req bit flags
																																		// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT: CPU can interact with the memeory
																																		// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: Allows placement of data straight into buffer after it
																																		// allocates memory to the VkDevice
										

/*-- ALLOCATE MEMORY to VkDeviceMemory --*/
	result = vkAllocateMemory(device, &memAllocInfo, nullptr, bufferMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Allocate Memory for a Vertex Buffer");
	}

	/*-- BIND MEMORY TO THE VERTEX BUFFER --*/
	vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}