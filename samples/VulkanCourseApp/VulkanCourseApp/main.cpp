#define  GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// force angle in radians
#define GLM_FORCE_RADIANS
// specific to VULKAN 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow  *window = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);

	// Vulkan code
	// how many instance / extensions are supported
	uint32_t extensionCount = 0;

	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::cout << "Num of Extensions supported: " << extensionCount << std::endl;

	// check if glm is working
	glm::mat4 testMatrix(1.0);

	glm::vec4 testVector(1.0);

	auto testResults = testMatrix * testVector;

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	// Destroy Window
	glfwDestroyWindow(window);

	//Deactivate GLFW 
	glfwTerminate();
	
	return 0;
}
