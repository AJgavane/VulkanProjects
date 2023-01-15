#pragma once

#define GLFW_INCLUDE_VULKAN		// tells glfw to auto include vulkan for us
#include <GLFW/glfw3.h>

#include <vector>
#include "Utilities.h"

class Mesh
{
public:
	Mesh();

	Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice,
		 VkQueue transferQueue, VkCommandPool transferCommandPool, 
		 std::vector<Vertex>* vertices, std::vector<uint32_t>* indices);

	int getVertexCount();
	VkBuffer getVertexBuffer();

	int getIndexCount();
	VkBuffer getIndexBuffer();

	void destroyBuffers();

	~Mesh();

private:
	int				 m_vertexCount;
	VkBuffer		 m_vertexBuffer;
	VkDeviceMemory	 m_vertexBufferMemory;

	int				 m_indexCount;
	VkBuffer		 m_indexBuffer;
	VkDeviceMemory   m_indexBufferMemory;

	VkPhysicalDevice m_physicalDevice;
	VkDevice		 m_device;

	void createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices);
	void createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices);
	

};

