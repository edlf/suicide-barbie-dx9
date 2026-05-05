#include "DemoPlayer.h"

#include "Timeline.h"
#include "ScenePlayer.h"
#include "dx9ScenePlayer.h"

using namespace mutalisk;
namespace
{
  void splitFilename(std::string const& fullPath, std::string& path, std::string& fileName)
  {
    using namespace std;
    size_t offset0 = fullPath.find_last_of('/');
    size_t offset1 = fullPath.find_last_of('\\');

    size_t offset = max(offset0, offset1);
    if(offset == std::string::npos)
      offset = min(offset0, offset1);

    path = "";
    fileName = fullPath;
    if(offset == std::string::npos)
      return;

    ++offset;
    path = fullPath.substr(0, offset);
    fileName = fullPath.substr(offset);
  }
}

void BaseDemoPlayer::platformSetup(IDirect3DDevice9& device, ID3DXEffect& defaultEffect)
{
  renderContext.device = &device;
  renderContext.defaultEffect = &defaultEffect;
  D3DXMatrixIdentity(&renderContext.viewProjMatrix);
  D3DXMatrixIdentity(&renderContext.projMatrix);
}

BaseDemoPlayer::Scene const& BaseDemoPlayer::load(Scene& scene, std::string const& sceneName)
{
  std::string path, fileName;
  splitFilename(sceneName, path, fileName);
  scene.pathPrefix = path;
  setResourcePath(mPathPrefix + path);

  scene.blueprint = loadResource<mutalisk::data::scene>(fileName);
  scene.renderable = prepare(renderContext, *scene.blueprint).release();
  scene.startTime = -1.0f;
  scene.znear = 1.0f;
  scene.zfar = 50.0f;
  return scene;
}

void BaseDemoPlayer::restart(Scene const& scene)
{
  scene.startTime = -1.0f;
}

void BaseDemoPlayer::processJobQueue()
{
  for(size_t q = 0; q < mJobQueue.size(); ++q)
  {
    ASSERT(mJobQueue[q]);
    mJobQueue[q]->process();
    //delete mJobQueue[q];
  }
  mJobQueue.resize(0);
}

void BaseDemoPlayer::setPhase(nPhase phase)
{
  mPhase = phase;
}

void BaseDemoPlayer::setTime(float t)
{
  mCurrTime = t;
  mCurrFrame = timeToFrame(t);
}

void BaseDemoPlayer::setPath(std::string const& pathPrefix)
{
  mPathPrefix = pathPrefix;
}

 float BaseDemoPlayer::sceneTime(Scene const& scene)
{
  ASSERT(scene.startTime >= 0.0f);
  return time() - scene.startTime;
}

namespace {
void onDrawDefault(RenderableSceneT const& scene) {}
}
void BaseDemoPlayer::draw(Scene const& scene, float timeScale)
{
  draw(scene, &::onDrawDefault, timeScale);
}

/*
struct RenderJob : public BaseDemoPlayer::IJob
{
  BaseDemoPlayer::Scene const*  scene;
  RenderContextT*          renderContext;
  void process()
  {
    renderContext->znear = scene->znear;
    renderContext->zfar = scene->zfar;
    mutalisk::render(*renderContext, *scene->renderable);
  }
};
static const unsigned JobCacheSize = 8;
RenderJob gRenderJob[JobCacheSize];
unsigned gRenderJobIndex = 0;*/

void BaseDemoPlayer::draw(Scene const& scene, OnDrawT onDraw, float timeScale)
{
  if(mPhase == UpdatePhase)
  {
    if(scene.startTime <= 0.0f)
      scene.startTime = time();

    ASSERT(scene.renderable);
    mutalisk::update(*scene.renderable, (time() - scene.startTime) * timeScale);
    mutalisk::process(*scene.renderable);
    //renderContext.znear = scene.znear;
    //renderContext.zfar = scene.zfar;

    onDraw(*scene.renderable);
  }
  else if(mPhase == RenderPhase)
  {
    renderContext.znear = scene.znear;
    renderContext.zfar = scene.zfar;
    mutalisk::render(renderContext, *scene.renderable);

  /*
    RenderJob* job = &gRenderJob[gRenderJobIndex]; gRenderJobIndex = (gRenderJobIndex+1)%JobCacheSize;
    job->scene = &scene;
    job->renderContext = &renderContext;
    job->process();
  //  mJobQueue.push_back(job);*/
  }
}

void BaseDemoPlayer::clear()
{
  if(mPhase == UpdatePhase)
  { }
  else if(mPhase == RenderPhase)
  { }
}
/*
struct ClearZJob : public BaseDemoPlayer::IJob
{
  RenderContextT*          renderContext;
  void process()
  {
    ASSERT(renderContext);
    DX_MSG("Depth clear") =
      renderContext->device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f,0.0f,0.0f,0.0f), 1.0f, 0);
  }
};
ClearZJob gClearZJob[JobCacheSize];
unsigned gClearZJobIndex = 0;*/

void BaseDemoPlayer::clearZ()
{
  if(mPhase == UpdatePhase)
  {
  }
  else if(mPhase == RenderPhase)
  {
    DX_MSG("Depth clear") =
      renderContext.device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f,0.0f,0.0f,0.0f), 1.0f, 0);

  /*
    ClearZJob* job = &gClearZJob[gClearZJobIndex]; gClearZJobIndex = (gClearZJobIndex+1)%JobCacheSize;
    job->renderContext = &renderContext;
    job->process();
  //  mJobQueue.push_back(job);*/
  }
}

void BaseDemoPlayer::clearColor()
{
  if(mPhase == UpdatePhase)
  { }
  else if(mPhase == RenderPhase)
  { }
}

/*
struct BloomJob : public BaseDemoPlayer::IJob
{
  BaseDemoPlayer::PostProcessSettings* dstSettings;
  BaseDemoPlayer::PostProcessSettings srcSettings;
  void process()
  {
    ASSERT(dstSettings);
    *dstSettings = srcSettings;
  }
};
BloomJob gBloomJob[JobCacheSize];
unsigned gBloomJobIndex = 0;*/

void BaseDemoPlayer::ppBloom(float strength, unsigned threshold, unsigned srcModifier, unsigned dstModifier, unsigned quality)
{
  if(mPhase == UpdatePhase)
  {
  }
  else if(mPhase == RenderPhase)
  {
    mPPSettings.strength = strength;
    mPPSettings.threshold = threshold;
    mPPSettings.srcModifier = srcModifier;
    mPPSettings.dstModifier = dstModifier;
    mPPSettings.quality = quality;

  /*
    BloomJob* job = &gBloomJob[gBloomJobIndex]; gBloomJobIndex = (gBloomJobIndex+1)%JobCacheSize;
    job->dstSettings = &mPPSettings;
    job->srcSettings.strength = strength;
    job->srcSettings.threshold = threshold;
    job->srcSettings.srcModifier = srcModifier;
    job->srcSettings.dstModifier = dstModifier;
    job->srcSettings.quality = quality;
    job->process();
  //  mJobQueue.push_back(job);*/
  }
}
