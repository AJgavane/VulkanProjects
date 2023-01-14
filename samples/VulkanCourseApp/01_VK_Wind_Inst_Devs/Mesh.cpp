#include "Mesh.h"

#include <iostream>


Mesh::Mesh()
{
}

Mesh::Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices)
{
	m_vertexCount = vertices->size();
	m_physicalDevice = newPhysicalDevice;
	m_device = newDevice;
	createVertexBuffer(vertices);
}

int Mesh::getVertexCount()
{
	return m_vertexCount;
}

VkBuffer Mesh::getVertexBuffer()
{
	return m_vertexBuffer;
}

void Mesh::destroyVertexBuffer()
{
	vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
	vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);
}


Mesh::~Mesh()
{
}

void Mesh::createVertexBuffer(std::vector<Vertex>* vertices)
{
	/*-- CREATE VERTEX BUFFER --*/
	// info to create buffer (doesn't include assigning memory)
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = sizeof(Vertex) * m_vertexCount;				// Size of buffer: size of 1 vertex * size of vertices
	bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;				// Multiple type of buffer possible, we want vertex buffer
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;				// Similar to swap chain images, can share vertex buffers

	VkResult result = vkCreateBuffer(m_device, &bufferCreateInfo, nullptr, &m_vertexBuffer);
	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Vertex Buffer");
	}

	/*-- GET BUFFER MEMORY REQUIREMENTS --*/
	VkMemoryRequirements memRequirements = {};
	vkGetBufferMemoryRequirements(m_device, m_vertexBuffer, &memRequirements);

	/*-- ALLOCATE MEMORY TO BUFFER --*/
	VkMemoryAllocateInfo memAllocInfo = {};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.allocationSize = memRequirements.size;
	memAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,									// Index of memory type on phy device that has req bit flags
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
									   /*  CPU can interact with memory   */  /* Allows placement of data straight into buffer after mapping (else we'll have to manually specify) */

	/*-- ALLOCATE MEMORY to VkDeviceMemory --*/
	result = vkAllocateMemory(m_device, &memAllocInfo, nullptr, &m_vertexBufferMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Allocate Memory for a Vertex Buffer");
	}

	/*-- BIND MEMORY TO THE VERTEX BUFFER --*/
	vkBindBufferMemory(m_device, m_vertexBuffer, m_vertexBufferMemory, 0);

	/*-- MAP MEMORY TO VERTEX BUFFER --*/
	void* data;																					// 1. Create pointer to point in normal memory
	vkMapMemory(m_device, m_vertexBufferMemory, 0, bufferCreateInfo.size, 0, &data);	// 2. "Map" the vertex buffer mem to that point
	memcpy(data, vertices->data(), (size_t)bufferCreateInfo.size);							// 3. Copy mem from vertices vector to that point
	vkUnmapMemory(m_device, m_vertexBufferMemory);												// 4. UnMap the vertex buffer memory

	std::cout << "vertexCount: " << m_vertexCount << std::endl;
}

uint32_t Mesh::findMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties)
{
	// Properties of Physical device
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryProperties);

	for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{	
		if((allowedTypes & (i << 1))														// index of mem type must match corresponding bit in allowedTypes
			&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)	// Desired properties bit flags are part of memory type's properties flags
		{
			// This mem type is valid so return its index
			return i;
		}
	}
}
