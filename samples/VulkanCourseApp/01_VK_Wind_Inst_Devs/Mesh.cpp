#include "Mesh.h"

#include <iostream>


Mesh::Mesh()
{
}

Mesh::Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue,
			VkCommandPool transferCommandPool, std::vector<Vertex>* vertices)
{
	m_vertexCount = vertices->size();
	m_physicalDevice = newPhysicalDevice;
	m_device = newDevice;
	createVertexBuffer(transferQueue, transferCommandPool, vertices);
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

void Mesh::createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices)
{
	// Get size of buffer needed for vertices
	VkDeviceSize  bufferSize = sizeof(Vertex) * vertices->size();

	//TEMP: Buffers to "stage" vertex data before transferring it to GPU
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	// Create Staging Buffer and allocate memory to it	 
	createBuffer(m_physicalDevice, m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
				&stagingBuffer, &stagingBufferMemory);

	/*-- MAP MEMORY TO STAGING BUFFER --*/
	void* data;																					// 1. Create pointer to point in normal memory
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);	// 2. "Map" the vertex buffer mem to that point
	memcpy(data, vertices->data(), (size_t)bufferSize);							// 3. Copy mem from vertices vector to that point
	vkUnmapMemory(m_device, stagingBufferMemory);												// 4. UnMap the vertex buffer memory


	// CREATE BUFFER w/ TRANSFER_DST_BIT to mark as a recipient of transfer data (also vertex buffer)
	// Buffer memory is to be DEVICE_LOCAL_BIT ==> m/o is on GPU and only accessible by it and not CPU (HOST)
	createBuffer(m_physicalDevice, m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_vertexBuffer, &m_vertexBufferMemory);

	// Copying staging buffer to vertex buffer on GPU
	copyBuffer(m_device, transferQueue, transferCommandPool, stagingBuffer, m_vertexBuffer, bufferSize);

	// Cleanup the staging buffer
	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

