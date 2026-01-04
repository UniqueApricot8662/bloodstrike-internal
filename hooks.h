#pragma once
#include "pch.h"
#include "sdk.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
inline FILE *f;
inline uint64_t base;

typedef __int64 *( *tIEntityConstructor )( __int64 Block, __int64 a2, int a3 );
inline tIEntityConstructor oIEntityConstructor = nullptr;
inline uint64_t aIEntityConstructor;

typedef void *( *tIObjectInitalizer )( __int64 IObject, __int64 a2, int  a3 );
inline tIObjectInitalizer oIObjectInitalizer = nullptr;
inline uint64_t aIObjectInitalizer;

typedef void *( *tIObjectDeconstructor )( __int64 *Block );
inline tIObjectDeconstructor oIObjectDeconstructor = nullptr;
inline uint64_t aIObjectDeconstructor;

typedef HRESULT( *tPresent )( IDXGISwapChain *pSwapChain, UINT, UINT );
inline tPresent oPresent = nullptr;
inline uint64_t aPresent;

typedef double *( *tProject )( __int64 thisptr, double *out, __int64 worldpos );
inline tProject oProject = nullptr;
inline uint64_t aProject;

typedef void ( *tSetBonePos ) ( __int64 SkeletonComponent, void *NewBonePos );
inline tSetBonePos oSetBonePos = nullptr;
inline uint64_t aSetBonePos;

typedef UINT( *tGetRawInputData ) ( HRAWINPUT, UINT, LPVOID, PUINT, UINT );
inline tGetRawInputData oGetRawInputData;
inline uint64_t aGetRawInputData;

typedef HRESULT( *tResizeBuffers )( IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags );
inline tResizeBuffers oResizeBuffers;
inline uint64_t aResizeBuffers;

typedef __int64( *tGetBoneTransform )( __int64 a1, __int64 a2, __int64 a3 );
inline tGetBoneTransform oGetBoneTransform = nullptr;
inline uint64_t aGetBoneTransform;

namespace hooks
{
    inline bool g_needsResize = false;
    inline UINT g_newWidth = 0, g_newHeight = 0;

    void *hkIObjectInitalizer( __int64 IObject, __int64 a2, int a3 );

    void *hkIObjectDeconstructor( __int64 *Block );


    void *hkIEntityConstructor( __int64 Block, __int64 a2, int a3 );

    UINT WINAPI hkGetRawInputData( HRAWINPUT hRaw, UINT uiCmd, LPVOID pData, PUINT pcbSize, UINT cbHeader );

    HRESULT hkResizeBuffers( IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags );
}