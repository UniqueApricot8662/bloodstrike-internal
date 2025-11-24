#include "sdk.hpp"

namespace visuals
{
	float *ColorToArray( ImColor color )
	{
		return (float *)&color;
	}

	void DrawFilledRect( int x, int y, int w, int h, float color[3] )
	{
		ImGui::GetBackgroundDrawList( )->AddRectFilled( ImVec2( x, y ), ImVec2( x + w, y + h ), ImColor( color[0], color[1], color[2] ) );
	}

	void DrawCornerBox( int x, int y, int w, int h, int borderPx, float color[3] )
	{
		visuals::DrawFilledRect( x + borderPx, y, w / 3, borderPx, color );
		visuals::DrawFilledRect( x + w - w / 3 + borderPx, y, w / 3, borderPx, color );
		visuals::DrawFilledRect( x, y, borderPx, h / 3, color );
		visuals::DrawFilledRect( x, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color );
		visuals::DrawFilledRect( x + borderPx, y + h + borderPx, w / 3, borderPx, color );
		visuals::DrawFilledRect( x + w - w / 3 + borderPx, y + h + borderPx, w / 3, borderPx, color );
		visuals::DrawFilledRect( x + w + borderPx, y, borderPx, h / 3, color );
		visuals::DrawFilledRect( x + w + borderPx, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color );
	}

	void DrawLabel( std::string text, glm::vec2 pos, float color[3], bool outline )
	{
		pos.x -= ( ImGui::CalcTextSize( text.c_str( ) ).x / 2.f );

		if ( outline )
		{
			/* I know this can cause an impact on preformance, I will fix it later  */
			ImGui::GetBackgroundDrawList( )->AddText( ImVec2( pos.x, pos.y - 1 ), ImColor( 0.f, 0.f, 0.f, color[3] ), text.c_str( ) );
			ImGui::GetBackgroundDrawList( )->AddText( ImVec2( pos.x, pos.y + 1 ), ImColor( 0.f, 0.f, 0.f, color[3] ), text.c_str( ) );

			ImGui::GetBackgroundDrawList( )->AddText( ImVec2( pos.x - 1, pos.y ), ImColor( 0.f, 0.f, 0.f, color[3] ), text.c_str( ) );
			ImGui::GetBackgroundDrawList( )->AddText( ImVec2( pos.x + 1, pos.y ), ImColor( 0.f, 0.f, 0.f, color[3] ), text.c_str( ) );

			ImGui::GetBackgroundDrawList( )->AddText( ImVec2( pos.x + 1, pos.y + 1 ), ImColor( 0.f, 0.f, 0.f, color[3] ), text.c_str( ) );
			ImGui::GetBackgroundDrawList( )->AddText( ImVec2( pos.x - 1, pos.y + 1 ), ImColor( 0.f, 0.f, 0.f, color[3] ), text.c_str( ) );

			ImGui::GetBackgroundDrawList( )->AddText( ImVec2( pos.x + 1, pos.y - 1 ), ImColor( 0.f, 0.f, 0.f, color[3] ), text.c_str( ) );
			ImGui::GetBackgroundDrawList( )->AddText( ImVec2( pos.x - 1, pos.y - 1 ), ImColor( 0.f, 0.f, 0.f, color[3] ), text.c_str( ) );
		}

		ImGui::GetBackgroundDrawList( )->AddText( ImVec2( pos.x, pos.y ), ImColor( color[0], color[1], color[2], color[3] ), text.c_str( ) );
	}
}

namespace sdk
{
    void BoneConnection( glm::vec2 a, glm::vec2 b, ImColor color )
    {
        ImGui::GetForegroundDrawList( )->AddLine( ImVec2( a.x, a.y ), ImVec2( b.x, b.y ), color, 1.5f );
    }

    bool MessiahMatrixAdd( XMFLOAT3X4 bonemat, XMFLOAT3X4 pos, glm::vec3 &out ) {
        out.x = ( pos._11 * bonemat._32 ) + ( pos._14 * bonemat._33 ) + ( pos._23 * bonemat._34 ) + pos._32;
        out.y = ( pos._12 * bonemat._32 ) + ( pos._21 * bonemat._33 ) + ( pos._24 * bonemat._34 ) + pos._33;
        out.z = ( pos._13 * bonemat._32 ) + ( pos._22 * bonemat._33 ) + ( pos._31 * bonemat._34 ) + pos._34;
        return true;
    }

    glm::vec3 Affine( const glm::mat3x4 &child, const glm::mat3x4 &parent )
    {
        /*
        GLM and d3d11 math are so different I have zero idea why I have to do this, nobody can help so this is the best I could come up with :tired:
        */
        glm::vec3 out;
        XMFLOAT3X4 childXM, parentXM;
        childXM = *(XMFLOAT3X4 const *)&child;
        parentXM = *(XMFLOAT3X4 const *)&parent;
        MessiahMatrixAdd( childXM, parentXM, out );
        return out;
    }

    glm::vec3 Affine( uint64_t child, uint64_t parent )
    {
        /*
        GLM and d3d11 math are so different I have zero idea why I have to do this, nobody can help so this is the best I could come up with :tired:
        */
        glm::vec3 out;
        XMFLOAT3X4 childXM, parentXM;
        childXM = *(XMFLOAT3X4 const *)&child;
        parentXM = *(XMFLOAT3X4 const *)&parent;
        printf( "child %llX parent %llX\n", child, parent );
        MessiahMatrixAdd( childXM, parentXM, out );
        return out;
    }

    bool can_read( void *addr, size_t size )
    {
        MEMORY_BASIC_INFORMATION mbi;
        if ( VirtualQuery( addr, &mbi, sizeof( mbi ) ) == 0 )
            return false;

        if ( !( mbi.Protect & ( PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE ) ) )
            return false;

        if ( mbi.State != MEM_COMMIT )
            return false;

        return ( (char *)addr + size ) <= ( (char *)mbi.BaseAddress + mbi.RegionSize );
    }

    bool _can_read( void *addr, size_t size )
    {
        return (uint64_t)addr > base && (uint64_t)addr < base + 0xffffffff;
    }

    template <typename T>
    bool safe_read( uint64_t addr, T &val )
    {
        if ( !_can_read( (void *)addr, sizeof( T ) ) ) return false;
        val = *(T *)addr;
        return true;
    }

    template <typename T>
    bool safe_read_slow( uint64_t addr, T &val )
    {
        if ( !can_read( (void *)addr, sizeof( T ) ) ) return false;
        val = *(T *)addr;
        return true;
    }
    bool w2s( __int64 cam, const glm::vec3 &world, glm::vec2 &out )
    {
        float relX = world.x - *(float *)( cam + 124 );
        float relY = world.y - *(float *)( cam + 128 );
        float relZ = world.z - *(float *)( cam + 132 );

        float px = relX * *(float *)( cam + 772 )
            + relY * *(float *)( cam + 784 )
            + relZ * *(float *)( cam + 796 );

        float py = relX * *(float *)( cam + 776 )
            + relY * *(float *)( cam + 788 )
            + relZ * *(float *)( cam + 800 );

        float pzOrig = relX * *(float *)( cam + 780 )
            + relY * *(float *)( cam + 792 )
            + relZ * *(float *)( cam + 804 );

        if ( pzOrig >= -0.01f ) // behind camera, original w2s doesn't have this for some reason
            return false;

        float pz = -pzOrig;

        float fov = *(float *)( cam + 824 );
        float f = 1.0f / tanf( ( fov * 0.017453292f ) * 0.5f );

        float screenW = (float)*(uint16_t *)( cam + 752 );
        float screenH = (float)*(uint16_t *)( cam + 754 );

        float invZ = 1.0f / fmaxf( fabsf( pz ), 0.000001f );

        out.x = roundf( ( ( px * invZ ) * f * screenH + screenW ) * 0.5f * 10.0f ) * 0.1f;
        out.y = roundf( ( ( screenH - ( ( py * invZ ) * f * screenH ) ) * 0.5f ) * 10.0f ) * 0.1f;

        return true;
    }
    bool findPresent( )
    {
        HWND hwnd = CreateWindowA( "STATIC", "dummy", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, 0, 0, 0, 0 );

        DXGI_SWAP_CHAIN_DESC scDesc = {};
        scDesc.BufferCount = 1;
        scDesc.BufferDesc.Width = 1;
        scDesc.BufferDesc.Height = 1;
        scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.OutputWindow = hwnd;
        scDesc.SampleDesc.Count = 1;
        scDesc.Windowed = TRUE;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        ID3D11Device *pDevice = nullptr;
        ID3D11DeviceContext *pContext = nullptr;
        IDXGISwapChain *pSwap = nullptr;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &scDesc,
            &pSwap,
            &pDevice,
            nullptr,
            &pContext
        );

        if ( !SUCCEEDED( hr ) ) return false;
        if ( !pSwap ) return false;

        void **vtable = *(void ***)pSwap;

        aPresent = (uint64_t)vtable[8];
        aResizeBuffers = (uint64_t)vtable[13];

        // cleanup
        pSwap->Release( );
        pContext->Release( );
        pDevice->Release( );
        DestroyWindow( hwnd );

        return true;
    }

    void cleanup( HMODULE hModule )
    {
        if ( f ) fclose( f );
        printf( "[-] unhooking\n" );
        MH_DisableHook( reinterpret_cast<void *>( aIObjectInitalizer ) );
        MH_DisableHook( reinterpret_cast<void *>( aIObjectDeconstructor ) );
        MH_DisableHook( reinterpret_cast<void *>( aPresent ) );
        MH_DisableHook( reinterpret_cast<void *>( aResizeBuffers ) );
        MH_DisableHook( reinterpret_cast<void *>( aGetRawInputData ) );
        MH_Uninitialize( );
        printf( "[-] finished\n" );
        FreeConsole( );
        FreeLibraryAndExitThread( hModule, 0 );
    }
}