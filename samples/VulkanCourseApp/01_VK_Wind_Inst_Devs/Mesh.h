#pragma once

#define GLFW_INCLUDE_VULKAN		// tells glfw to auto include vulkan for us
#include <GLFW/glfw3.h>

#include <vector>
#include "Utilities.h"

class Mesh
{
public:
	Mesh();

	Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices);

	int getVertexCount();
	VkBuffer getVertexBuffer();

	void destroyVertexBuffer();

	~Mesh();

private:
	int				m_vertexCount;
	VkBuffer		m_vertexBuffer;
	VkDeviceMemory	m_vertexBufferMemory;

	VkPhysicalDevice m_physicalDevice;
	VkDevice		 m_device;

	void createVertexBuffer(std::vector<Vertex>* vertices);


};

