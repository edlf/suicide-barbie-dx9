#include "dx9ScenePlayer.h"

#include <memory>
#include <d3d9types.h>
#include <Modules/effects/all.h>
#include <Modules/effects/Library.h>
#include <Modules/effects/BaseEffect.h>

namespace mutalisk
{
  using namespace effects;

////////////////////////////////////////////////

struct Settings
{
  int overrideCameraMethod;
  bool forceIdentityActorsMatrix;
  bool forceAnimatedActors;
  bool forceAnimatedCamera;
  bool debugHierarchy;

  Settings()
    : overrideCameraMethod(1)
    , forceIdentityActorsMatrix(false)
    , forceAnimatedActors(true)
    , forceAnimatedCamera(true)
    , debugHierarchy(false)
  {}
};
static Settings gSettings;


namespace {
  unsigned int indices2Faces(unsigned int indexCount, D3DPRIMITIVETYPE primitiveType)
  {
    switch(primitiveType)
    {
    case D3DPT_POINTLIST:
      return indexCount;
    case D3DPT_LINELIST:
      return indexCount / 2;
    case D3DPT_LINESTRIP:
      return indexCount - 1;
    case D3DPT_TRIANGLELIST:
      return indexCount / 3;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:
      return indexCount - 2;
    default:
      ASSERT(!"Unknown primitive type");
      return 0;
    }
  }

  static unsigned int faces2Indices(unsigned int  faceCount, D3DPRIMITIVETYPE primitiveType)
  {
    switch(primitiveType)
    {
    case D3DPT_POINTLIST:
      return faceCount;
    case D3DPT_LINELIST:
      return faceCount * 2;
    case D3DPT_LINESTRIP:
      return faceCount + 1;
    case D3DPT_TRIANGLELIST:
      return faceCount * 3;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:
      return faceCount + 2;
    default:
      ASSERT(!"Unknown primitive type");
      return 0;
    }
  }
} // namespace

////////////////////////////////////////////////
//template <>
static com_ptr<IDirect3DTexture9> prepare(RenderContext& rc, std::string fileName)
{
  com_ptr<IDirect3DTexture9> resource;
  DX_MSG("load texture") =
    D3DXCreateTextureFromFileA(rc.device, (getResourcePath() + fileName).c_str(), &resource);
  return resource;
}

////////////////////////////////////////////////
std::auto_ptr<Dx9RenderableScene> prepare(RenderContext& rc, mutalisk::data::scene const& data)
{
  std::auto_ptr<Dx9RenderableScene> scene(new Dx9RenderableScene(data));

  // load shared resources
  scene->mResources.meshes.resize(data.meshIds.size());
  for(size_t q = 0; q < data.meshIds.size(); ++q)
  {
    scene->mResources.meshes[q].blueprint = loadResource<mutalisk::data::mesh>(data.meshIds[q]);
    scene->mResources.meshes[q].renderable = prepare(rc, *scene->mResources.meshes[q].blueprint);
  }
  scene->mNativeResources.textures.resize(data.textureIds.size());
  for(size_t q = 0; q < data.textureIds.size(); ++q)
  {
    scene->mNativeResources.textures[q] = prepare(rc, data.textureIds[q]); //loadResource<IDirect3DTexture9>(rc, data.textureIds[q]);
  }

  scene->mResources.animCharSet = loadResource<mutant::anim_character_set>(data.animCharId);

  // setup scene
  scene->setClip(data.defaultClipIndex);

  return scene;
}

std::auto_ptr<RenderableMesh> prepare(RenderContext& rc, mutalisk::data::mesh const& data)
{
  std::auto_ptr<RenderableMesh> mesh(new RenderableMesh(data));

  unsigned int bitsPerIndex = (data.indexSize == 4)? D3DXMESH_32BIT: 0;
  DX_MSG("create mesh") = D3DXCreateMeshFVF(
    (DWORD)indices2Faces(data.indexCount, data.primitiveType),
    data.vertexCount,
    D3DXMESH_MANAGED | bitsPerIndex,
    data.fvfVertexDecl,
    rc.device,
    &mesh->mNative);

  {
    uint8_t* data = 0;
    DX_MSG("lock vertex buffer for writing") = mesh->mNative->LockVertexBuffer(0, (void**)&data);
    memcpy(data, mesh->mBlueprint.vertexData, mesh->mBlueprint.vertexDataSize);
    DX_MSG("unlock vertex buffer") = mesh->mNative->UnlockVertexBuffer();
  }

  {
    uint8_t* data = 0;
    DX_MSG("lock index buffer for writing") = mesh->mNative->LockIndexBuffer(0, (void**)&data);
    memcpy(data, mesh->mBlueprint.indexData, mesh->mBlueprint.indexCount * mesh->mBlueprint.indexSize);
    DX_MSG("unlock index buffer") = mesh->mNative->UnlockIndexBuffer();
  }

  if(!data.subsets.empty())
  {
    // fill attribute table according to subsets
    DWORD* faceAttribute = 0;
    DX_MSG("lock attribute buffer for writing") = mesh->mNative->LockAttributeBuffer(0, &faceAttribute);
    for(size_t q = 0; q < data.subsets.size(); ++q)
    {
      size_t faceStart = indices2Faces(data.subsets[q].offset, data.primitiveType);
      size_t faceCount = indices2Faces(data.subsets[q].count, data.primitiveType);

      for(size_t w = faceStart; w < faceStart + faceCount; ++w)
        faceAttribute[w] = (DWORD)q;
    }
    DX_MSG("unlock attribute buffer") = mesh->mNative->UnlockAttributeBuffer();
  }
  else
  {
    DWORD* adjacency = new DWORD[mesh->mNative->GetNumFaces() * 3 * sizeof(DWORD)];
    DX_MSG("generate adjacency") =
      mesh->mNative->GenerateAdjacency(std::numeric_limits<float>::epsilon(), adjacency);
    DX_MSG("inplace optimize for vertex cache") =
      mesh->mNative->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, adjacency, 0, 0, 0);
    delete[] adjacency;
  }

  {
    uint8_t* data = 0;
    DX_MSG("lock vertex buffer for reading") = mesh->mNative->LockVertexBuffer(0, (void**)&data);
    memcpy(mesh->mBlueprint.vertexData, data, mesh->mBlueprint.vertexDataSize);
    DX_MSG("unlock vertex buffer") = mesh->mNative->UnlockVertexBuffer();
  }

  {
    uint8_t* data = 0;
    DX_MSG("lock index buffer for reading") = mesh->mNative->LockIndexBuffer(0, (void**)&data);
    memcpy(mesh->mBlueprint.indexData, data, mesh->mBlueprint.indexCount * mesh->mBlueprint.indexSize);
    DX_MSG("unlock index buffer") = mesh->mNative->UnlockIndexBuffer();
  }


  return mesh;
}

namespace {
  void toNative(D3DXMATRIX& worldMatrix, CTransform::t_matrix const& src, bool swap = true)
  {
    static int method = 1;
    if(method == 0)
    {
      worldMatrix = D3DXMATRIX(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        src.Move.x, src.Move.y, src.Move.z, 1.0f
      );
    }
    else if(method == 1)
    {
      worldMatrix = D3DXMATRIX(
        src.Rot.Row[0].x, src.Rot.Row[1].x, src.Rot.Row[2].x, 0.0f,
        src.Rot.Row[0].y, src.Rot.Row[1].y, src.Rot.Row[2].y, 0.0f,
        src.Rot.Row[0].z, src.Rot.Row[1].z, src.Rot.Row[2].z, 0.0f,
        src.Move.x, src.Move.y, src.Move.z, 1.0f
      );
    }
    else if(method == 2)
    {
      worldMatrix = D3DXMATRIX(
        src.Rot.Row[0].x, src.Rot.Row[0].y, src.Rot.Row[0].z, 0.0f,
        src.Rot.Row[1].x, src.Rot.Row[1].y, src.Rot.Row[1].z, 0.0f,
        src.Rot.Row[2].x, src.Rot.Row[2].y, src.Rot.Row[2].z, 0.0f,
        src.Move.x, src.Move.y, src.Move.z, 1.0f
      );
    }

    if(swap)
    {
      std::swap(worldMatrix._41, worldMatrix._43);
      std::swap(worldMatrix._11, worldMatrix._13);
      std::swap(worldMatrix._21, worldMatrix._23);
      std::swap(worldMatrix._31, worldMatrix._33);
    }
  }
  void toNative(CTransform::t_matrix& worldMatrix, float const* worldMatrixData)
  {
    worldMatrix = CTransform::t_matrix(
      worldMatrixData[8], worldMatrixData[4], worldMatrixData[0],
      worldMatrixData[9], worldMatrixData[5], worldMatrixData[1],
      worldMatrixData[10], worldMatrixData[6], worldMatrixData[2],
      worldMatrixData[14], worldMatrixData[13], worldMatrixData[12]);
  }
  void toNative(D3DXMATRIX& worldMatrix, float const* worldMatrixData)
  {
    worldMatrix = D3DXMATRIX(
      worldMatrixData[8], worldMatrixData[4], worldMatrixData[0], 0,
      worldMatrixData[9], worldMatrixData[5], worldMatrixData[1], 0,
      worldMatrixData[10], worldMatrixData[6], worldMatrixData[2], 0,
      worldMatrixData[14], worldMatrixData[13], worldMatrixData[12], 1);
  }
  Vec3 const& getTranslation(Vec3& translation, D3DXMATRIX const& nativeMatrix)
  {
    translation.x = nativeMatrix._41;
    translation.y = nativeMatrix._42;
    translation.z = nativeMatrix._43;
    return translation;
  }


  void toNative(D3DXVECTOR4& dst, mutalisk::data::Vec4 const& src)
  {
    dst.x = src[0]; dst.y = src[1]; dst.z = src[2]; dst.w = src[3];
  }
  void toNative(D3DXCOLOR& c, mutalisk::data::Color const& color)
  {
    c.r = color.r; c.g = color.g; c.b = color.b; c.a = color.a;
  }

  void toNative(BaseEffect::Input::Surface& dst, Dx9RenderableScene const& scene, mutalisk::data::shader_fixed const& src)
  {
    toNative(dst.ambient, src.ambient);
    toNative(dst.diffuse, src.diffuse);
    toNative(dst.specular, src.specular);
    toNative(dst.emissive, src.emissive);

    dst.diffuseTexture = (src.diffuseTexture != ~0U)? scene.mNativeResources.textures[src.diffuseTexture] : 0;
    dst.envmapTexture = (src.envmapTexture != ~0U)? scene.mNativeResources.textures[src.envmapTexture] : 0;

    dst.uOffset = src.uOffset;
    dst.vOffset = src.vOffset;
    dst.transparency = src.transparency;
    dst.dummy = 0;

    typedef mutalisk::data::shader_fixed  Op;
    switch(src.frameBufferOp)
    {
    case Op::fboReplace:
      dst.srcBlend = D3DBLEND_ONE;
      dst.dstBlend = D3DBLEND_ZERO;
      break;
    case Op::fboAdd:
      dst.srcBlend = D3DBLEND_ONE;
      dst.dstBlend = D3DBLEND_ONE;
      break;
    case Op::fboSub:
      ASSERT("Not supported");
      // not supported, do closest match instead (mul)
      dst.srcBlend = D3DBLEND_DESTCOLOR;
      dst.dstBlend = D3DBLEND_ZERO;
      break;
    case Op::fboLerp:
      dst.srcBlend = D3DBLEND_SRCALPHA;
      dst.dstBlend = D3DBLEND_INVSRCALPHA;
      break;
    case Op::fboMul:
      dst.srcBlend = D3DBLEND_DESTCOLOR;
      dst.dstBlend = D3DBLEND_ZERO;
      break;
    case Op::fboMadd:
      dst.srcBlend = D3DBLEND_SRCALPHA;
      dst.dstBlend = D3DBLEND_ONE;
      break;
    case Op::fboReject:
      dst.srcBlend = D3DBLEND_ZERO;
      dst.dstBlend = D3DBLEND_ONE;
      break;
    }

    toNative(dst.aux0, src.aux0);
  }

  void setProjection(RenderContext& rc, float fovy, float aspect)
  {
    if(gSettings.overrideCameraMethod == 1)
      D3DXMatrixPerspectiveFovLH(&rc.projMatrix, fovy * (D3DX_PI/180.0f), aspect, 0.5f, 40000.0f);
  }

  void setCameraMatrix(RenderContext& rc, D3DXMATRIX const& camera)
  {
    D3DXMATRIX view = camera;

    D3DXMATRIX sm;
    D3DXMatrixScaling(&sm, 1.0f, 1.0f, -1.0f);
    D3DXMatrixMultiply(&view, &sm, &view);

    D3DXMatrixInverse(&view, 0, &view);
    if(gSettings.overrideCameraMethod == 0)
      D3DXMatrixMultiply(&rc.viewMatrix, &view, &rc.viewMatrix);
    else if(gSettings.overrideCameraMethod == 1)
      rc.viewMatrix = view;
    D3DXMatrixMultiply(&rc.viewProjMatrix, &rc.viewMatrix, &rc.projMatrix);
  }

  void setWorldMatrix(MatrixT* dst, RenderContext const& rc, D3DXMATRIX world)
  {
    if(gSettings.forceIdentityActorsMatrix)
      D3DXMatrixIdentity(&world);

    D3DXMATRIX invWorld;
    D3DXMATRIX worldViewProj;

    D3DXMatrixInverse(&invWorld, 0, &world);
    D3DXMatrixMultiply(&worldViewProj, &world, &rc.viewProjMatrix);

    dst[BaseEffect::WorldMatrix] = world;
    dst[BaseEffect::ViewMatrix] = rc.viewMatrix;
    dst[BaseEffect::ProjMatrix] = rc.projMatrix;
    dst[BaseEffect::ViewProjMatrix] = rc.viewProjMatrix;
    dst[BaseEffect::WorldViewProjMatrix] = worldViewProj;
    dst[BaseEffect::InvWorldMatrix] = invWorld;
  }

  void render(RenderContext& rc, RenderableMesh const& mesh, unsigned subset = 0)
  {
    // @TBD: support for 'undefined' (~0) subset, whole mesh should be rendered
    rc.device->SetFVF(mesh.mBlueprint.fvfVertexDecl);
    DX_MSG("draw all subsets") =
      mesh.mNative->DrawSubset(subset);
  }
} // namespace


void update(Dx9RenderableScene& scene, float time)
{
  scene.update(time);
}

void process(Dx9RenderableScene& scene)
{
  scene.process();
}

typedef Dx9RenderableScene  RenderableSceneT;
typedef RenderContext    RenderContextT;
#include "Renderer.h"

void render(RenderContext& rc, Dx9RenderableScene const& scene, int maxActors)
{
  (void) maxActors;

  bool animatedActors = gSettings.forceAnimatedActors;
  bool animatedCamera = gSettings.forceAnimatedCamera;

  ASSERT(rc.defaultEffect);
  Vec3 cameraPos;

  {
    MatrixT nativeMatrix;
    if(scene.mBlueprint.defaultClipIndex == ~0U)
    {
      animatedActors = false;
      animatedCamera = false;
    }

    if(!animatedActors)
      for(size_t q = 0; q < scene.mBlueprint.actors.size(); ++q)
      {
        ASSERT(q >= 0 && q < scene.mState.actor2XformIndex.size());
        toNative(
          const_cast<Dx9RenderableScene&>(scene).mState.matrices[scene.mState.actor2XformIndex[q]],
          scene.mBlueprint.actors[q].worldMatrix.data);
      }

    if(!animatedCamera)
      for(size_t q = 0; q < scene.mBlueprint.cameras.size(); ++q)
      {
        ASSERT(q >= 0 && q < scene.mState.camera2XformIndex.size());
        toNative(
          const_cast<Dx9RenderableScene&>(scene).mState.matrices[scene.mState.camera2XformIndex[q]],
          scene.mBlueprint.cameras[q].worldMatrix.data);
      }

    if(scene.mBlueprint.defaultCameraIndex != ~0U)
    {
      unsigned int cameraIndex = scene.mBlueprint.defaultCameraIndex;
      if(animatedCamera)
      {
        ASSERT(cameraIndex >= 0 && cameraIndex < scene.mState.camera2XformIndex.size());
        toNative(nativeMatrix, scene.mState.matrices[scene.mState.camera2XformIndex[cameraIndex]]);
      }
      else
      {
        ASSERT(cameraIndex >= 0 && cameraIndex < scene.mBlueprint.cameras.size());
        toNative(nativeMatrix, scene.mBlueprint.cameras[cameraIndex].worldMatrix.data);
      }

      setProjection(rc, scene.mBlueprint.cameras[cameraIndex].fov, scene.mBlueprint.cameras[cameraIndex].aspect);
      setCameraMatrix(rc, nativeMatrix);
      getTranslation(cameraPos, nativeMatrix);
    }

    gContext.device = rc.device;
    gContext.uberShader = rc.defaultEffect;
  }

  static std::vector<InstanceInput> instanceInputs;
  static std::vector<BaseEffect::Input::Surface> surfaceInputs;
  static std::vector<RenderBlock> bgRenderBlocks, opaqueRenderBlocks, transparentRenderBlocks, fgRenderBlocks;

  instanceInputs.resize(0);
  surfaceInputs.resize(0);
  bgRenderBlocks.resize(0); fgRenderBlocks.resize(0);
  opaqueRenderBlocks.resize(0); transparentRenderBlocks.resize(0);

  static std::vector<mutalisk::data::scene::Actor const*> visibleActors;
  visibleActors.resize(0);

  RenderContext& camera = rc; // @TBD:
  findVisibleActors(camera, 0) (scene.mBlueprint.actors, visibleActors);
  blastInstanceInputs(scene, camera) (visibleActors, instanceInputs);
  blastSurfaceInputs(scene, 0) (visibleActors, surfaceInputs);
  blastRenderBlocks(scene, cameraPos) (visibleActors, bgRenderBlocks, opaqueRenderBlocks, transparentRenderBlocks, fgRenderBlocks);
  sortRenderBlocks()(transparentRenderBlocks);
  if(instanceInputs.empty() || surfaceInputs.empty())
  {
    ASSERT(visibleActors.empty());
  }
  else
  {
    BaseEffect::Input::BufferControl background;
    background.colorWriteEnable = true;
    background.alphaTestEnable = false;
    background.zWriteEnable = false;
    background.zReadEnable = true;
    background.zEqual = false;

    BaseEffect::Input::BufferControl opaque[2];
    opaque[0].colorWriteEnable = true;
    opaque[0].alphaTestEnable = false;
    opaque[0].zWriteEnable = true;
    opaque[0].zReadEnable = true;
    opaque[0].zEqual = false;
    // zpass
    opaque[1] = opaque[0];
    opaque[1].alphaTestEnable = true;
    opaque[1].colorWriteEnable = false;

    BaseEffect::Input::BufferControl transparent[2];
    transparent[0].colorWriteEnable = true;
    transparent[0].alphaTestEnable = false;
    transparent[0].zWriteEnable = false;
    transparent[0].zReadEnable = true;
    transparent[0].zEqual = false;
    // zpass
    transparent[1] = transparent[0];
    transparent[1].zEqual = true;

    BaseEffect::Input::BufferControl foreground;
    foreground.colorWriteEnable = true;
    foreground.alphaTestEnable = false;
    foreground.zWriteEnable = false;
    foreground.zReadEnable = false;
    foreground.zEqual = false;

    drawRenderBlocks draw(rc, scene,
      &instanceInputs[0], instanceInputs.size(), &surfaceInputs[0], surfaceInputs.size());

    draw(opaqueRenderBlocks,    opaque[0], opaque[1]);
    draw(bgRenderBlocks,      background, background);
    draw(transparentRenderBlocks,  transparent[0], transparent[1]);
    draw(fgRenderBlocks,      foreground, foreground);
  }

  if(gSettings.debugHierarchy)
  {
    static float radius = 0.1f;
    static ID3DXMesh* mesh = 0;
    if(!mesh)
      D3DXCreateSphere(rc.device, radius, 8, 8, &mesh, 0);
    for(size_t q = 0; q < scene.mState.matrices.size(); ++q)
    {
      D3DXMATRIX world;
      toNative(world, scene.mState.matrices[q]);

      D3DXMATRIX worldViewProj;
      D3DXMatrixMultiply(&worldViewProj, &world, &rc.viewProjMatrix);
      gContext.uberShader->SetMatrix("mWorld", &world);
      gContext.uberShader->SetMatrix("mWorldViewProjection", &worldViewProj);

      uint32_t passes = 0;
      gContext.uberShader->Begin(&passes, 0);
      gContext.uberShader->BeginPass(0);
      mesh->DrawSubset(0);
      gContext.uberShader->EndPass();
      gContext.uberShader->End();
    }
  }
}
////////////////////////////////////////////////

void CSkinnedAlgos::processSkinMesh(RenderableMesh& mesh, BoneMapT const& boneMap, CTransform::t_matrix const* matrices)
{
  ASSERT(mesh.mBlueprint.skinInfo);
  ASSERT(!boneMap.empty());

  D3DVERTEXELEMENT9 dx9Declaration[MAX_FVF_DECL_SIZE];
  D3DXDeclaratorFromFVF(mesh.mBlueprint.fvfVertexDecl, dx9Declaration);

  size_t positionsOffset = ~0U;
  size_t normalsOffset = ~0U;
  size_t weightsOffset = ~0U;
  size_t boneIndicesOffset = ~0U;

  for(size_t q = 0; q < MAX_FVF_DECL_SIZE && dx9Declaration[q].Stream != 0xff; ++q)
  {
    size_t offset = dx9Declaration[q].Offset;
    switch(dx9Declaration[q].Usage)
    {
    case D3DDECLUSAGE_POSITION:
      positionsOffset = offset;
      break;
    case D3DDECLUSAGE_BLENDWEIGHT:
      weightsOffset = offset;
      break;
    case D3DDECLUSAGE_BLENDINDICES:
      boneIndicesOffset = offset;
      break;
    case D3DDECLUSAGE_NORMAL:
      normalsOffset = offset;
      break;
    }
  }

  ASSERT(positionsOffset != ~0);
  ASSERT(normalsOffset != ~0);
  ASSERT(weightsOffset != ~0);
  ASSERT(boneIndicesOffset != ~0);

  uint8_t* dstRaw = 0;
  DX_MSG("lock vertex buffer for writing") = mesh.mNative->LockVertexBuffer(0, (void**)&dstRaw);
  uint8_t const* srcRaw = mesh.mBlueprint.vertexData;
  memcpy(dstRaw, srcRaw, mesh.mBlueprint.vertexDataSize);

  processSkinMesh(
    reinterpret_cast<Vec3 const*>(srcRaw + positionsOffset),
    reinterpret_cast<Vec3 const*>(srcRaw + normalsOffset),
    reinterpret_cast<float const*>(srcRaw + weightsOffset),
    reinterpret_cast<uint8_t const*>(srcRaw + boneIndicesOffset),

    reinterpret_cast<Vec3*>(dstRaw + positionsOffset),
    reinterpret_cast<Vec3*>(dstRaw + normalsOffset),

    mesh.mBlueprint.vertexStride,                      // srcVertexStride
    mesh.mBlueprint.vertexStride,                      // srcWeightStride
    mesh.mBlueprint.vertexStride,                      // srcBoneIndexStride
    mesh.mBlueprint.vertexStride,                      // dstVertexStride
    mesh.mBlueprint.vertexCount,

    *mesh.mBlueprint.skinInfo, boneMap, matrices);

  DX_MSG("unlock vertex buffer") = mesh.mNative->UnlockVertexBuffer();

}
} //namespace mutalisk
