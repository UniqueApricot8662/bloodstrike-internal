#include "pch.h"
using namespace sdk;
using namespace hooks;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void CheatTick( )
{
    uint64_t ClientEngine = *(uint64_t *)( base + bloodstrike::offsets::Messiah__ClientEngine );
    if ( !ClientEngine ) return;

    uint64_t IGameplay = *(uint64_t *)( ClientEngine + 0x58 );
    if ( !IGameplay ) return;

    uint64_t ClientPlayer = *(uint64_t *)( IGameplay + 0x58 );
    if ( !ClientPlayer ) return;

    bloodstrike::renderer::camera = *(uint64_t *)( ClientPlayer + 0x238 );
    bloodstrike::renderer::localActor = *(uint64_t *)( ClientPlayer + 0x288 );

    if ( !bloodstrike::renderer::camera || !bloodstrike::renderer::localActor ) return;
    glm::mat4x3 local_trans = *(glm::mat4x3 *)( bloodstrike::renderer::localActor + 0x58 );
    glm::vec3 local_pos = local_trans[3];

    float closest_dst = FLT_MAX;
    glm::vec2 target_pos{ -1.f, -1.f };
    glm::vec3 target_pos3d{ -1.f, -1.f, -1.f };
    ImVec2 ds = ImGui::GetIO( ).DisplaySize;
    ds.x /= 2.f;
    ds.y /= 2.f;

    glm::vec2 sc = { ds.x, ds.y };

    uint64_t entityListStart = *(uint64_t *)( base + bloodstrike::offsets::Messiah__EntityList );
    if ( !entityListStart ) return;

    uint64_t head = *(uint64_t *)( entityListStart + 0x8 );
    if ( !head ) return;

    uint64_t currentActor = *(uint64_t *)( head );

    int valid = 0;
    int skipped = 0;
    bool isMisc = false;
    if ( currentActor )
    {
        do
        {
            isMisc = false;
            targetAddr = 0x0;
            uint64_t actorInstance = *(uint64_t *)( currentActor + 0x18 );
            if ( !actorInstance )
            {
                currentActor = *(uint64_t *)( currentActor );
                skipped++;
                continue;
            }

            uint64_t actorProps = *(uint64_t *)( actorInstance + 0x278 );
            if ( !actorProps )
            {
                currentActor = *(uint64_t *)( currentActor );
                skipped++;
                continue;
            }

            uint64_t actorComponent = *(uint64_t *)( actorProps + 0x18 );
            if ( !actorComponent )
            {
                currentActor = *(uint64_t *)( currentActor );
                skipped++;
                continue;
            }

            uint64_t IEntity = *(uint64_t *)( actorComponent + 0x40 );
            if ( !IEntity )
            {
                currentActor = *(uint64_t *)( currentActor );
                skipped++;
                continue;
            }

            uint64_t entityMask = *(uint64_t *)( IEntity + 0x2e0 );
            if ( entityMask != 2 )
            {
                isMisc = true;
            }

            if ( IEntity == bloodstrike::renderer::localActor )
            {
                if ( debugStats )
                {
                    visuals::DrawLabel( std::format( "Local {:x}\nCamera {:x}\nEntity List {:x}\nSkipped: {} | Valid: {}", bloodstrike::renderer::localActor, bloodstrike::renderer::camera, head, skipped, valid ), glm::vec2( 150, 100 ), visuals::ColorToArray( ImColor( 255, 255, 255, 255 ) ), true );
                }
                currentActor = *(uint64_t *)( currentActor );
                skipped++;
                continue;
            }

            uint64_t IArea = *(uint64_t *)( IEntity + 0x88 );

            if ( IArea == 0x0 )
            {
                currentActor = *(uint64_t *)( currentActor );
                skipped++;
                continue;
            }

            uint64_t pose = *(uint64_t *)( actorInstance + 0x18 );
            if ( !pose )
            {
                currentActor = *(uint64_t *)( currentActor );
                skipped++;
                continue;
            }

            uint64_t BipedPose = *(uint64_t *)( pose + 0x90 );
            if ( !BipedPose )
            {
                currentActor = *(uint64_t *)( currentActor );
                skipped++;
                continue;
            }

            BipedPose += 0x8; // first bone ptr is garbage

            glm::vec2 result;
            glm::mat4x3 trans = *(glm::mat4x3 *)( IEntity + 0x58 );
            XMFLOAT3X4 dxTrans = *(XMFLOAT3X4 *)( IEntity + 0x58 );

            glm::vec3 coords = trans[3];
            float d = ( glm::distance( local_pos, coords ) );
            float d2 = (int)d;

            if ( w2s( bloodstrike::renderer::camera, coords, result ) )
            {
                ImVec2 pos = { (float)result[0], (float)result[1] };
                if ( ( pos.x > ImGui::GetIO( ).DisplaySize.x || pos.y > ImGui::GetIO( ).DisplaySize.y ) || ( pos.x < 1.0 || pos.y < 1.0 ) )
                {
                    currentActor = *(uint64_t *)( currentActor );
                    skipped++;
                    continue;
                }
                int dst_m = (int)( d2 / 5 );

                if ( dst_m < 1000 && !isMisc )
                {
                    float t = d2 / 10.f;

                    std::string address_txt = std::format( "[{:x}]", IEntity );
                    std::string dist_txt = std::format( "{:d}m", dst_m );

                    t = std::clamp( t, 0.f, 1.f );

                    float g_lin = powf( t, 2.2f );
                    float r_lin = powf( 1.f - t, 2.2f );

                    ImColor color( r_lin, g_lin, 0.f, 1.f );

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

                        boneStart = *(uint64_t *)( ( 18 * 0x8 ) + BipedPose );
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

                        float height = abs( neck.y - result.y );
                        float width = height * 0.4;

                        if ( kingKongEsp && bloodstrike::renderer::srv != nullptr )
                        {
                            ImVec2 topLeft( neck.x - width, neck.y );
                            ImVec2 bottomRight( topLeft.x + width * 1.8f, topLeft.y + height );

                            ImGui::GetBackgroundDrawList( )->AddImage(
                                (ImTextureID)bloodstrike::renderer::srv,
                                topLeft,
                                bottomRight
                            );
                        }

                        valid++;
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


                        float dist = glm::distance( sc, neck );
                        if ( dist < closest_dst )
                        {
                            targetAddr = IEntity;
                            closest_dst = dist;
                            if ( randomizeAim )
                            {
                                float r = static_cast <float> ( rand( ) ) / static_cast <float> ( RAND_MAX );
                                if ( r < headChance )
                                {
                                    target_pos = neck;
                                    target_pos3d = _neck;
                                }
                                else
                                {
                                    target_pos = spine3;
                                    target_pos3d = _spine3;
                                }
                            }
                            else
                            {
                                target_pos = aimBody ? spine3 : neck;
                                target_pos3d = aimBody ? _spine3 : _neck;
                            }
                        }

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
                        visuals::DrawLabel( address_txt, glm::vec2( result.x, result.y + ( z / 3 ) ), visuals::ColorToArray( ImColor( 255, 255, 255, 255 ) ), true );
                    }
                }
                else if ( dst_m < 1000 )
                {
                    float t = d2 / 10.f;

                    std::string txt = std::format( "[{}m] object~{}", dst_m, entityMask );
                    visuals::DrawLabel( txt, glm::vec2( result.x, result.y ), visuals::ColorToArray( ImColor( 125, 125, 125, 255 ) ), true );
                }
            }

            currentActor = *(uint64_t *)( currentActor );
            valid++;
        } while ( currentActor != head );
        
        ImGui::GetBackgroundDrawList( )->AddCircle( ImVec2( sc.x, sc.y ), fov, ImColor( 255, 0, 0, 200 ) );
        if ( aimbotDebug )
        {
            for ( auto &pt : bloodstrike::aimbotPoints )
            {
                bloodstrike::aimbotPoints.erase(
                    std::remove_if(
                        bloodstrike::aimbotPoints.begin( ),
                        bloodstrike::aimbotPoints.end( ),
                        [&]( const auto &p ) { return p.duration <= 0; }
                    ),
                    bloodstrike::aimbotPoints.end( )
                );

                pt.duration = std::clamp<int>( pt.duration, 0, frameDebugDuration );

                glm::vec2 from2d, to2d;
                if ( w2s( bloodstrike::renderer::camera, pt.to, to2d ) )
                {
                    w2s( bloodstrike::renderer::camera, pt.from, from2d, true );
                    int a = (int)( 255 * ( pt.duration / (float)frameDebugDuration ) );
                    ImGui::GetBackgroundDrawList( )->AddLine( ImVec2( from2d.x, from2d.y ), ImVec2( to2d.x, to2d.y ), ImColor( 255, 255, 0, a ), 1.5f );
                }

                pt.duration--;
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

            should_change_mouse = true;

            targetAddr = lastTarget;

            if ( aimbotDebug )
            {
                if ( ImGui::GetFrameCount( ) % frameDebugDelay == 0 )
                {
                    AimbotDebugPoint st;
                    st.from = local_pos;
                    st.to = target_pos3d;
                    st.duration = frameDebugDuration;

                    bloodstrike::aimbotPoints.push_back( st );
                }
            }
        }
        else
        {
            should_change_mouse = false;
            closest_dst = FLT_MAX;
        }
    }

    // UGLY ImGui::GetForegroundDrawList( )->AddCircle( { target_pos.x, target_pos.y }, 5.f, ImColor( 200, 50, 250, 255 ) );
}

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

#ifdef DEBUG
        printf( "[+] imgui init\n" );
#endif

        int w, h, channels = 0;
        unsigned char *pixels = stbi_load_from_memory(
            (const stbi_uc *)kk,
            sizeof(kk),
            &w,
            &h,
            &channels,
            4
        );

        if ( pixels )
        {
            D3D11_TEXTURE2D_DESC desc{};
            desc.Width = w;
            desc.Height = h;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            D3D11_SUBRESOURCE_DATA sub{};
            sub.pSysMem = pixels;
            sub.SysMemPitch = w * 4;

            ID3D11Texture2D *texture = nullptr;
            bloodstrike::renderer::deviceInstance->CreateTexture2D( &desc, &sub, &texture );

            bloodstrike::renderer::deviceInstance->CreateShaderResourceView( texture, nullptr, &bloodstrike::renderer::srv );
            texture->Release( );
            stbi_image_free( pixels );

            bloodstrike::renderer::hooked = true;
        }

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

    if ( hooks::g_needsResize )
    {
        ID3D11Texture2D *backBuffer = nullptr;
        pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void **)&backBuffer );
        if ( backBuffer )
        {
            bloodstrike::renderer::deviceInstance->CreateRenderTargetView( backBuffer, nullptr, &bloodstrike::renderer::rtv );
            backBuffer->Release( );
            ImGui_ImplDX11_InvalidateDeviceObjects( );
            ImGui_ImplDX11_CreateDeviceObjects( );
        }

        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        vp.Width = static_cast<FLOAT>( hooks::g_newWidth );
        vp.Height = static_cast<FLOAT>( hooks::g_newHeight );
        vp.MinDepth = 0.f;
        vp.MaxDepth = 1.f;
        bloodstrike::renderer::contextInstance->RSSetViewports( 1, &vp );

        ImGuiIO &io = ImGui::GetIO( );
        io.DisplaySize.x = static_cast<float>( hooks::g_newWidth );
        io.DisplaySize.y = static_cast<float>( hooks::g_newHeight );

        hooks::g_needsResize = false;
    }


    ImGui_ImplDX11_NewFrame( );
    ImGui_ImplWin32_NewFrame( );
    ImGui::NewFrame( );

    if ( GetAsyncKeyState( VK_INSERT ) & 1 || GetAsyncKeyState(VK_BACK) & 1  )
    {
		menuOpen = !menuOpen;
    }

    if ( menuOpen )
    {
        ImGui::Begin( "illustrious.wtf king king edition" );
        ImGui::Text( "bloodstrike internal by WorldToScreen" );
        ImGui::Checkbox( "Aimbot Debug", &aimbotDebug );
        ImGui::Checkbox( "Do Debug Stats", &debugStats );
        ImGui::Checkbox( "King Kong ESP", &kingKongEsp );
        ImGui::SliderInt( "Aimbot Debug FrameDelay", &frameDebugDelay, 1, 100 );
        ImGui::SliderInt( "Aimbot Debug FrameDuration", &frameDebugDuration, 10, 10000 );
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
            aimbotDebug = false;
        }
        ImGui::SameLine( ); if ( ImGui::Button( "Rage Config" ) )
        {
            aimbotDebug = true;
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

    CheatTick( );

    ImGui::Render( );
    const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 255.f };
    bloodstrike::renderer::contextInstance->OMSetRenderTargets( 1, &bloodstrike::renderer::rtv, nullptr );
    ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

    return oPresent( pSwapChain, SyncInterval, Flags );
}


void Thread( HMODULE hModule )
{
    if ( !findPresent( ) || MH_Initialize() != MH_OK )
    {
#ifdef DEBUG
        printf( "[-] failed to initalize d3d11..." );
#endif
		cleanup( hModule );
        return;
    }

#ifdef DEBUG
    printf( "[+] present %llX\n", aPresent );
#endif

    MH_STATUS status = MH_CreateHook(
        (LPVOID)aPresent,
        &hkPresent,
        (void **)&oPresent
    );
    status = MH_EnableHook( (LPVOID)aPresent );

#ifdef DEBUG
    printf( "hooked Present -> %d\n", status );
#endif

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
    status = MH_EnableHook( (LPVOID)aGetRawInputData );

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
        base = (uint64_t)GetModuleHandleA( NULL );


#ifdef DEBUG
        AllocConsole( );
        freopen_s( &f, "CONOUT$", "w", stdout );
        freopen_s( &f, "CONIN$", "r", stdin );
        printf( "[+] init\n" );
#endif

        CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)Thread, hModule, NULL, NULL );

        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}