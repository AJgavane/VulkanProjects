#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// With Vulkan we are going to do tons of error checking
// we will be checking generic error
#include <stdexcept>
#include <vector>

#include <iostream>

GLFWwindow *window;

void initWindow(std::string wName = "Test Window", const int width = 800, const int height = 600)
{
	// initialize glfw
	glfwInit();

	//Set glfw to not work with opengl
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// set Resizeable window to FALSE: bcoz it gets a little bit complicated
	//	-- you end up recreating lot of things that are already created :(
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


	window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}

int main()
{
	// create window
	initWindow("Test Window", 800, 600);

	// loop until close
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}


	//clean up
	// Destroy glfw window and stop glfw
	glfwDestroyWindow(window);
	glfwTerminate();

	
	return 0;
}
