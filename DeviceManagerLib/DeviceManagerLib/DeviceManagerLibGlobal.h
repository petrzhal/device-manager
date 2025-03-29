#pragma once

#ifdef DEVICE_MANAGER_LIB_LIBRARY
	#define DEVICE_MANAGER_LIB_EXPORT __declspec(dllexport)
#else
	#define DEVICE_MANAGER_LIB_EXPORT __declspec(dllimport)
#endif