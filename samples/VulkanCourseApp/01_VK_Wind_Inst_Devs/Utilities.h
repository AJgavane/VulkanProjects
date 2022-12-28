#pragma once

// Indices (location) of queue families if they exists at all
struct QueueFamiliesIndices
{
	int graphicsFamily = -1;		// location of graphics queue families

	// Check if queue families are valid
	bool isValid()
	{
		return graphicsFamily >= 0;
	}
};