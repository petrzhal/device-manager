#ifndef DEVICE_MANAGER_LIB_GLOBAL_H
#define DEVICE_MANAGER_LIB_GLOBAL_H

#ifdef _WIN32
	#ifdef DEVICE_MANAGER_LIB_LIBRARY
		#define DEVICE_MANAGER_LIB_EXPORT __declspec(dllexport)
	#else
		#define DEVICE_MANAGER_LIB_EXPORT __declspec(dllimport)
	#endif
#else
	#define DEVICE_MANAGER_LIB_EXPORT
#endif

#endif // DEVICE_MANAGER_LIB_GLOBAL_H
