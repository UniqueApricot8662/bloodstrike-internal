#ifndef PCH_H
#define PCH_H

#define GLM_ENABLE_EXPERIMENTAL

#include "config.hpp"
#include <stdio.h>
#include "MinHook.h"
#include "kingkong.h"

typedef unsigned long long uint64_t;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "ntdll.lib")
#include <d3d11.h>
#include <dxgi.h>

struct XMFLOAT3X4 {
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
};

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>
#include <emmintrin.h>
#include <intrin.h>
#include <cmath>
#include <cstdint>
#include <winternl.h>
#include <chrono>
#include <thread>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

inline std::vector<uint64_t> g_IEntity = {};

#include "hooks.h"
#include "sdk.hpp"
#endif //PCH_H