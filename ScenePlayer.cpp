//--------------------------------------------------------------------------------------
// File: BasicHLSL.cpp
//
// This sample shows a simple example of the Microsoft Direct3D's High-Level
// Shader Language (HLSL) using the Effect interface.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <iostream>
#include "resource.h"

#include "cfg.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders

#include <d3d9.h>

#include <Common\dxstdafx.h>
#include <Common\DXUT.h>
#include <Common\DXUTmisc.h>
#include <Common\DXUTgui.h>
#include <Common\DXUTsettingsdlg.h>

#include <memory>
#include <Modules/mutant/mutant.h>
#include <Modules/mutant/reader.h>
#include <Modules/mutant/io_factory.h>

#include <Modules/mutalisk/dx9Platform.h>
#include <Modules/mutalisk/mutalisk.h>
#include <Modules/player/ScenePlayer.h>
#include <Modules/player/dx9ScenePlayer.h>

#include "guicon.h"

#include "TestDemo.h"

namespace {
  void splitFilename(std::string const& fullPath, std::string& path, std::string& fileName) {
    size_t offset0 = fullPath.find_last_of('/');
    size_t offset1 = fullPath.find_last_of('\\');

    size_t offset = max(offset0, offset1);

    if(offset == std::string::npos){
      offset = min(offset0, offset1);
    }

    path = "";
    fileName = fullPath;
    if(offset == std::string::npos) {
      return;
    }

    ++offset;
    path = fullPath.substr(0, offset);
    fileName = fullPath.substr(offset);
  }
}

// Global variables
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pSprite = NULL;       // Sprite for batching draw text calls
ID3DXEffect*            g_pEffect = NULL;       // D3DX effect interface
CDXUTDialogResourceManager g_DialogResourceManager;
CD3DSettingsDlg         g_SettingsDlg;
CDXUTDialog             g_HUD;
D3DXMATRIXA16           g_mCenterWorld;

// GUI
bool         g_bEnablePreshader;
bool         g_bShowHelp = false;
bool         g_scenePlayerTimePause = false;
double       g_scenePlayerTime = 0.0;
double       g_scenePlayerTimeModifier = 1.0;
const double g_scenePlayerTimeModifierStep = 0.02;

// Demo
std::auto_ptr<TestDemo> gDemo;


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
enum {
  IDC_TOGGLEFULLSCREEN,
  IDC_TOGGLEREF,
  IDC_CHANGEDEVICE
};

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void    CALLBACK OnLostDevice( void* pUserContext );
void    CALLBACK OnDestroyDevice( void* pUserContext );

void    InitApp();
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh );
void    RenderText( double fTime );


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR cmdLine, int )
{
  RedirectIOToConsole();

  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

  // Set the callback functions. These functions allow DXUT to notify
  // the application about device changes, user input, and windows messages.  The
  // callbacks are optional so you need only set callbacks for events you're interested
  // in. However, if you don't handle the device reset/lost callbacks then the sample
  // framework won't be able to reset your device since the application must first
  // release all device resources before resetting.  Likewise, if you don't handle the
  // device created/destroyed callbacks then DXUT won't be able to
  // recreate your device resources.
  DXUTSetCallbackDeviceCreated( OnCreateDevice );
  DXUTSetCallbackDeviceReset( OnResetDevice );
  DXUTSetCallbackDeviceLost( OnLostDevice );
  DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
  DXUTSetCallbackMsgProc( (LPDXUTCALLBACKMSGPROC)MsgProc );
  DXUTSetCallbackKeyboard( (LPDXUTCALLBACKKEYBOARD)KeyboardProc );
  DXUTSetCallbackFrameRender( OnFrameRender );
  DXUTSetCallbackFrameMove( OnFrameMove );

  // Show the cursor and clip it when in full screen
  DXUTSetCursorSettings( true, true );

  InitApp();

  // Initialize DXUT and create the desired Win32 window and Direct3D
  // device for the application. Calling each of these functions is optional, but they
  // allow you to set several options which control the behavior of the framework.
  DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
  DXUTCreateWindow( L"Suicide Barbie" );

  float const screenScaler = 3;

  DXUTCreateDevice( D3DADAPTER_DEFAULT, true, (int) (480*screenScaler), (int) (272*screenScaler), (LPDXUTCALLBACKISDEVICEACCEPTABLE)IsDeviceAcceptable, (LPDXUTCALLBACKMODIFYDEVICESETTINGS)ModifyDeviceSettings );

  // Pass control to DXUT for handling the message pump and
  // dispatching render calls. DXUT will call your FrameMove
  // and FrameRender callback when there is idle time between handling window messages.
  DXUTMainLoop();

  // Perform any application-level cleanup here. Direct3D device resources are released within the
  // appropriate callback functions and therefore don't require any cleanup code here.

  return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{
  g_bEnablePreshader = true;

  // Initialize dialogs
  g_SettingsDlg.Init(&g_DialogResourceManager);
  g_HUD.Init(&g_DialogResourceManager);

  g_HUD.SetCallback( OnGUIEvent );
  int iY = 10;
  g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22);
  g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2);
  g_HUD.AddButton(IDC_TOGGLEREF, L"Toggle SW rendering (F3)", 35, iY += 24, 125, 22);
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning E_FAIL.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext) {
  // No fallback defined by this app, so reject any device that doesn't support at least ps2.0
  if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) ){
    return false;
  }

  // Skip backbuffer formats that don't support alpha blending
  IDirect3D9* pD3D = DXUTGetD3DObject();
  if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_TEXTURE, BackBufferFormat ) ) ) {
    return false;
  }

  return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// DXUT will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
  // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW then switch to SWVP.
  if((pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 || pCaps->VertexShaderVersion < D3DVS_VERSION(1,1))
  {
    pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
  }

  // Debugging vertex shaders requires either REF or software vertex processing
  // and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
  if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
  {
    pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
    pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
    pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
  }
#endif
#ifdef DEBUG_PS
  pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif

  if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF ) {
     std::cerr << "Using Software rendering" << std::endl;
  }

  return true;
}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
  HRESULT hr;

  V_RETURN( g_DialogResourceManager.OnCreateDevice( pd3dDevice ) );
  V_RETURN( g_SettingsDlg.OnCreateDevice( pd3dDevice ) );
  // Initialize the font
  V_RETURN( D3DXCreateFont(pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &g_pFont ) );

  D3DXVECTOR3 vCenter;
  FLOAT fObjectRadius;

  static float newRadius = 250.0f;
  if(newRadius > 0)
  {
    vCenter.x = 0;
    vCenter.y = 0;
    vCenter.z = 0;
    fObjectRadius = newRadius;
  }

  D3DXMatrixTranslation( &g_mCenterWorld, -vCenter.x, -vCenter.y, -vCenter.z );
  D3DXMATRIXA16 m;
  D3DXMatrixRotationY( &m, D3DX_PI );
  g_mCenterWorld *= m;
  D3DXMatrixRotationX( &m, D3DX_PI / 2.0f );
  g_mCenterWorld *= m;

  // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the
  // shader debugger. Debugging vertex shaders requires either REF or software vertex
  // processing, and debugging pixel shaders requires REF.  The
  // D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the
  // shader debugger.  It enables source level debugging, prevents instruction
  // reordering, prevents dead code elimination, and forces the compiler to compile
  // against the next higher available software target, which ensures that the
  // unoptimized shaders do not exceed the shader model limitations.  Setting these
  // flags will cause slower rendering since the shaders will be unoptimized and
  // forced into software.  See the DirectX documentation for more information about
  // using the shader debugger.
  DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
#ifdef DEBUG_VS
  dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
  dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

  // Preshaders are parts of the shader that the effect system pulls out of the
  // shader and runs on the host CPU. They should be used if you are GPU limited.
  // The D3DXSHADER_NO_PRESHADER flag disables preshaders.
  if( !g_bEnablePreshader ) {
    dwShaderFlags |= D3DXSHADER_NO_PRESHADER;
  }

  // Read the D3DX effect file
  WCHAR str[MAX_PATH];
  V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"MutaliskUberShader.fx" ) );

  // If this fails, there should be debug output as to why the .fx file failed to compile
  com_ptr<ID3DXBuffer> errorBuffer;
  HRESULT hr2 = D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, dwShaderFlags, NULL, &g_pEffect, &errorBuffer );
  if(errorBuffer) {
    std::string errorStr = std::string((char*)errorBuffer->GetBufferPointer(), errorBuffer->GetBufferSize());
  }
  V_RETURN(hr2);

  g_scenePlayerTime = 0.0;
  gDemo.reset(new TestDemo());
  gDemo->platformSetup(*pd3dDevice, *g_pEffect);
  gDemo->start();
  return S_OK;
}

//--------------------------------------------------------------------------------------
// This function loads the mesh and ensures the mesh has normals; it also optimizes the
// mesh for the graphics card's vertex cache, which improves performance by organizing
// the internal triangle list for less cache misses.
//--------------------------------------------------------------------------------------
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh )
{
  ID3DXMesh* pMesh = NULL;
  WCHAR str[MAX_PATH];
  HRESULT hr;

  // Load the mesh with D3DX and get back a ID3DXMesh*.  For this
  // sample we'll ignore the X file's embedded materials since we know
  // exactly the model we're loading.  See the mesh samples such as
  // "OptimizedMesh" for a more generic mesh loading example.
  V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strFileName ) );
  V_RETURN( D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, pd3dDevice, NULL, NULL, NULL, NULL, &pMesh) );

  DWORD *rgdwAdjacency = NULL;

  // Make sure there are normals which are required for lighting
  if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
  {
    ID3DXMesh* pTempMesh;
    V( pMesh->CloneMeshFVF( pMesh->GetOptions(),
      pMesh->GetFVF() | D3DFVF_NORMAL,
      pd3dDevice, &pTempMesh ) );
    V( D3DXComputeNormals( pTempMesh, NULL ) );

    SAFE_RELEASE( pMesh );
    pMesh = pTempMesh;
  }

  // Optimize the mesh for this graphics card's vertex cache
  // so when rendering the mesh's triangle list the vertices will
  // cache hit more often so it won't have to re-execute the vertex shader
  // on those vertices so it will improve perf.
  rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
  if( rgdwAdjacency == NULL )
    return E_OUTOFMEMORY;
  V( pMesh->GenerateAdjacency(1e-6f,rgdwAdjacency) );
  V( pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL) );
  delete []rgdwAdjacency;

  *ppMesh = pMesh;

  return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
  HRESULT hr;

  V_RETURN(g_DialogResourceManager.OnResetDevice());
  V_RETURN(g_SettingsDlg.OnResetDevice());

  if( g_pFont ) {
    V_RETURN(g_pFont->OnResetDevice());
  }
  if( g_pEffect ) {
    V_RETURN(g_pEffect->OnResetDevice());
  }

  // Create a sprite to help batch calls when drawing many lines of text
  V_RETURN(D3DXCreateSprite(pd3dDevice, &g_pSprite));

  g_HUD.SetLocation(pBackBufferSurfaceDesc->Width-170, 0);
  g_HUD.SetSize(170, 170);

  return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
  if (!g_scenePlayerTimePause) {
    g_scenePlayerTime += fElapsedTime * g_scenePlayerTimeModifier;
  }
  gDemo->updateFrame((float) g_scenePlayerTime);
}

void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
  // If the settings dialog is being shown, then
  // render it instead of rendering the app's scene
  if(g_SettingsDlg.IsActive()) {
    g_SettingsDlg.OnRender( fElapsedTime );
    return;
  }

  HRESULT hr;
  D3DXMATRIXA16 mWorldViewProjection;
  D3DXMATRIXA16 mWorld;
  D3DXMATRIXA16 mView;
  D3DXMATRIXA16 mProj;

  // Clear the render target and the zbuffer
  V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f,0.25f,0.25f,0.55f), 1.0f, 0) );
  //V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f,0.0f,0.0f,0.0f), 1.0f, 0) );

  // Render the scene
  if( SUCCEEDED( pd3dDevice->BeginScene() ) )
  {
    gDemo->renderFrame();

    // GUI Stuff
    g_HUD.OnRender( fElapsedTime );
    RenderText( fTime );

    V( pd3dDevice->EndScene() );
  }
}

void RenderText( double fTime )
{
  // The helper object simply helps keep track of text position, and color
  // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
  // If NULL is passed in as the sprite object, then it will work fine however the
  // pFont->DrawText() will not be batched together.  Batching calls will improves perf.
  CDXUTTextHelper txtHelper( g_pFont, g_pSprite, 15 );

  // Output statistics
  txtHelper.Begin();
  txtHelper.SetInsertionPos( 2, 0 );
  txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
  txtHelper.DrawTextLine( DXUTGetFrameStats() );
  txtHelper.DrawTextLine( DXUTGetDeviceStats() );

  txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
  txtHelper.DrawFormattedTextLine( L"fTime: %0.2f", fTime);
  txtHelper.DrawFormattedTextLine( L"demoTime: %0.2f @ %0.2f", g_scenePlayerTime, g_scenePlayerTimeModifier );

  std::wstring scene = L"None";

  if (gDemo.get() != NULL) {
    scene = gDemo->getSceneName();
  }

  txtHelper.DrawFormattedTextLine( L"scene: %s", scene.c_str() );

  // Draw help
  if( g_bShowHelp )
  {
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
    txtHelper.SetInsertionPos( 2, pd3dsdBackBuffer->Height-15*6 );
    txtHelper.SetForegroundColor( D3DXCOLOR(1.0f, 0.75f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( L"Controls:" );

    txtHelper.SetInsertionPos( 20, pd3dsdBackBuffer->Height-15*5 );
    txtHelper.DrawTextLine( L"Pause: Space bar\n"
      L"Scrub: Left and right keys\n"
      L"Speed: Up and down keys\n" );

    txtHelper.SetInsertionPos( 250, pd3dsdBackBuffer->Height-15*5 );
    txtHelper.DrawTextLine( L"Hide help: F1\n"
      L"Quit: ESC\n" );
  }
  else
  {
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
    txtHelper.DrawTextLine( L"Press F1 for help" );
  }
  txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows
// messages to the application through this callback function. If the application sets
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
  // Always allow dialog resource manager calls to handle global messages
  // so GUI state is updated correctly
  *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
  if( *pbNoFurtherProcessing ) {
    return 0;
  }

  if( g_SettingsDlg.IsActive() ) {
    g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
    return 0;
  }

  // Give the dialogs a chance to handle the message first
  *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
  if( *pbNoFurtherProcessing ) {
    return 0;
  }

  return 0;
}

void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
  if( bKeyDown )
  {
    double speedModifier = 1.0;
    if(bAltDown) {
      speedModifier = 0.01;
    }

    switch( nChar )
    {
    case VK_F1:
      g_bShowHelp = !g_bShowHelp;
      break;
    case VK_UP:
      if (g_scenePlayerTimeModifier <= 4.0) {
        g_scenePlayerTimeModifier += g_scenePlayerTimeModifierStep;
      }
      break;
    case VK_DOWN:
      if (g_scenePlayerTimeModifier >= g_scenePlayerTimeModifierStep) {
        g_scenePlayerTimeModifier -= g_scenePlayerTimeModifierStep;
      }
      break;
    case VK_LEFT:
      if (g_scenePlayerTime > 1.0) {
        g_scenePlayerTime -= 1.0 * speedModifier;
      } else {
        g_scenePlayerTime = 0.0 * speedModifier;
      }
      break;
    case VK_RIGHT:
      g_scenePlayerTime += 1.0 * speedModifier;
      break;
    case VK_SPACE:
      g_scenePlayerTimePause = !g_scenePlayerTimePause;
      break;
    case VK_NUMPAD3:
      break;
    case VK_NUMPAD9:
      break;
    case 'R':
      g_scenePlayerTimeModifier = 1.0;
      g_scenePlayerTime = 0.0;
      break;
    }
  }
}

void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
  switch( nControlID )
  {
  case IDC_TOGGLEFULLSCREEN:
    DXUTToggleFullScreen();
    break;
  case IDC_TOGGLEREF:
    DXUTToggleREF();
    break;
  case IDC_CHANGEDEVICE:
    g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() );
    break;
  }
}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext ) {
  gDemo.release();
  g_DialogResourceManager.OnLostDevice();
  g_SettingsDlg.OnLostDevice();
  if( g_pFont ) {
    g_pFont->OnLostDevice();
  }
  if( g_pEffect ) {
    g_pEffect->OnLostDevice();
  }
  g_pSprite->OnLostDevice();
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext ) {
  gDemo.release();
  g_DialogResourceManager.OnDestroyDevice();
  g_SettingsDlg.OnDestroyDevice();
  SAFE_RELEASE(g_pEffect);
  SAFE_RELEASE(g_pFont);
  SAFE_RELEASE(g_pSprite);
}
