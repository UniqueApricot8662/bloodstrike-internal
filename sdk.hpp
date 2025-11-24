#pragma once
#include "pch.h"

namespace visuals
{
	float *ColorToArray( ImColor color );

	void DrawFilledRect( int x, int y, int w, int h, float color[3] );

	void DrawCornerBox( int x, int y, int w, int h, int borderPx, float color[3] );

	void DrawLabel( std::string text, glm::vec2 pos, float color[3], bool outline );
}

namespace sdk
{
	void BoneConnection( glm::vec2 a, glm::vec2 b, ImColor color = ImColor( 255, 0, 0, 255 ) );
	bool MessiahMatrixAdd( XMFLOAT3X4 bonemat, XMFLOAT3X4 pos, glm::vec3 &out );
	glm::vec3 Affine( const glm::mat3x4 &child, const glm::mat3x4 &parent );
	glm::vec3 Affine( uint64_t child, uint64_t parent );
	bool can_read( void *addr, size_t size = sizeof(PVOID) );
	bool _can_read( void *addr, size_t size );
	template <typename T>
	bool safe_read( uint64_t addr, T &val );
	template <typename T>
	bool safe_write( uint64_t addr, T val );
    bool w2s( __int64 cam, const glm::vec3 &world, glm::vec2 &out );
    bool findPresent( );
	void cleanup( HMODULE hModule );
}

namespace bloodstrike
{
    namespace renderer
    {
        constexpr uint64_t hwnd = 0x6DE9430; // updated 9.26

        static ID3D11Device *deviceInstance = nullptr;
        static ID3D11DeviceContext *contextInstance = nullptr;
        static ID3D11RenderTargetView *rtv = nullptr;
        static HWND hWindow;
        static bool hooked = false;
        static uint64_t camera = 0x0;
        static std::vector<uint64_t> all_cameras = {};
    }

    namespace funcs
    {
        constexpr uint64_t Messiah__IObject__Initalizer = 0x02CF160;       // updated 9.26
        constexpr uint64_t Messiah__IObject__Deconstructor = 0x2CF890;     // updated 9.26
        constexpr uint64_t Messiah_WorldToScreen = 0x940F60;               // updated 9.26
        constexpr uint64_t Messiah__GetBoneTransform = 0x0D2BEC0;          // updated 9.26

		constexpr uint64_t GetRawInputData = 0x3BE8FF8;					   // updated 9.26
    }

    namespace vftables
    {
        const uint64_t Messiah__IEntity = 0x3D80048;                // updated 9.26
        const uint64_t Messiah__ICamera = 0x3F9B1D0;                // updated 9.26
        const uint64_t Messiah__AnimationCore__Pose = 0x3FA2F18;    // updated 9.26
        const uint64_t Messiah__SkeletonComponent = 0x4103698;      // updated 9.26
        const uint64_t Messiah__Actor = 0x5001C20;                  // updated 9.26
        const uint64_t Messiah__ActorComponent = 0x4138AB0;         // updated 9.28
        const uint64_t Messiah__IArea = 0x3FBAA68;                  // updated 9.28
        const uint64_t Messiah__TachComponent = 0x4101FC8;          // updated 9.28
        const uint64_t Messiah__FontType = 0x3C189F0;               // updated 9.28
    }
}
