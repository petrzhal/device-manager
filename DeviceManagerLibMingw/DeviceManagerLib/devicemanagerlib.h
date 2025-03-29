#pragma once

#pragma warning(disable: 4251)

// winapi
#include <windows.h>
#include <newdev.h>
#include <setupapi.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <cfgmgr32.h>
#include <pdh.h>
#include <pdhmsg.h>

// stl
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cstdio>
#include <functional>
#include <sstream>
#include <set>

// DeviceManagerLib
#include "DeviceManagerLibGlobal.h"
#include "DeviceEnumerator.h"
#include "DeviceController.h"
#include "DeviceDiagnostic.h"
#include "PerformanceMonitor.h"
