#include "VulkanRenderer.h"



VulkanRenderer::VulkanRenderer()
{
}

int VulkanRenderer::init(GLFWwindow* newWindow)
{
	m_window = newWindow;

	try
	{
		createInstance();
		createDebugCallback();
		createSurface();
		getPhysicalDevice();
		createLogicalDevice();
	}
	catch (const std::runtime_error &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return 0;
}

void VulkanRenderer::createInstance()
{
	// add validation check
	if(enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("Validation Layer requested, But not available!");
	}

	// Information about the application itself (not just vk but the whole application)
	// NOTE:Most data here doesn't affect the programmer and its for developer convienence
	// Debugging and log informations
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan App";				//Custom application's name
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);	// Custom App's version
	appInfo.pEngineName = "No Engine";						//Custon App engine's name
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);		// Custom app engine's version
	appInfo.apiVersion = VK_API_VERSION_1_3;				// the vulkan version
	
	// Creation information for a VkInstance (Vulkan Instance)
	VkInstanceCreateInfo createInfo = {  };

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	//createInfo.pNext = // pointed to abs anything, extended information
	//createInfo.flags = VK_WHATEVER | VK_ANYTHING; // seting bit fields
	createInfo.pApplicationInfo = &appInfo;	//VK application info


	// Create list to hold instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	// what extension is required in order to interface w/ VK
	// need to know how many extensions and pointers to them
	
	// Set up Extension intances that we'll use
	uint32_t glfwExtensionCount = 0;		// glfw may req multiple extensions. its a pointer
	const char** glfwExtensions;			// extensions passed array of cstrings,
											// so need pointer (the array) to pointer (to cstring)
											// Array of string, and string is array of characters

	// Get GLFW extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	// ADD glfw extenstions to the list of instanceExtensions
	for(size_t i = 0; i < glfwExtensionCount; i++)
	{
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	// validation check on extensions
	if(enableValidationLayers)
	{
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	// Check if instanceExtension supported!
	if(!checkInstanceExtensionSupport(&instanceExtensions))
	{
		throw std::runtime_error("VkInstance does not support required extensions!");
	}


	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// validation layer
	// TODO:setup validation layer that instance will use.
	// TODO: Updated validation layer!!
	if(enableValidationLayers)
	{
		std::cout << "Validation Layer: ON!" << std::endl;
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		std::cout << "Validation Layer: OFF!" << std::endl;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}


	// Create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);

	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create VK Instance!");
	} 
}

void VulkanRenderer::createLogicalDevice()
{
	// get queue family indieces for the chosen phy dev
	QueueFamiliesIndices indices = getQueueFamilies(m_mainDevice.m_physicalDevice);

	// Vector for queue creation information, and set for family indices 
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = {indices.graphicsFamily, indices.presentationFamily};



	// QUeues logical device needs to create and info to do so
	for (int queueFamilyIndex : queueFamilyIndices) 
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;				// index of the family to create a queue from
		queueCreateInfo.queueCount = 1;										// number of queues to create;
		float priority = 1.0f;
		queueCreateInfo.pQueuePriorities = &priority;						// VK needs to know how to handle multiple queues, so decide priority (1 - high, 0 -low)

		queueCreateInfos.push_back(queueCreateInfo);
	}

	// information to create logical device (sometimes called "device")
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());		// number of queue create infos
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();								// List of create infos so dev can create requried queues
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());	//Num of logical dev ext. instance handles extensions
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();							// list of enabled logical dev ext
	//deviceCreateInfo.enabledLayerCount = 0;

	// physical device features used by logical device
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;		// enable shader stages:VS, GS, TS, etc

	// create the logical device for the given physical device
	VkResult result = vkCreateDevice(m_mainDevice.m_physicalDevice, &deviceCreateInfo, nullptr, &m_mainDevice.m_logicalDevice);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Logical Device");
	}

	// Queues are created at the same time as the Device
	// So we want to handle the queues
	// From given logical device, of given queue family, of given queue index (0 since only 1 queue), place ref in given vkQueue
	vkGetDeviceQueue(m_mainDevice.m_logicalDevice, indices.graphicsFamily, 0, &m_graphicsQueue);	// grab our queue for us
	vkGetDeviceQueue(m_mainDevice.m_logicalDevice, indices.presentationFamily, 0, &m_presentationQueue);	// grab our queue for us
}

void VulkanRenderer::createSurface()
{
	// GLFW is going to do all the work for us!!

	// Create Surface
	//   -- creates a surface createInfo struct
	//   -- runs a create surface function
	//   -- returns result

	VkResult result = glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface);

	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a surface");
	}

}

VkResult VulkanRenderer::createDebugUtilsMessengerEXT(
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		return func(m_instance, pCreateInfo, pAllocator, &m_debugMessenger);
	} else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanRenderer::createDebugCallback()
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT	createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if(createDebugUtilsMessengerEXT(&createInfo, nullptr) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to setup DEBUG MESSENGER!");
	}
}

void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT -- gave too many warnings!
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = m_debugCallback;
}

void VulkanRenderer::getPhysicalDevice()
{
	// Get the number of devices
	// Create a list of that size
	// Populate that list

	//Enumerate Physical devices VKInstance can access
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	// If no devices are available, then Vulkan not supported
	if (deviceCount == 0)
		throw std::runtime_error("Can't find GPU's that support Vulkan Instances");
	
	// Create a list of physical devices using count and populate it
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, deviceList.data());

	// Temp: Just pick first device
	//m_mainDevice.m_physicalDevice = deviceList[0];
	
	// find device that is actually valid for what we want to do
	for(const auto &device: deviceList)
	{
		if(checkDeviceSuitable(device))
		{
			m_mainDevice.m_physicalDevice = device;
			break;
		}
	}
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
	// How many extensions VK support, so that we can create the array
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// create a list of vkExtensionProperties using count
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// the list "extensions" will get populated with all the extensions the device supports
	// and we will loop through them

	//Check if given extensions are in the list of available extensions
	for(const auto &checkExtension : *checkExtensions)	
	{
		bool hasExtensions = false;
		for(const auto &extension : extensions)
		{
			// string compare. we need to compare the names of the extensions
			if(strcmp(checkExtension, extension.extensionName))
			{
				hasExtensions = true;
				break;
			}
		}
		if(!hasExtensions)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	/*
	// Info about device itself: name, id, vendor, etc
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	

	// info about what these devices can do -- geo shader, tess shader, widelines, etc
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	
	*/

	// We will be checking queue family is supported
		// - for that we will be creating a struct where we can store what queues phy device can handle
		
	QueueFamiliesIndices indices = getQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapchainValid = false;

	if(extensionsSupported)
	{
		SwapchainDetails swapchainDetails = getSwapchainDetails(device);
		swapchainValid = !swapchainDetails.presentationModes.empty() && !swapchainDetails.formats.empty();
	}

	return (indices.isValid() && extensionsSupported && swapchainValid);
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	// Get device extension count
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	// double check if you have any extension counts:
	if (extensionCount == 0)
		return false;

	// Populate list of extensions
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// Check for extension
	for(const auto &deviceExtension: deviceExtensions)
	{
		bool hasExtension = false;
		for(const auto extension: availableExtensions)
		{
			if(strcmp(extension.extensionName, deviceExtension) == 0)
			{
				hasExtension = true;
				break;
			}
		}
		if (!hasExtension)
			return false;
	}
	return true;
}

bool VulkanRenderer::checkValidationLayerSupport()
{
	// How many instance vLayers VK support, so that we can create the array
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// create a list of vkLayerProperties using count
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


	// check if all of the layers in validationLayers exist in the availableLayers list
	for (const char* layerName: validationLayers)
	{
		bool layerFound = false;

		for(const auto &layerProperties: availableLayers)
		{
			if(strcmp(layerName, layerProperties.layerName))
			{
				layerFound = true;
				break;
			}
			if(!layerFound)
			{
				return  false;
			}
		}
	}

	return true;
}

QueueFamiliesIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamiliesIndices indices;

	// enumerate, create vector, and populate
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());


	// Go through each queue family, and check if it has at least one type of required type of queue
	int i = 0;
	for (const auto &queueFamily : queueFamilyList)
	{
		// check if queueFamily has atleast one queue in the family (could have no queues)
		// Queue can be multiple types defined through bitfield. Need to bitwise AND with VK_QUEUE_* to check if has requried type.
		if (queueFamily.queueCount >= 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;	// if queue family is valid, then get index;
		}

		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentationSupport);
		// Check if queue is presentation type (can be both - graphics and presentation)
		if(queueFamily.queueCount > 0 && presentationSupport)
		{
			indices.presentationFamily = i;
		}


		// Check if queue family indices are in a valid state, stop searching if so	
		if (indices.isValid())
			break;
		
		i++;
	}
	return indices;
}

SwapchainDetails VulkanRenderer::getSwapchainDetails(VkPhysicalDevice device)
{
	SwapchainDetails swapchainDetails;

	// -- CAPABILITIES --
	// Get the surface capabilities for the given surface on the given pDevice
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &swapchainDetails.surfaceCapabilities);

	// -- FORMATS --
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

	// if formats returned, get list of formats
	if(formatCount != 0)
	{
		swapchainDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, swapchainDetails.formats.data());
	}

	//-- PRESENTATION MODES --
	uint32_t presentationCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentationCount, nullptr);

	// if presentation mode returned, get list of presentationModes
	if(presentationCount != 0)
	{
		swapchainDetails.presentationModes.resize(presentationCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentationCount, swapchainDetails.presentationModes.data());
	}
	

	return swapchainDetails;
}

void VulkanRenderer::destroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(m_instance, m_debugMessenger, pAllocator);
	}
}

// Clean up our code
void VulkanRenderer::cleanUp()
{
	// Destroy the surface
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

	// Destroy the logical device
	vkDestroyDevice(m_mainDevice.m_logicalDevice, nullptr);

	// Destroy the validationlayer debugger
	if(enableValidationLayers)
	{
		destroyDebugUtilsMessengerEXT(nullptr);
	}

	// Destroy the instance
	vkDestroyInstance(m_instance, nullptr);	// should be the last to be deleted!
}

VulkanRenderer::~VulkanRenderer()
{
}

