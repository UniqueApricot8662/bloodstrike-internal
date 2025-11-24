#include "pch.h"
using namespace sdk;
using namespace hooks;

HRESULT hkPresent( IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags )
{
    if ( !bloodstrike::renderer::hooked )
    {
        if ( FAILED( pSwapChain->GetDevice( __uuidof( ID3D11Device ), (void **)&bloodstrike::renderer::deviceInstance ) ) )
            return oPresent( pSwapChain, SyncInterval, Flags );

        bloodstrike::renderer::deviceInstance->GetImmediateContext( &bloodstrike::renderer::contextInstance );

        ID3D11Texture2D *backBuffer = nullptr;
        pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void **)&backBuffer );

        if ( !backBuffer ) return oPresent( pSwapChain, SyncInterval, Flags );

        bloodstrike::renderer::deviceInstance->CreateRenderTargetView( backBuffer, nullptr, &bloodstrike::renderer::rtv );
        backBuffer->Release( );

        IMGUI_CHECKVERSION( );
        ImGui::CreateContext( );
        ImGuiIO &io = ImGui::GetIO( ); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark( );

        ImGuiStyle &style = ImGui::GetStyle( );
        ImVec4 *colors = style.Colors;

        // base colors remain, swap blues for pinks
        colors[ImGuiCol_Text] = ImVec4( 1.00f, 0.98f, 0.98f, 1.00f );
        colors[ImGuiCol_TextDisabled] = ImVec4( 0.50f, 0.45f, 0.50f, 1.00f );

        // window bg
        colors[ImGuiCol_WindowBg] = ImVec4( 0.13f, 0.12f, 0.14f, 1.00f );
        colors[ImGuiCol_ChildBg] = ImVec4( 0.16f, 0.14f, 0.18f, 1.00f );
        colors[ImGuiCol_PopupBg] = ImVec4( 0.18f, 0.16f, 0.20f, 1.00f );

        // borders
        colors[ImGuiCol_Border] = ImVec4( 0.50f, 0.35f, 0.45f, 0.50f ); // accent
        colors[ImGuiCol_BorderShadow] = ImVec4( 0.10f, 0.08f, 0.12f, 0.50f );

        // frames
        colors[ImGuiCol_FrameBg] = ImVec4( 0.20f, 0.15f, 0.22f, 1.00f );
        colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.85f, 0.45f, 0.70f, 0.40f ); // pink hover
        colors[ImGuiCol_FrameBgActive] = ImVec4( 0.95f, 0.35f, 0.65f, 0.70f ); // stronger accent

        // title bars
        colors[ImGuiCol_TitleBg] = ImVec4( 0.18f, 0.12f, 0.20f, 1.00f );
        colors[ImGuiCol_TitleBgActive] = ImVec4( 0.90f, 0.40f, 0.70f, 1.00f ); // pink active
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.15f, 0.10f, 0.18f, 0.90f );

        // scrollbars
        colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.12f, 0.10f, 0.14f, 1.00f );
        colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.85f, 0.40f, 0.70f, 0.60f );
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.95f, 0.35f, 0.65f, 0.80f );
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.95f, 0.25f, 0.60f, 1.00f );

        // checkmarks / sliders
        colors[ImGuiCol_CheckMark] = ImVec4( 0.95f, 0.35f, 0.65f, 1.00f );
        colors[ImGuiCol_SliderGrab] = ImVec4( 0.85f, 0.45f, 0.70f, 0.70f );
        colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.95f, 0.35f, 0.65f, 1.00f );

        // buttons
        colors[ImGuiCol_Button] = ImVec4( 0.75f, 0.30f, 0.65f, 0.60f );
        colors[ImGuiCol_ButtonHovered] = ImVec4( 0.95f, 0.35f, 0.65f, 0.85f );
        colors[ImGuiCol_ButtonActive] = ImVec4( 1.00f, 0.25f, 0.60f, 1.00f );

        // tabs
        colors[ImGuiCol_Tab] = ImVec4( 0.18f, 0.14f, 0.20f, 0.86f );
        colors[ImGuiCol_TabHovered] = ImVec4( 0.90f, 0.35f, 0.70f, 0.80f );
        colors[ImGuiCol_TabActive] = ImVec4( 0.95f, 0.30f, 0.65f, 1.00f );
        colors[ImGuiCol_TabUnfocused] = ImVec4( 0.15f, 0.10f, 0.18f, 0.90f );
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.85f, 0.25f, 0.60f, 0.80f );

        // title accents
        colors[ImGuiCol_Header] = ImVec4( 0.85f, 0.40f, 0.70f, 0.45f );
        colors[ImGuiCol_HeaderHovered] = ImVec4( 0.95f, 0.35f, 0.65f, 0.70f );
        colors[ImGuiCol_HeaderActive] = ImVec4( 1.00f, 0.25f, 0.60f, 1.00f );

        // separators
        colors[ImGuiCol_Separator] = ImVec4( 0.50f, 0.35f, 0.45f, 0.50f );
        colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.95f, 0.35f, 0.65f, 0.70f );
        colors[ImGuiCol_SeparatorActive] = ImVec4( 1.00f, 0.25f, 0.60f, 1.00f );

        // resize grips
        colors[ImGuiCol_ResizeGrip] = ImVec4( 0.85f, 0.35f, 0.65f, 0.20f );
        colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.95f, 0.35f, 0.65f, 0.70f );
        colors[ImGuiCol_ResizeGripActive] = ImVec4( 1.00f, 0.25f, 0.60f, 1.00f );

        bloodstrike::renderer::hWindow = *(HWND *)( base + bloodstrike::renderer::hwnd );

        ImGui_ImplWin32_Init( bloodstrike::renderer::hWindow );
        ImGui_ImplDX11_Init( bloodstrike::renderer::deviceInstance, bloodstrike::renderer::contextInstance );

        bloodstrike::renderer::hooked = true;
        return oPresent( pSwapChain, SyncInterval, Flags );
    }


    ImGuiStyle &style = ImGui::GetStyle( );
    ImVec4 *colors = style.Colors;

    auto &c = colors[ImGuiCol_TitleBg];

    ImVec4 target = colorBack
        ? ImVec4( 0.90f, 0.40f, 0.70f, 1.00f )
        : ImVec4( 0.18f, 0.12f, 0.20f, 1.00f );

    c = ImLerp( c, target, anim_speed );

    if ( fabsf( c.x - target.x ) < 0.001f )
        colorBack = !colorBack;


    DXGI_SWAP_CHAIN_DESC desc;
    pSwapChain->GetDesc( &desc );

    ImGuiIO &io = ImGui::GetIO( );
    io.MouseDown[0] = ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) != 0;
    io.WantCaptureMouse = true;

    ImGui_ImplDX11_NewFrame( );
    ImGui_ImplWin32_NewFrame( );
    ImGui::NewFrame( );

    if ( bloodstrike::renderer::camera != 0x0 )
    {
        ImVec2 ds = io.DisplaySize;
        ds.x /= 2.f;
        ds.y /= 2.f;

        glm::vec2 sc = { ds.x, ds.y };
        bool canProject = *(int *)( bloodstrike::renderer::camera + 384 ) != 0x7FFFFFFF || *(int *)( bloodstrike::renderer::camera + 388 ) != 0x7FFFFFFF;
        if ( canProject && *(int *)( bloodstrike::renderer::camera + 392 ) != 0x7FFFFFFF )
        {
            float closest_dst = FLT_MAX;
            glm::vec2 target_pos{ -1.f, -1.f };

            glm::mat4x3 local_trans = *(glm::mat4x3 *)( bloodstrike::renderer::camera + 0x58 );
            glm::vec3 local_pos = local_trans[3];
            for ( auto addr : g_IEntity )
            {
                if ( !addr )
                {
                    g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), addr ), g_IEntity.end( ) );
                    continue;
                }

                uint64_t vtable = *(uint64_t *)( addr );

                if ( vtable == base + bloodstrike::vftables::Messiah__IEntity )
                {
                    uint64_t expiredObj = *(uint64_t *)( addr + 0x18 );
                    if ( !expiredObj )
                    {
                        g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), addr ), g_IEntity.end( ) );
                        continue;
                    }


                    uint64_t SkelCache = 0;
                    uint64_t BoneData = 0;

                    if ( !can_read( (void *)( addr + 0x58 ) ) )
                    {
                        g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), addr ), g_IEntity.end( ) );
                        continue;
                    }

                    glm::mat4x3 trans = *(glm::mat4x3 *)( addr + 0x58 );
                    XMFLOAT3X4 dxTrans = *(XMFLOAT3X4 *)( addr + 0x58 );

                    glm::vec3 coords = trans[3];

                    float d = ( glm::distance( local_pos, coords ) );
                    float d2 = (int)d;

                    if ( fabsf( d - 1.506006f ) < 1e-6f ) continue;


                    uint64_t CachedObjects = *(uint64_t *)( addr + 0x40 );
                    if ( !CachedObjects )
                    {
                        g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), addr ), g_IEntity.end( ) );
                        continue;
                    }

                    uint64_t ActorComponent = *(uint64_t *)( CachedObjects + 0x10 );
                    if ( !ActorComponent )
                    {
                        g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), addr ), g_IEntity.end( ) );
                        continue;
                    }

                    if ( !can_read( (void *)ActorComponent, sizeof( void * ) ) ) continue;
                    uint64_t ac_vt = *(uint64_t *)( ActorComponent );

                    if ( ac_vt != base + bloodstrike::vftables::Messiah__ActorComponent )
                    {
                        g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), addr ), g_IEntity.end( ) );

                        continue;
                    }

                    uint64_t Actor = *(uint64_t *)( ActorComponent + 0xD0 );
                    if ( !Actor )
                    {
                        g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), addr ), g_IEntity.end( ) );
                        continue;
                    }

                    uint32_t PlayerType = *(uint32_t *)( Actor + 0x8 );
                    if ( PlayerType != 3 ) continue;

                    uint64_t Pose = *(uint64_t *)( Actor + 0x18 );
                    if ( !Pose )
                    {
                        g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), addr ), g_IEntity.end( ) );
                        continue;
                    }

                    if ( !can_read( (void *)Pose ) ) continue;

                    uint64_t BipedPose = *(uint64_t *)( Pose + 0x90 ); // sizeof = 0x118
                    if ( !BipedPose ) continue;

                    BipedPose += 0x8; // first bone ptr is garbage

                    glm::vec2 result;
                    if ( w2s( bloodstrike::renderer::camera, coords, result ) )
                    {
                        ImVec2 pos = { (float)result[0], (float)result[1] };
                        if ( pos.x > ImGui::GetIO( ).DisplaySize.x || pos.y > ImGui::GetIO( ).DisplaySize.y ) continue;
                        if ( pos.x < 1.0 || pos.y < 1.0 ) continue;
                        int dst_m = (int)( d2 / 5 );

                        if ( dst_m < 1000 )
                        {

                            bool visible = *(bool *)( addr + 0x128 );
                            ImColor color = visible ? ImColor( 100, 25, 25, 255 ) : ImColor( 175, 175, 175, 255 );

                            {
                                glm::vec2 neck, spine1, spine2, spine3, pelvis, buttCheekL, buttCheekR, kneeL, kneeR, footL, footR, sholL, elbowL, wristL, sholR, elbowR, wristR; //  no head in bipedPose too lazy to find it in the other array
                                glm::vec3 _neck, _spine1, _spine2, _spine3, _pelvis, _buttCheekL, _buttCheekR, _kneeL, _kneeR, _footL, _footR, _sholL, _elbowL, _wristL, _sholR, _elbowR, _wristR;

                                // thanks tantem for matrix offset from bonestart
                                uint64_t boneStart = *(uint64_t *)( ( 7 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _neck );
                                    w2s( bloodstrike::renderer::camera, _neck, neck );
                                }

                                boneStart = *(uint64_t *)( ( 6 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _spine1 );
                                    w2s( bloodstrike::renderer::camera, _spine1, spine1 );
                                }

                                boneStart = *(uint64_t *)( ( 5 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _spine2 );
                                    w2s( bloodstrike::renderer::camera, _spine2, spine2 );
                                }

                                boneStart = *(uint64_t *)( ( 4 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _spine3 );
                                    w2s( bloodstrike::renderer::camera, _spine3, spine3 );
                                }

                                boneStart = *(uint64_t *)( ( 3 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _pelvis );
                                    w2s( bloodstrike::renderer::camera, _pelvis, pelvis );
                                }

                                boneStart = *(uint64_t *)( ( 22 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _buttCheekL );
                                    w2s( bloodstrike::renderer::camera, _buttCheekL, buttCheekL );
                                }

                                boneStart = *(uint64_t *)( ( 19 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _buttCheekR );
                                    w2s( bloodstrike::renderer::camera, _buttCheekR, buttCheekR );
                                }

                                boneStart = *(uint64_t *)( ( 23 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _kneeL );
                                    w2s( bloodstrike::renderer::camera, _kneeL, kneeL );
                                }

                                boneStart = *(uint64_t *)( ( 19 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _kneeR );
                                    w2s( bloodstrike::renderer::camera, _kneeR, kneeR );
                                }

                                boneStart = *(uint64_t *)( ( 24 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _footL );
                                    w2s( bloodstrike::renderer::camera, _footL, footL );
                                }

                                boneStart = *(uint64_t *)( ( 20 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _footR );
                                    w2s( bloodstrike::renderer::camera, _footR, footR );
                                }

                                boneStart = *(uint64_t *)( ( 14 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _sholL );
                                    w2s( bloodstrike::renderer::camera, _sholL, sholL );
                                }

                                boneStart = *(uint64_t *)( ( 9 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _sholR );
                                    w2s( bloodstrike::renderer::camera, _sholR, sholR );
                                }

                                boneStart = *(uint64_t *)( ( 15 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _elbowL );
                                    w2s( bloodstrike::renderer::camera, _elbowL, elbowL );
                                }

                                boneStart = *(uint64_t *)( ( 10 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _elbowR );
                                    w2s( bloodstrike::renderer::camera, _elbowR, elbowR );
                                }

                                boneStart = *(uint64_t *)( ( 16 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _wristL );
                                    w2s( bloodstrike::renderer::camera, _wristL, wristL );
                                }

                                boneStart = *(uint64_t *)( ( 11 * 0x8 ) + BipedPose );
                                if ( boneStart ) {
                                    MessiahMatrixAdd( *(XMFLOAT3X4 *)( boneStart + 0x30 ), dxTrans, _wristR );
                                    w2s( bloodstrike::renderer::camera, _wristR, wristR );
                                }

                                BoneConnection( neck, spine1, color );
                                BoneConnection( spine1, spine2, color );
                                BoneConnection( spine2, spine3, color );
                                BoneConnection( spine3, pelvis, color );

                                BoneConnection( spine1, sholL, color );
                                BoneConnection( sholL, elbowL, color );
                                BoneConnection( elbowL, wristL, color );

                                BoneConnection( spine1, sholR, color );
                                BoneConnection( sholR, elbowR, color );
                                BoneConnection( elbowR, wristR, color );

                                BoneConnection( pelvis, buttCheekL, color );
                                BoneConnection( buttCheekL, kneeL, color );
                                BoneConnection( kneeL, footL, color );

                                BoneConnection( pelvis, buttCheekR, color );
                                BoneConnection( buttCheekR, kneeR, color );
                                BoneConnection( kneeR, footR, color );

                                std::string address_txt = std::format( "[{:x}]", addr );
                                std::string dist_txt = std::format( "{:d}m", dst_m );

                                float dist = glm::distance( sc, neck );
                                if ( dist < closest_dst )
                                {
                                    closest_dst = dist;
                                    if ( randomizeAim )
                                    {
										float r = static_cast <float> ( rand( ) ) / static_cast <float> ( RAND_MAX );
                                        if ( r < headChance )
                                        {
                                            target_pos = neck;
                                        }
                                        else
                                        {
                                            target_pos = spine3;
                                        }
                                    }
                                    else
                                    {
                                        target_pos = aimBody ? spine3 : neck;
                                    }
                                }

                                float height = abs( neck.y - result.y );
                                float width = height * 0.4;

                                visuals::DrawCornerBox(
                                    neck.x - width,
                                    neck.y,
                                    width * 1.8f,
                                    height,
                                    1,
                                    visuals::ColorToArray( ImColor( 255, 255, 255, 255 ) )
                                );

                                float z = height * 0.25;
                                z = std::clamp( z, 15.f, 40.f );

                                visuals::DrawLabel( dist_txt, glm::vec2( neck.x, neck.y - ( z ) ), visuals::ColorToArray( ImColor( 255, 255, 255, 255 ) ), true );
                                visuals::DrawLabel( address_txt, glm::vec2( result.x, result.y + ( z / 3 ) ), visuals::ColorToArray( ImColor( 255, 255, 255, 255 ) ), false );
                            }
                        }
                    }
                }
                else
                {
                    g_IEntity.erase( std::remove( g_IEntity.begin( ), g_IEntity.end( ), addr ), g_IEntity.end( ) );
                }
            }

            if ( closest_dst < fov && target_pos.x != -1.f && GetAsyncKeyState( VK_RBUTTON ) )
            {
                POINT target = { (LONG)target_pos.x, (LONG)target_pos.y };
                ClientToScreen( bloodstrike::renderer::hWindow, &target );

                POINT cur;
                GetCursorPos( &cur );

                dx = target.x - cur.x;
                dy = target.y - cur.y;

                dx *= sens;
                dy *= sens;


                SetCursorPos( (int)target_pos.x, (int)target_pos.y );

                uint64_t wlptrw = GetWindowLongPtrW( bloodstrike::renderer::hWindow, -21 );

                // Messiah::Win32ViewportClientWindow : Messiah::IViewportClientWindow : Messiah::InputDispatcherWin32 : Messiah::InputTranslatorWin32]
                // whcar_t* WindowTitle +0x288
                // BYTE MouseFlag +0x22C

                printf( "[debug]  wlptrw=%llx\n", wlptrw );

                should_change_mouse = true;
            }
            else
            {
                should_change_mouse = false;
                closest_dst = FLT_MAX;
            }
        }
        ImGui::GetForegroundDrawList( )->AddCircle( ds, fov, ImColor( 255, 255, 255, 255 ), 250.f );
    }
    else
    {
        ImGui::GetForegroundDrawList( )->AddText( ImVec2( 100, 100 ), ImColor( 255, 0, 0, 255 ), "looking for camera..." );
        for ( auto addr : g_IEntity )
        {
            if ( !addr ) continue;

            void **vtable = *(void ***)addr;
            if ( vtable && (uint64_t)vtable == base + bloodstrike::vftables::Messiah__ICamera )
            {
                printf( "[camera] concidering %llX\n", addr );
                bool canProject = *(int *)( addr + 384 ) != 0x7FFFFFFF || *(int *)( addr + 388 ) != 0x7FFFFFFF;
                if ( canProject )
                {
                    canProject = *(int *)( addr + 392 ) != 0x7FFFFFFF;
                }

                if ( canProject )
                {
                    float fv = *(float *)( addr + 0x18 );
                    canProject = fv == 0.00f;
                }

                if ( canProject )
                {
                    bloodstrike::renderer::camera = addr;
                    printf( "[camera] set camera to %llX\n", addr );

                    HWND consoleWindow = GetConsoleWindow( );
                    ShowWindow( consoleWindow, SW_HIDE );
                    fclose( stderr );
                    if ( f ) fclose( f );
                    FreeConsole( );
                    break;
                }
                else
                {
                    printf( "[invalid camera] %llX\n", addr );
                }
            }
        }
    }

    if ( GetAsyncKeyState( VK_INSERT ) & 1 || GetAsyncKeyState(VK_BACK) & 1  )
    {
		menuOpen = !menuOpen;
    }

    if ( menuOpen )
    {
        ImGui::Begin( "the hermes bag came pink" );
        ImGui::Text( "Total objects: %d", g_IEntity.size( ) );
        ImGui::Text( "bloodstrike internal by WorldToScreen" );
        ImGui::SliderFloat( "fov", &fov, 35.f, 500.f );
		ImGui::Checkbox( "Randomize Aimbot", &randomizeAim );
		ImGui::SliderFloat( "Head Chance", &headChance, 0.0f, 1.0f, "%.2f" );
        if ( !randomizeAim )
        {
            ImGui::Checkbox( "Target chest", &aimBody );
        }
        ImGui::SliderInt( "Clamp Min", &clamp_min, -255, -1 );
        ImGui::SliderInt( "Clamp Max", &clamp_max, 1, 255 );
        ImGui::SliderFloat( "dx/dy sens", &sens, 0.001, 1.000, "%.5f" );
        ImGui::SliderFloat( "animation speed", &anim_speed, 0.01f, 0.2f, "%.02f" );
        if ( ImGui::Button( "Legit Config" ) )
        {
            randomizeAim = true;
            aimBody = false;
            headChance = 0.1855f;
            sens = 0.15;
            clamp_min = -100;
            clamp_max = 100;
            fov = 175.f;
        }
        ImGui::SameLine( ); if ( ImGui::Button( "Rage Config" ) )
        {
			randomizeAim = false;
			aimBody = false;
			headChance = 0.0f;
			sens = 0.95f;
			clamp_min = -255;
			clamp_max = 255;
			fov = 500.f;
        }
        ImGui::End( );
    }

    ImGui::Render( );
    const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 255.f };
    bloodstrike::renderer::contextInstance->OMSetRenderTargets( 1, &bloodstrike::renderer::rtv, nullptr );
    ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

    return oPresent( pSwapChain, SyncInterval, Flags );
}


void Thread( HMODULE hModule )
{
    if ( !findPresent( ) )
    {
        printf( "[-] failed to initalize d3d11..." );
		cleanup( hModule );
        return;
    }

    printf( "[+] present %llX\n", aPresent );

    MH_STATUS status = MH_CreateHook(
        (LPVOID)aPresent,
        &hkPresent,
        (void **)&oPresent
    );
    status = MH_EnableHook( (LPVOID)aPresent );

    status = MH_CreateHook(
        (LPVOID)aResizeBuffers,
        &hkResizeBuffers,
        (void **)&oResizeBuffers
    );
    status = MH_EnableHook( (LPVOID)aResizeBuffers );

    HMODULE hUser32 = GetModuleHandleW( L"user32.dll" );
    if ( !hUser32 ) hUser32 = LoadLibraryW( L"user32.dll" );
    if ( !hUser32 )
    {
        cleanup( hModule );
        return;
    }

    aGetRawInputData = (uint64_t)GetProcAddress( hUser32, "GetRawInputData" );

    status = MH_CreateHook(
        (LPVOID)aGetRawInputData,
        &hkGetRawInputData,
        (void **)&oGetRawInputData
    );

    printf( "[ntdll] created GetRawInputData hook %d\n", status );
    status = MH_EnableHook( (LPVOID)aGetRawInputData );
    printf( "[ntdll] created GetRawInputData hook %d\n", status );

    while ( !GetAsyncKeyState( VK_F6 ) )
    {
		std::this_thread::sleep_for( std::chrono::milliseconds( 40 ) );
    }

    cleanup( hModule );
}


BOOL APIENTRY DllMain( HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch ( ul_reason_for_call )
    {
    case DLL_PROCESS_ATTACH:
        AllocConsole( );
        freopen_s( &f, "CONOUT$", "w", stdout );
        freopen_s( &f, "CONIN$", "r", stdin );
        base = (uint64_t)GetModuleHandleA( NULL );

        printf( "[+] EngineBase %llX\n", base );

        printf( "[+] hooks\n" );


        if ( MH_Initialize( ) == MH_OK )
        {
            printf( "[+] minhook init success, game @ %llX\n", base );
            aIObjectInitalizer = bloodstrike::funcs::Messiah__IObject__Initalizer + base;
            aIObjectDeconstructor = bloodstrike::funcs::Messiah__IObject__Deconstructor + base;
            aProject = bloodstrike::funcs::Messiah_WorldToScreen + base;
            aGetBoneTransform = bloodstrike::funcs::Messiah__GetBoneTransform + base;

            MH_STATUS status = MH_CreateHook(
                reinterpret_cast<void *>( aIObjectInitalizer ),
                &hkIObjectInitalizer,
                reinterpret_cast<void **>( &oIObjectInitalizer )
            );
            // printf( "Messiah__IEntity__Initalizer CreateHook %d\n", status );
            status = MH_EnableHook( reinterpret_cast<void *>( aIObjectInitalizer ) );
            // printf( "Messiah__IEntity__Initalizer EnableHook %d\n", status );

            status = MH_CreateHook(
                reinterpret_cast<void *>( aIObjectDeconstructor ),
                &hkIObjectDeconstructor,
                reinterpret_cast<void **>( &oIObjectDeconstructor )
            );
            status = MH_EnableHook( reinterpret_cast<void *>( aIObjectDeconstructor ) );

            status = MH_CreateHook(
                reinterpret_cast<void *>( aGetBoneTransform ),
                &hkGetBoneTransform,
                reinterpret_cast<void **>( &oGetBoneTransform )
            );
            status = MH_EnableHook( reinterpret_cast<void *>( aGetBoneTransform ) );
        }
        else
        {
            printf( "[-] failed to init minhook\n" );
        }

        CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)Thread, hModule, NULL, NULL );

        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}