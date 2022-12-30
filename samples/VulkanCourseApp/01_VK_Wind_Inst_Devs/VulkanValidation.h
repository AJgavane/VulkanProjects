#pragma once

#include <vector>


#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif


// List of validation layers to use
// 
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};


// new static member function
static VKAPI_ATTR VkBool32 VKAPI_CALL m_debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}