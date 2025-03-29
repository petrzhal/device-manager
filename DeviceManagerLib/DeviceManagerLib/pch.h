#ifndef PCH_H
#define PCH_H

#pragma warning(disable: 4251)

// winapi
#ifdef _WIN32
#include <windows.h>
#include <newdev.h>
#include <new>
#include <setupapi.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <cfgmgr32.h>
#include <pdh.h>
#include <pdhmsg.h>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "newdev.lib")
#pragma comment(lib, "cfgmgr32.lib")
#pragma comment(lib, "pdh.lib")
#endif

// stl
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cstdio>
#include <functional>
#include <sstream>
#include <set>

#endif // PCH_H
