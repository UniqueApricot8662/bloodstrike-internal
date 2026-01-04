#include "hooks.h"

namespace hooks
{

    HRESULT hkResizeBuffers( IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags )
    {
        Beep( 1000, 100 );

        // just mark that we need to recreate on the next frame
        g_needsResize = true;
        g_newWidth = width;
        g_newHeight = height;

        return oResizeBuffers( swap, bufferCount, width, height, newFormat, flags );
    }

    void *hkIEntityConstructor( __int64 Block, __int64 a2, int a3 )
    {
		/*
        printf( "entity %llX\n", Block );
        void *stack[256];
        USHORT nFrames = RtlCaptureStackBackTrace( 0, 256, stack, NULL );

        for ( USHORT i = 0; i < nFrames; ++i )
        {
            uint64_t addr = (uint64_t)stack[i] - base;
			addr += 0x140000000;
            printf( "\t%llX frame %d: %p\n", Block,  i, addr );
        }
        */
		return oIEntityConstructor( Block, a2, a3 );
    }

    void *hkIObjectInitalizer( __int64 IObject, __int64 a2, int a3 )
    {
        if ( IObject && std::find( g_IEntity.begin( ), g_IEntity.end( ), IObject ) == g_IEntity.end( ) )  // && sdk::can_read( (void *)IObject, sizeof( void * ) )
        {
            g_IEntity.push_back( IObject );
        }

        return oIObjectInitalizer( IObject, a2, a3 );
    }

    void *hkIObjectDeconstructor( __int64 *Block )
    {
        if ( Block )
        {
            if ( *Block == bloodstrike::renderer::camera ) bloodstrike::renderer::camera = 0x0;
            if ( std::find( bloodstrike::renderer::all_cameras.begin( ), bloodstrike::renderer::all_cameras.end( ), *Block ) == bloodstrike::renderer::all_cameras.end( ) )
            {
                bloodstrike::renderer::all_cameras.erase( std::remove( bloodstrike::renderer::all_cameras.begin( ), bloodstrike::renderer::all_cameras.end( ), *Block ), bloodstrike::renderer::all_cameras.end( ) );
            }

            g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), *Block ), g_IEntity.end( ) );
        }

        return oIObjectDeconstructor( Block );
    }

    UINT WINAPI hkGetRawInputData( HRAWINPUT hRaw, UINT uiCmd, LPVOID pData, PUINT pcbSize, UINT cbHeader ) {
        UINT ret = oGetRawInputData( hRaw, uiCmd, pData, pcbSize, cbHeader );
        if ( !should_change_mouse ) return ret;

        if ( pData ) {
            RAWINPUT *ri = (RAWINPUT *)pData;
            if ( ri->header.dwType == RIM_TYPEMOUSE ) {
                ri->data.mouse.lLastX = dx;
                ri->data.mouse.lLastY = dy;
            }
        }
        return ret;
    }

    /*
    HRESULT hkResizeBuffers( IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags )
    {
        if ( bloodstrike::renderer::rtv ) {
            bloodstrike::renderer::rtv->Release( );
            bloodstrike::renderer::rtv = nullptr;
        }

        HRESULT hr = oResizeBuffers( swap, bufferCount, width, height, newFormat, flags );
        if ( FAILED( hr ) ) return hr;

        ID3D11Texture2D *backBuffer = nullptr;
        swap->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void **)&backBuffer );
        if ( backBuffer ) {
            bloodstrike::renderer::deviceInstance->CreateRenderTargetView( backBuffer, nullptr, &bloodstrike::renderer::rtv );
            backBuffer->Release( );
        }

        return hr;
    }
    */
}