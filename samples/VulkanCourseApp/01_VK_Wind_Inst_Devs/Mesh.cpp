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
	// Get size of buffer needed for vertices
	VkDeviceSize  bufferSize = sizeof(Vertex) * vertices->size();

	// Create Buffer and allocate memory to it	
	createBuffer(m_physicalDevice, m_device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
				&m_vertexBuffer, &m_vertexBufferMemory);

	/*-- MAP MEMORY TO VERTEX BUFFER --*/
	void* data;																					// 1. Create pointer to point in normal memory
	vkMapMemory(m_device, m_vertexBufferMemory, 0, bufferSize, 0, &data);	// 2. "Map" the vertex buffer mem to that point
	memcpy(data, vertices->data(), (size_t)bufferSize);							// 3. Copy mem from vertices vector to that point
	vkUnmapMemory(m_device, m_vertexBufferMemory);												// 4. UnMap the vertex buffer memory

	std::cout << "vertexCount: " << m_vertexCount << std::endl;
}

