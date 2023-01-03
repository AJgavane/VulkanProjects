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
		createSwapchain();
		createGraphicsPipeline();
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
	VkInstanceCreateInfo instanceCreateInfo = {  };

	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	//createInfo.pNext = // pointed to abs anything, extended information
	//createInfo.flags = VK_WHATEVER | VK_ANYTHING; // seting bit fields
	instanceCreateInfo.pApplicationInfo = &appInfo;	//VK application info


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


	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// validation layer
	// TODO:setup validation layer that instance will use.
	// TODO: Updated validation layer!!
	if(enableValidationLayers)
	{
		std::cout << "Validation Layer: ON!" << std::endl;
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		std::cout << "Validation Layer: OFF!" << std::endl;
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = nullptr;
	}


	// Create instance
	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);

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

void VulkanRenderer::createSwapchain()
{
	// get swapchain details to retrieve the best settings
	SwapchainDetails swapchainDetails = getSwapchainDetails(m_mainDevice.m_physicalDevice);

	// Find optimal surface value for our swapchain
		// 1. CHOOSE BEST SURFACE FORMAT
	VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapchainDetails.formats);
		// 2. CHOOSE BEST PRESENTATION MODE
	VkPresentModeKHR presentMode = chooseBestPresentationMode(swapchainDetails.presentationModes);
		// 3. CHOOSE SWAPCHAIN IMAGE RESOLUTION
	VkExtent2D extent = choseSwapExtent(swapchainDetails.surfaceCapabilities);

	// ** how many images are in the swap chain? Get 1 more than min to allow triple buffering
	uint32_t imageCount = swapchainDetails.surfaceCapabilities.minImageCount + 1;

	// clamping the image count.
	// if maxImageCount == 0 ==> no limit
	if (swapchainDetails.surfaceCapabilities.maxImageCount > 0 &&
		swapchainDetails.surfaceCapabilities.maxImageCount < imageCount)
	{
		imageCount = swapchainDetails.surfaceCapabilities.maxImageCount;
	}


	// Creation info for swap chain
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType			 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;	
	swapchainCreateInfo.surface			 = m_surface;												// Swapchain surface
	swapchainCreateInfo.imageFormat		 = surfaceFormat.format;									// Swapchain format
	swapchainCreateInfo.imageColorSpace  = surfaceFormat.colorSpace;								// Swapchain colorspace
	swapchainCreateInfo.presentMode		 = presentMode;												// Swapchain presentation mode
	swapchainCreateInfo.imageExtent		 = extent;													// Swapchain image extent
	swapchainCreateInfo.minImageCount	 = imageCount;	//**										// Min images in swapchain
	swapchainCreateInfo.imageArrayLayers = 1;														// Number of layers for each image in chain
	swapchainCreateInfo.imageUsage		 = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;						// What attachments images will be used as
	swapchainCreateInfo.preTransform	 = swapchainDetails.surfaceCapabilities.currentTransform;	// T/f to perform on swapchain images
	swapchainCreateInfo.compositeAlpha	 = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;						// How to handle blending images with external graphics (e.g. other windows)
	swapchainCreateInfo.clipped			 = VK_TRUE ;												// Whether to clip parts of the image not in the view (e.g. behind another window, off screen, etc)

	// Note about queues:
	//		Graphics_Queue: draws :: Presentation_Queue: puts it on the display/surface

	// Get queue family indices
	QueueFamiliesIndices indices = getQueueFamilies(m_mainDevice.m_physicalDevice);

	// If graphics and presentation families are diff, then swapchain must be shared between families
	if(indices.graphicsFamily != indices.presentationFamily)
	{
		// Queues to share between
		uint32_t queueFamilyIndices[] = {
			(uint32_t)indices.graphicsFamily,
			(uint32_t)indices.presentationFamily
		};

		swapchainCreateInfo.imageSharingMode		= VK_SHARING_MODE_CONCURRENT;	// Image share handling
		swapchainCreateInfo.queueFamilyIndexCount	= 2;							// Number of queues to share images between
		swapchainCreateInfo.pQueueFamilyIndices		= queueFamilyIndices;			// Arrays of queues to share between
	}
	else
	{
		swapchainCreateInfo.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount	= 0;
		swapchainCreateInfo.pQueueFamilyIndices		= nullptr;
	}

	// If old swap chain been destroyed, and this one replaces it, then
	// link old one to quickly handover responsibilities
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	// Create Swapchain
 	VkResult result = vkCreateSwapchainKHR(m_mainDevice.m_logicalDevice, &swapchainCreateInfo, nullptr, &m_swapchain);

	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create SWAPCHAIN!!");
	}

	// created member vars for format and extent because
	// I will be reusing them to create IMAGE_VIEW which is interface to the image we created
	// e.g.: LogicalDevice : PhysicalDevice :: ImageView : Image
	m_swapchainImageFormat = surfaceFormat.format;
	m_swapchainExtent = extent;

	// Get swapchain images (first count and then populate)
	uint32_t swapchainImageCount;
	vkGetSwapchainImagesKHR(m_mainDevice.m_logicalDevice, m_swapchain, &swapchainImageCount, nullptr);

	std::vector<VkImage> images(swapchainImageCount);
	vkGetSwapchainImagesKHR(m_mainDevice.m_logicalDevice, m_swapchain, &swapchainImageCount, images.data());

	// iterate and store in the array we created : m_swapchainImages
	for(VkImage image : images)
	{
		// store image handle
		SwapchainImage swapchainImage = { };
		swapchainImage.image = image;

		// CREATE IMAGE VIEW HERE -- Created function
		swapchainImage.imageView = createImageView(image, m_swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		// Add to our swapchainImages member variable list 
		m_swapchainImages.push_back(swapchainImage);
	}
}

void VulkanRenderer::createGraphicsPipeline()
{
	// read in SPIR-V code for shader
	auto vertexShaderCode		= readFile("./Shaders/vert.spv");
	auto fragmentShaderCode	= readFile("./Shaders/frag.spv");

	// Build a Shader Module to link to Graphics Pipeline
	// once we create graphics pipeline, we don't need these modules
	// -- and we have to destroy the modules

	// CREATE SHADER MODULE
	VkShaderModule vertexShaderModule	= createShaderModule(vertexShaderCode);
	VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

	/** -- SHADER STAGE CREATION INFORMATION -- **/
	// Vertex Stage creation information
	VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
	vertexShaderCreateInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderCreateInfo.stage	= VK_SHADER_STAGE_VERTEX_BIT;							// Shader stage name
	vertexShaderCreateInfo.module	= vertexShaderModule;									// shader module to be used by the stage
	vertexShaderCreateInfo.pName	= "main";												// entry point into shader

	// Fragment Stage creation information
	VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
	fragmentShaderCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderCreateInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;							// Shader stage name
	fragmentShaderCreateInfo.module = fragmentShaderModule;									// shader module to be used by the stage
	fragmentShaderCreateInfo.pName  = "main";												// entry point into shader

	// Put shader stage creation info into an array
	// Graphics pipeline input requires array of of shader stage create info
	VkPipelineShaderStageCreateInfo shaderStagesCreateInfos[] = { vertexShaderCreateInfo, fragmentShaderCreateInfo };


	// CREATE PIPELINE
	/** -- VERTEX INPUT (ToDo: Put vertex description when rss created) -- **/
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;		// List of vertex binding description (data spacing, stride info, etc)
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;	// List of vertex attribute description (data format and where to find to/from)


	/** -- INPUT ASSEMBLY -- **/
	VkPipelineInputAssemblyStateCreateInfo inputAssembleCreateInfo = {};
	inputAssembleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembleCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;		// Primitive Type to assemble vertices as
	inputAssembleCreateInfo.primitiveRestartEnable = VK_FALSE;					// allow overriding of strip topology to start new primitive


	/** -- VIEWPORT AND SCISSOR-- **/
	// Create a viewport info struct
	VkViewport viewport = {};
	viewport.x = 0.0f;													// x start coord
	viewport.y = 0.0f;													// y start coord
	viewport.width = static_cast<float>(m_swapchainExtent.width);		// viewport width
	viewport.height = static_cast<float>(m_swapchainExtent.height);		// viewport height
	viewport.minDepth = 0.0f;											// min viewport depth
	viewport.maxDepth = 1.0f;											// max viewport depth

	// Create a Scissor info struct
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };									// Offset to use region from
	scissor.extent = m_swapchainExtent;									// Extent to describe region to use, starting at offset
			// -> scissor giving visible area

	// create viewport state
	VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.scissorCount = 1;
	viewportCreateInfo.pScissors = &scissor;


	/** -- DYNAMIC STATES -- **/
	// Dynamic states to ENABLE
	/*std::vector<VkDynamicState> dynamicStateEnables;
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);	// Dynamic viewport: we can resize in cmd buffer w/ vkCmdSetViewport(cmdbuffer, whichViewport=0 , howmanyToSet=1, reftoViewportType=viewport) 
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);	// Dynamic scissor: we can resize in cmd buffer w/ vkCmdSetScissor(cmdbuffer, whichScissor=0 , howmanyToSet=1, reftoScissorType=scissor)


	// Dynamic state creation info
	VkPipelineDynamicStateCreateInfo dynStateCreateInfo = {};
	dynStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	dynStateCreateInfo.pDynamicStates = dynamicStateEnables.data();//*/


	/** -- RASTERIZER -- **/
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.depthBiasClamp = VK_FALSE;				// Change if frag beyond near/far plane are clipped (default) or clamped to plane
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;	// Whether to skip rasterizer. Never creates fragments, only suitable for pipeline w/o a FB output
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;	// How to handle filling frags between vertices. e.g. Render only vertex, edges or fill the triangle (our curr primitive)
	rasterizerCreateInfo.lineWidth = 1.0f;						// How thick a line should be when drawn, needs a gpu extension if value other than 1.0f
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;		// Which side of a tri to cull. Cull back facing polygon
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;	// Winding to determine which side is front
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;			// Whether to add Depthbias to fragments (good for handling Shadow Acne). DepthBias can be defined as a part of pipeline.


	/** -- MULTISAMPLING (for anti-aliasing) -- **/
	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {};
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;				// Enable multisampling shading or not. r.n we are disabling it
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	// Num of samples to use per fragment


	/** -- BLENDING -- **/
	// Blending decides how to blend a new color being written to a fragment with the old value

	// Blend Attachment state (how blending is handled)
	VkPipelineColorBlendAttachmentState colorStateAttachments = {};
	colorStateAttachments.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
							| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;		// colors to apply blending to
	colorStateAttachments.blendEnable = VK_TRUE;													// enable blending

	// Blending uses following equation: (srcColorBlendFactor * new_color) colorBlendOp (dstColorBlendFactor * old_color);
	colorStateAttachments.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorStateAttachments.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorStateAttachments.colorBlendOp		   = VK_BLEND_OP_ADD;
	// Summarize: (VK_BLEND_FACTOR_SRC_ALPHA * new_color) + (VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA * old_color)
	//				  ===   (new_color_alpha * new_color) + ((1 - new_color_alpha) * old_color)

	colorStateAttachments.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorStateAttachments.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorStateAttachments.alphaBlendOp = VK_BLEND_OP_ADD;
	// Summarize: (1 * new_alpha)  + (0 * old_alpha) === new_alpha


	VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo = {};
	colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingCreateInfo.logicOpEnable = VK_FALSE;			// Alternative to calc is to use logical ops
	colorBlendingCreateInfo.attachmentCount = 1;
	colorBlendingCreateInfo.pAttachments = &colorStateAttachments;


	/** -- PIPELINE LAYOUT (ToDo: Apply Future Descriptor Set Layouts) -- **/
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pSetLayouts = nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	// Create pipeline layout
	VkResult result = vkCreatePipelineLayout(m_mainDevice.m_logicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create PIPELINE_LAYOUT");
	}

	
	/** -- DEPTH STENSIL TESETING -- **/
	// ToDo: Set up depth stensil testing: We need to learn about images, buffers, and device memory.




	// DESTROY shader modules, no longer needed after pipeline
	vkDestroyShaderModule(m_mainDevice.m_logicalDevice, fragmentShaderModule, nullptr);
	vkDestroyShaderModule(m_mainDevice.m_logicalDevice, vertexShaderModule, nullptr);
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
	QueueFamiliesIndices qFamIndices;

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
			qFamIndices.graphicsFamily = i;	// if queue family is valid, then get index;
		}

		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentationSupport);
		// Check if queue is presentation type (can be both - graphics and presentation)
		if(queueFamily.queueCount > 0 && presentationSupport)
		{
			qFamIndices.presentationFamily = i;
		}


		// Check if queue family indices are in a valid state, stop searching if so	
		if (qFamIndices.isValid())
			break;
		
		i++;
	}
	return qFamIndices;
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
	// Destroy pipeline layout
	vkDestroyPipelineLayout(m_mainDevice.m_logicalDevice, m_pipelineLayout, nullptr);

	// Because we have created IMAGE_VIEWS, we need to destroy them as well
	for(auto image: m_swapchainImages)
	{
		vkDestroyImageView(m_mainDevice.m_logicalDevice, image.imageView, nullptr);
	}

	// Destroy swapchain
	vkDestroySwapchainKHR(m_mainDevice.m_logicalDevice, m_swapchain, nullptr);

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


// Best format is subjective, but I'll be using:
// FORMAT		: VK_FORMAT_R8G8B8A8_UNORM			(VK_FORMAT_B8G8R8A8_UNORM as a backup)
// colorSpace	: VK_COLORSPACE_SRGB_NONLINEAR_KHR 
VkSurfaceFormatKHR VulkanRenderer::chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
	// If only 1 format avail and its undefined, then
	//		it means all formats are available (no restrictions.
	if(formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_R8G8B8A8_UNORM , VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	// If restricted,
	// search for optimal format
	for(const auto &format : formats)
	{
		if((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM) 
			&& format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	// If not found, then just returned the first format.
	return formats[0];
}


// I'll be using: VK_PRESENT_MODE_MAILBOX_KHR
VkPresentModeKHR VulkanRenderer::chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes)
{
	// Look for Mailbox presentation mode
	for(const auto &presentationMode : presentationModes)
	{
		if(presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return  presentationMode;
		}
	}

	// if not found, use FIFO -- as a part of Vulkan specs this will always be available.
	return VK_PRESENT_MODE_FIFO_KHR;	
}

VkExtent2D VulkanRenderer::choseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
	// If current extent is at numeric limit, then extent can vary.
	// Otherwise, it is the size of the window
	std::numeric_limits<uint32_t>::max();
	if(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
	{
		return surfaceCapabilities.currentExtent;
	}
	else
	{
		// If value can vary, need to set manually

		// Get Window Size
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);

		// create new extent using window size
		VkExtent2D newExtent = {};
		newExtent.width = static_cast<uint32_t>(width);
		newExtent.height = static_cast<uint32_t>(height);

		// Surface also defines max and min window size, so we need to clamp our values
		newExtent.width  = std::max(surfaceCapabilities.minImageExtent.width, 
								std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
		newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, 
								std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));

		return newExtent;
	}
}

VkImageView VulkanRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectflags)
{
	VkImageViewCreateInfo viewCreateInfo = {};

	viewCreateInfo.sType		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image		= image;										// Image to creat View for
	viewCreateInfo.viewType		= VK_IMAGE_VIEW_TYPE_2D;						// Type of image (1D, 2D, Cubemap, etc)
	viewCreateInfo.format		= format;										// Format of image data
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;				// Allows remapping of RGBA components to other RGBA values
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	// Subresources allow a View to view only a part of the image
	viewCreateInfo.subresourceRange.aspectMask		= aspectflags;				// Which aspect of image to view : COLOR_BIT to view color, DEPTH_BIT to view depth, etc
	viewCreateInfo.subresourceRange.baseMipLevel	= 0;						// Base mipmap level to view from
	viewCreateInfo.subresourceRange.levelCount		= 1;						// Number of mipmap levels to view
	viewCreateInfo.subresourceRange.baseArrayLayer	= 0;						// Start array level to view from
	viewCreateInfo.subresourceRange.layerCount		= 1;						// Number of array layers to view

	// Create Image view and return it
	VkImageView imageView;
	VkResult result = vkCreateImageView(m_mainDevice.m_logicalDevice, &viewCreateInfo, nullptr, &imageView);

	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("FAILED to create IMAGE_VIEW");
	}

	return imageView;
}

VkShaderModule VulkanRenderer::createShaderModule(const std::vector<char>& code)
{
	// Shader module creation information
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {};

	shaderModuleCreateInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.codeSize = code.size();										// Size of code
	shaderModuleCreateInfo.pCode	= reinterpret_cast<const uint32_t*>(code.data());	// pointer to code (of type uint32_t pointer type)

	VkShaderModule shaderModule;

	VkResult result = vkCreateShaderModule(m_mainDevice.m_logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);

	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Shader Module!");
	}

	return shaderModule;
}
