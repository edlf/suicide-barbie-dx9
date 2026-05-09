#include <cmath>
#include "TestDemo.h"

// #include <Module/effects/library/Mirror.h>

#define S_FUNC(f) (&SelfT::f)

void TestDemo::updateFrame(float t)
{
  setTime(t + timeOffset);
  timeline.gather(*this, frame());
  setPhase(UpdatePhase);
  timeline.run(*this);
}

namespace
{
  void flashScreen(float intensity, unsigned color )
  {
    // TODO
  }

  void fadeScreen(float intensity)
  {
    // TODO
  }

  unsigned findActor(TestDemo::Scene const& scene, std::string const& actorName)
  {
    unsigned actorId = ~0U;
    for(size_t q = 0; q < scene.blueprint->actors.size(); ++q) {
      if(scene.blueprint->actors[q].nodeName.find(actorName) != std::string::npos) {
        actorId = q;
      }
    }
    return actorId;
  }

} // namespace

void TestDemo::renderFrame()
{
  // HACK: mirror
  frameContainsMirror = false;

  setPhase(RenderPhase);
  timeline.run(*this);

  /*
  {
  unsigned color;
  float val = getBlinkyValue(time(), color);
  if (val != 0.f)
  {
  if (val > 0.f)
  flashScreen(val, color);
  else
  fadeScreen(-val);

  // FlashScreenJob* job = new FlashScreenJob;
  // job->renderContext = &renderContext;
  // job->intensity = val;
  // job->color = color;
  // job->process();
  //mJobQueue.push_back(job);
  }
  }
  */
}

void TestDemo::drawMirrorFrame(Scene const& scene, unsigned mirrorActorId, unsigned reflectedActorId)
{
  if(mPhase == UpdatePhase)
  {
	  //
  }
  else if(mPhase == RenderPhase)
  {
    frameContainsMirror = true;

    CTransform::t_matrix cameraMatrix = scene.renderable->mState.cameraMatrix;
    CTransform::t_matrix prevCameraMatrix = cameraMatrix;

	for(size_t q = 0; q < scene.blueprint->actors.size(); ++q) {
		if(q != reflectedActorId) {
			scene.blueprint->actors[q].active = false;
		}
	}

    CTransform::t_matrix const& mirrorMatrix =
      scene.renderable->mState.matrices[
        scene.renderable->mState.actor2XformIndex[mirrorActorId]];

        Vec3 point = mirrorMatrix.Move;
        point.y -= 1.35f;

        Vec3 normal = mirrorMatrix.Rot.Row[0];
        normal.x = mirrorMatrix.Rot.Row[1].x;
        normal.y = mirrorMatrix.Rot.Row[1].y;
        normal.z = mirrorMatrix.Rot.Row[1].z;
        normal.x = -0.97789f;
        normal.y = 0.159117f;
        normal.z = 0.0f;
        Vec3_normalize(&normal, &normal);

        struct Plane { float a, b, c, d; };
        Plane P;
        P.a = normal.x;
        P.b = normal.y;
        P.c = normal.z;
        P.d = -(point.x * normal.x + point.y * normal.y + point.z * normal.z);
        float invPLen = 1.0f / sqrtf(P.a*P.a + P.b*P.b + P.c*P.c + P.d*P.d);
        P.a *= invPLen;
        P.b *= invPLen;
        P.c *= invPLen;
        P.d *= invPLen;

        Mat34 reflMatrix;


        reflMatrix.Rot.Row[0].x = -2 * P.a * P.a + 1;
        reflMatrix.Rot.Row[0].y = -2 * P.b * P.a;
        reflMatrix.Rot.Row[0].z = -2 * P.c * P.a;

        reflMatrix.Rot.Row[1].x = -2 * P.a * P.b;
        reflMatrix.Rot.Row[1].y = -2 * P.b * P.b + 1;
        reflMatrix.Rot.Row[1].z = -2 * P.c * P.b;

        reflMatrix.Rot.Row[2].x = -2 * P.a * P.c;
        reflMatrix.Rot.Row[2].y = -2 * P.b * P.c;
        reflMatrix.Rot.Row[2].z = -2 * P.c * P.c + 1;

        reflMatrix.Move.x = -2 * P.a * P.d;
        reflMatrix.Move.y = -2 * P.b * P.d;
        reflMatrix.Move.z = -2 * P.c * P.d;

        CTransform::t_matrix m = cameraMatrix;
        Mat34_mul(&m, &reflMatrix, &cameraMatrix);
        scene.renderable->mState.cameraMatrix = m;

        renderContext.znear = scene.znear;
        renderContext.zfar = scene.zfar;
        mutalisk::render(renderContext, *scene.renderable);

        scene.renderable->mState.cameraMatrix = prevCameraMatrix;
        for(size_t q = 0; q < scene.blueprint->actors.size(); ++q) {
          scene.blueprint->actors[q].active = true;
        }
  }
}

namespace {
  int ms(int v) {
    return static_cast<int>(floor(0.5f + static_cast<float>(v) * 0.3f));
  }
}

void TestDemo::onStart() {
  timeOffset = 0;

  {Item items[] = {
    Item(0,   ms(00),    S_FUNC(walk)),
    Item(11,  ms(00),    S_FUNC(walk_far)),

    Item(23,  ms(05),    S_FUNC(logo)),
    Item(32,  ms(75)/*ms(05)*/, S_FUNC(logo_x_flower)),
    Item(35,  ms(71),    S_FUNC(face_on_flower)),
    Item(48,  ms(80),    S_FUNC(flower)),

    Item(50,  ms(00),    S_FUNC(flower_x_spiral)),
    Item(55,  ms(00),    S_FUNC(spiral)),

    Item(61,  ms(46),    S_FUNC(phone1)),
    Item(68,  ms(64),    S_FUNC(phone1_x__)),

    Item(69,  ms(14),    S_FUNC(phone__x_2)),
    Item(69,  ms(64),    S_FUNC(phone2)),      // 14 + (114 - 64)
    //    Item(75,  ms(64),    S_FUNC(phone2_noMirror)),
    Item(76,  ms(32),    S_FUNC(phone2_x__)),

    Item(76,  ms(82),    S_FUNC(phone__x_3)),
    Item(77,  ms(32),    S_FUNC(phone3)),      // 82 + (82 - 32)
    Item(84,  ms(00),    S_FUNC(phone3_x__)),

    Item(84,  ms(50),    S_FUNC(phone__x_4)),
    Item(85,  ms(00),    S_FUNC(phone4)),      // (32 + 82)/2

    Item(92,   ms(18),    S_FUNC(text0)),
    Item(93,   ms(0),    S_FUNC(text)),
    Item(115,  ms(22),    S_FUNC(jealousy)),

    Item(122,     ms(90),    S_FUNC(beer1)),
    Item(129.75,  ms(71),    S_FUNC(beer2)),
    Item(137.75,  ms(51),    S_FUNC(garlic1)),
    Item(145.60,  ms(32),    S_FUNC(garlic2)),
    Item(153.50,  ms(13),    S_FUNC(mix1)),
    Item(164.25,  ms(13),    S_FUNC(mix2)),
    Item(169.25,  ms(13),    S_FUNC(mix3)),

    Item(184,  ms(34),    S_FUNC(reload)),
    Item(186,  ms(50),    S_FUNC(gun)),
    Item(196,  ms(94),    S_FUNC(m16)),
    Item(207,  ms(38),    S_FUNC(bullet1)),
    Item(217,  ms(06),    S_FUNC(explodeGirl1)),
    Item(225,  ms(35),    S_FUNC(bullet2)),
    Item(233,  ms(64),    S_FUNC(explodeGirl2)),
    Item(241,  ms(94),    S_FUNC(windowBarbie1)),
    Item(245,  ms(94),    S_FUNC(windowBarbie2)),
    //     Item(250,  ms(20),    S_FUNC(endBarbie0)),
    Item(250,  ms(20),    S_FUNC(endBarbie1)),
    Item(286,  ms(64),    S_FUNC(endBarbie2)),

    Item()
  };
  timeline.addScript(items);}

  bool doShowText = true;
  bool doShowGun = true;
  bool doShowExplode = true;
  bool doShowWindow = true;


  if(timeOffset >= 242)
  {
    goto __skipUntilWindow;
  }
  if(timeOffset >= 208)
  {
    doShowWindow = false;
    goto __skipUntilExpode;
  }
  if(timeOffset >= 184)
  {
    doShowExplode = false;
    doShowWindow = false;
    goto __skipUntilGun;
  }
  if(timeOffset >= 93)
  {
    doShowGun = false;
    doShowExplode = false;
    doShowWindow = false;
    goto __skipUntilText;
  }
  if(timeOffset >= 63)
  {
    doShowText = false;
    doShowGun = false;
    doShowExplode = false;
    doShowWindow = false;
    goto __skipUntilPhone;
  }

  load(scn.walk,    "walk\\dx9\\walk.msk");
  load(scn.walkBG,  "walk\\dx9\\back.msk");
  load(scn.logo,    "logo\\dx9\\logo.msk");

  load(scn.flower,  "flower\\dx9\\flower.msk");
  load(scn.face,    "head\\dx9\\head.msk");
  // prepareBalls(*scn.face.renderable);

  load(scn.spiral,  "snake\\dx9\\snake.msk");

__skipUntilPhone:
  load(scn.phone1,  "telephone_s1\\dx9\\telephone_s1.msk");
  load(scn.phone2,  "telephone_s2\\dx9\\telephone_s2.msk");
  load(scn.phone3,  "telephone_s3\\dx9\\telephone_s3.msk");
  load(scn.phone4,  "telephone_s4\\dx9\\telephone_s4.msk");
  load(scn.phoneTrans,"telephone_trans\\dx9\\telephone_trans.msk");
  // prepareSprites(*scn.phone1.renderable);
  // prepareSprites(*scn.phone2.renderable);
  // prepareSprites(*scn.phone3.renderable);

  phone2MirrorActorId = findActor(scn.phone2, "mirror");
  phone2ReflectorActorId = findActor(scn.phone2, "dfs");

__skipUntilText:
  if(doShowText)
  {
    load(scn.textWalk,  "text\\dx9\\text.msk");
    // prepareChars(*scn.textWalk.renderable);

    load(scn.textBG,  "text\\dx9\\back.msk");
    load(scn.text,    "text\\dx9\\undertext.msk");
    load(scn.jealousy,  "jealousy\\dx9\\jealousy.msk");

    load(scn.beer1,    "beer\\beer1\\dx9\\beer1.msk");
    load(scn.beer2,    "beer\\beer2\\dx9\\beer2.msk");

    load(scn.garlic1,  "garlic\\garlic1\\dx9\\garlic1.msk");
    load(scn.garlic2,  "garlic\\garlic2\\dx9\\garlic2.msk");

    load(scn.mix1,    "mix\\mix1\\dx9\\mix1.msk");
    load(scn.mix2,    "mix\\mix2\\dx9\\mix2.msk");
    load(scn.mix3,    "mix\\mix3\\dx9\\mix3.msk");
  }

__skipUntilGun:
  if(doShowGun)
  {
    load(scn.reload,  "reload\\dx9\\reload.msk");
    load(scn.m16,    "weapon3\\dx9\\weapon3.msk");
    load(scn.gun,    "weapon2\\dx9\\gun.msk");
  }

__skipUntilExpode:
  if(doShowExplode)
  {
    load(scn.bullet1,  "bull1\\dx9\\bull1.msk");
    load(scn.bullet2,  "bull2\\dx9\\bull2.msk");
    load(scn.expGirl1BG,"back_01\\dx9\\back_01.msk");
    load(scn.expGirl2BG,"back_02\\dx9\\back_02.msk");
    load(scn.expGirl1,  "exgirl1\\dx9\\exgirl1.msk");
    load(scn.expGirl2,  "exgirl2\\dx9\\exgirl2.msk");
  }

__skipUntilWindow:
  if(doShowWindow)
  {
    load(scn.windowBarbie, "suicidebarbie1\\dx9\\suicidebarbie1.msk");
    load(scn.window,  "window\\dx9\\window.msk");
    // TODO: smth wrong with this msk file, doesnt generate output for dx9
    // load(scn.endBack,  "suicidebarbie2\\dx9\\suicidebarbie_back2.msk");
    load(scn.end,    "suicidebarbie2\\dx9\\suicidebarbie2.msk");
  }

  if(timeOffset > 0)
  {
    timeline.jump(*this, mutalisk::timeToFrame(timeOffset));
  }
}

namespace {
  float gVScale = 1.0f;
  void updateAnimatedVisibility(mutalisk::RenderableSceneT const& scene)
  {
    const mutalisk::array<mutalisk::data::scene::Actor>& actors = scene.mBlueprint.actors;
    float time = scene.mState.time;

    // update properties
    for(size_t q = 0; q < actors.size(); ++q)
    {
      mutalisk::data::scene::Actor& actor = const_cast<mutalisk::data::scene::Actor&>(actors[q]);
      actor.active = (scene.mState.sampleAnimation(actor.nodeName, "Visibility", time, 1.0f) > 0.0f);
    }
  }

  void updateAnimatedProperties(mutalisk::RenderableSceneT const& scene)
  {
    float vScale = gVScale;

    const mutalisk::array<mutalisk::data::scene::Actor>& actors = scene.mBlueprint.actors;
    float time = scene.mState.time;

    // update properties
    for(size_t q = 0; q < actors.size(); ++q)
    {
      mutalisk::data::scene::Actor& actor = const_cast<mutalisk::data::scene::Actor&>(actors[q]);

      bool hasUVScroll = scene.mState.hasAnimation(actor.nodeName, "UVScroll");
      float v = scene.mState.sampleAnimation(actor.nodeName, "UVScroll", time);
      float fadeOut = scene.mState.sampleAnimation(actor.nodeName, "Fadeout", time, 0.0f);
      float fadeIn = scene.mState.sampleAnimation(actor.nodeName, "Fadein", time, 1.0f);
      for(size_t w = 0; w < actor.materials.size(); ++w)
      {
        if(hasUVScroll)
        {
          actor.materials[w].shaderInput.vOffset = 1.0f - v*(vScale + 1.0f);
          actor.materials[w].shaderInput.vScale = vScale;
        }
        actor.materials[w].shaderInput.transparency = 1.0f - ((1.0f - fadeOut) * fadeIn);
      }
      //actor.active = (scene.mState.sampleAnimation(actor.nodeName, "Fadein", time+1.0f, 1.0f) > 0.0f);
    }
  }

  void updateAnimatedProperties2(mutalisk::RenderableSceneT const& scene)
  {
    float vScale = gVScale;

    const mutalisk::array<mutalisk::data::scene::Actor>& actors = scene.mBlueprint.actors;
    float time = scene.mState.time;

    // update properties
    for(size_t q = 0; q < actors.size(); ++q)
    {
      mutalisk::data::scene::Actor& actor = const_cast<mutalisk::data::scene::Actor&>(actors[q]);

      bool hasUVScroll = scene.mState.hasAnimation(actor.nodeName, "UVScroll");
      float v = scene.mState.sampleAnimation(actor.nodeName, "UVScroll", time);
      float fadeOut = scene.mState.sampleAnimation(actor.nodeName, "Fadeout", time, 0.0f);
      float fadeIn = scene.mState.sampleAnimation(actor.nodeName, "Fadein", time, 1.0f);
      for(size_t w = 0; w < actor.materials.size(); ++w)
      {
        if(hasUVScroll)
        {
          actor.materials[w].shaderInput.vOffset = (vScale + 1.0f) - v*(vScale + 1.0f);//- v*2.0f;
          actor.materials[w].shaderInput.vScale = -vScale;
          //actor.materials[w].shaderInput.vOffset = 1.0f - v*(vScale + 1.0f);
          //actor.materials[w].shaderInput.vScale = vScale;
        }
        actor.materials[w].shaderInput.transparency = 1.0f - ((1.0f - fadeOut) * fadeIn);
      }
      //actor.active = (scene.mState.sampleAnimation(actor.nodeName, "Fadein", time+1.0f, 1.0f) > 0.0f);
    }
  }
  void updateAnimatedProperties3(mutalisk::RenderableSceneT const& scene)
  {
    const mutalisk::array<mutalisk::data::scene::Actor>& actors = scene.mBlueprint.actors;
    float time = scene.mState.time;

    // update properties
    for(size_t q = 0; q < actors.size(); ++q)
    {
      float f = min(time * 0.4f, 1.0f);

      mutalisk::data::scene::Actor& actor = const_cast<mutalisk::data::scene::Actor&>(actors[q]);
      for(size_t w = 0; w < actor.materials.size(); ++w)
      {
        actor.materials[w].shaderInput.diffuse.r = f;
        actor.materials[w].shaderInput.diffuse.g = f;
        actor.materials[w].shaderInput.diffuse.b = f;
      }
    }
  }

  void updateAnimatedProperties4(mutalisk::RenderableSceneT const& scene)
  {
    const mutalisk::array<mutalisk::data::scene::Actor>& actors = scene.mBlueprint.actors;
    float time = scene.mState.time;

    // update properties
    for(size_t q = 0; q < actors.size(); ++q)
    {
      float f = max(min(8.0f - time, 1.0f), 0.0f);

      mutalisk::data::scene::Actor& actor = const_cast<mutalisk::data::scene::Actor&>(actors[q]);
      for(size_t w = 0; w < actor.materials.size(); ++w)
      {
        actor.materials[w].shaderInput.diffuse.r = f;
        actor.materials[w].shaderInput.diffuse.g = f;
        actor.materials[w].shaderInput.diffuse.b = f;
      }
    }
  }

  void updateAnimatedProperties4_short(mutalisk::RenderableSceneT const& scene)
  {
    const mutalisk::array<mutalisk::data::scene::Actor>& actors = scene.mBlueprint.actors;
    float time = scene.mState.time;

    // update properties
    for(size_t q = 0; q < actors.size(); ++q)
    {
		float f = max(min((8.5f - time)*2.0f, 1.0f), 0.0f);

		mutalisk::data::scene::Actor& actor = const_cast<mutalisk::data::scene::Actor&>(actors[q]);
		for(size_t w = 0; w < actor.materials.size(); ++w) {
			actor.materials[w].shaderInput.emissive.r = f;
			actor.materials[w].shaderInput.emissive.g = f;
			actor.materials[w].shaderInput.emissive.b = f;
			actor.materials[w].shaderInput.ambient.r = f;
			actor.materials[w].shaderInput.ambient.g = f;
			actor.materials[w].shaderInput.ambient.b = f;
		}
    }
  }

  void updateAnimatedProperties5(mutalisk::RenderableScene const& scene)
  {
    const mutalisk::array<mutalisk::data::scene::Actor>& actors = scene.mBlueprint.actors;
    float time = scene.mState.time;

    // update properties
    for(size_t q = 0; q < actors.size(); ++q)
    {
      mutalisk::data::scene::Actor& actor = const_cast<mutalisk::data::scene::Actor&>(actors[q]);

      float fadeIn = scene.mState.sampleAnimation(actor.nodeName, "fadein", time, 1.0f);
      for(size_t w = 0; w < actor.materials.size(); ++w)
      {
        actor.materials[w].shaderInput.transparency = 1.0f - fadeIn;
      }
    }
  }

  void updateAnimatedProperties_sprites(mutalisk::RenderableSceneT const& scene)
  {
    updateAnimatedProperties5(scene);
    // mutalisk::renderSprites(scene);
  }

}

void TestDemo::walk()
{
  //  printf("TestDemo::walk\n");

  scn.walk.znear = 1;
  scn.walk.zfar = 15;

  draw(scn.walkBG);
  clearZ();
  draw(scn.walk);

  //  ppBloom(0.1f, 0, 200, 160, 2);
  ppBloom(0);
}
void TestDemo::walk_far()
{
  //  printf("TestDemo::walk_far\n");

  scn.walk.znear = 3;
  scn.walk.zfar = 8;

  draw(scn.walkBG);
  clearZ();
  draw(scn.walk);

  //  ppBloom(0.1f, 0, 200, 160, 2);
  ppBloom(0);
}

void TestDemo::logo()
{
  gVScale = 3.0f;
  draw(scn.logo, updateAnimatedProperties);
  ppBloom(0);
}

void TestDemo::logo_x_flower()
{
  gVScale = 3.0f;
  draw(scn.flower);
  clearZ();
  draw(scn.logo, updateAnimatedProperties);
}

void TestDemo::face_on_flower_w_logo()
{
  gVScale = 3.0f;
  draw(scn.flower);
  // draw(scn.face, &mutalisk::renderBalls);
  clearZ();
  draw(scn.logo, updateAnimatedProperties);
}

void TestDemo::face_on_flower()
{
  draw(scn.flower);
  // draw(scn.face, &mutalisk::renderBalls);
}

void TestDemo::flower()
{
  draw(scn.flower);
}

void TestDemo::flower_x_spiral()
{
  draw(scn.spiral, updateAnimatedProperties);
  clearZ();
  draw(scn.flower);
}

void TestDemo::spiral()
{
  draw(scn.spiral, updateAnimatedProperties);
}

void TestDemo::phone1()
{
  draw(scn.phone1, updateAnimatedProperties_sprites);//&mutalisk::renderSprites);
  ppBloom(0.2f, 114, 200, 160);
}
void TestDemo::phone2()
{
  draw(scn.phone2, updateAnimatedProperties_sprites);//&mutalisk::renderSprites);
  drawMirrorFrame(scn.phone2, phone2MirrorActorId, phone2ReflectorActorId);
  ppBloom(0.2f, 114, 200, 160);
  restart(scn.phoneTrans);
}

void TestDemo::phone3()
{
  draw(scn.phone3, updateAnimatedProperties_sprites);//&mutalisk::renderSprites);
  ppBloom(0.2f, 114, 200, 160);
  restart(scn.phoneTrans);
}
void TestDemo::phone4()
{
  draw(scn.phone4);
  ppBloom(0.2f, 114, 200, 160);
  restart(scn.phoneTrans);
}

void TestDemo::phone1_x__()
{
  draw(scn.phone1, updateAnimatedProperties_sprites);
  clearZ();
  draw(scn.phoneTrans);
}
void TestDemo::phone2_x__()
{
  draw(scn.phone2, updateAnimatedProperties_sprites);
  clearZ();
  draw(scn.phoneTrans);
}
void TestDemo::phone3_x__()
{
  draw(scn.phone3, updateAnimatedProperties_sprites);
  clearZ();
  draw(scn.phoneTrans);
}

void TestDemo::phone__x_2()
{
  draw(scn.phone2, updateAnimatedProperties_sprites);
  clearZ();
  draw(scn.phoneTrans);
}
void TestDemo::phone__x_3()
{
  draw(scn.phone3, updateAnimatedProperties_sprites);
  clearZ();
  draw(scn.phoneTrans);
}
void TestDemo::phone__x_4()
{
  draw(scn.phone4);
  clearZ();
  draw(scn.phoneTrans);
}


void TestDemo::text0()
{
  draw(scn.textBG);
  clearZ();
  draw(scn.text);
  clearZ();
  // draw(scn.textWalk, &mutalisk::renderChars);
  ppBloom(0.2f, 114, 200, 160);
}

void TestDemo::text()
{
  draw(scn.textBG);
  clearZ();
  draw(scn.text);
  clearZ();
  // draw(scn.textWalk, &mutalisk::renderChars);
  //  ppBloom(0.2f, 114, 200, 160);
  ppBloom(0);
}

void TestDemo::jealousy()
{
  gVScale = 3.0f;
  draw(scn.jealousy, updateAnimatedProperties);
  ppBloom(0.2f, 114, 200, 160);
}

void TestDemo::beer1()
{
  draw(scn.beer1);
  ppBloom(0.9f, 1, 212, 114);
}
void TestDemo::beer2()
{
  draw(scn.beer2);
  ppBloom(0.9f, 1, 212, 114);
}
void TestDemo::garlic1()
{
  draw(scn.garlic1);
  ppBloom(0.9f, 1, 212, 114);
}
void TestDemo::garlic2()
{
  draw(scn.garlic2);
  ppBloom(0.9f, 1, 212, 114);
}
void TestDemo::mix1()
{
  draw(scn.mix1);
  ppBloom(0.9f, 1, 212, 114);
}
void TestDemo::mix2()
{
  draw(scn.mix2, updateAnimatedVisibility);
  ppBloom(0.9f, 1, 212, 114);
}
void TestDemo::mix3()
{
  draw(scn.mix3);
  ppBloom(0.9f, 1, 212, 114);
}

void TestDemo::reload()
{
  scn.reload.znear = 0.1;
  scn.reload.zfar = 8;

  draw(scn.reload);
  ppBloom(0);
}
void TestDemo::m16()
{
  gVScale = 20.0f;
  draw(scn.m16, updateAnimatedProperties2);
  ppBloom(0);
}
void TestDemo::gun()
{
  gVScale = 20.0f;
  scn.gun.zfar = 75;
  draw(scn.gun, updateAnimatedProperties2);
  ppBloom(0);
}
void TestDemo::bullet1()
{
  gVScale = 3.0f;//3.0f;
  draw(scn.bullet1, updateAnimatedProperties);
  ppBloom(0);
}
void TestDemo::bullet2()
{
  gVScale = 3.0f;
  scn.bullet2.zfar = 100;
  draw(scn.bullet2, updateAnimatedProperties);
  ppBloom(0);
  restart(scn.expGirl1);
}
void TestDemo::explodeGirl1()
{
  draw(scn.expGirl1BG);
  clearZ();
  draw(scn.expGirl1, updateAnimatedVisibility);
  ppBloom(0);
}
void TestDemo::explodeGirl2()
{
  draw(scn.expGirl2BG);
  clearZ();
  draw(scn.expGirl2, updateAnimatedVisibility);
  ppBloom(0);

  restart(scn.window);
  restart(scn.windowBarbie);
}
void TestDemo::windowBarbie1()
{
  draw(scn.windowBarbie, updateAnimatedProperties3);
  ppBloom(0.2f, 114, 200, 160);
}
void TestDemo::windowBarbie2()
{
  draw(scn.windowBarbie);
  draw(scn.windowBarbie, updateAnimatedProperties4);
  clearZ();
  draw(scn.window, updateAnimatedProperties4_short, 2.1f);//2.5f);
  ppBloom(0.2f, 114, 200, 160);
}

void TestDemo::endBarbie0()
{
  restart(scn.window);
  draw(scn.window, 2.5f);
  ppBloom(0.2f, 114, 200, 160);
}

void TestDemo::endBarbie1()
{
  draw(scn.endBack);
  clearZ();
  draw(scn.end);
  ppBloom(0.2f, 114, 200, 160);
}

void TestDemo::endBarbie2()
{
  draw(scn.endBack);
  ppBloom(0.2f, 114, 200, 160);
}

void TestDemo::loadExploScenes()
{
  scn.phone1.renderable->mResources.animCharSet.reset();
  scn.phone2.renderable->mResources.animCharSet.reset();
  scn.phone3.renderable->mResources.animCharSet.reset();
  scn.phone4.renderable->mResources.animCharSet.reset();
  scn.phoneTrans.renderable->mResources.animCharSet.reset();
  scn.phone1.renderable->mResources.meshes.resize(0);
  scn.phone2.renderable->mResources.meshes.resize(0);
  scn.phone3.renderable->mResources.meshes.resize(0);
  scn.phone4.renderable->mResources.meshes.resize(0);
  scn.phoneTrans.renderable->mResources.meshes.resize(0);

  scn.textBG.renderable->mResources.animCharSet.reset();
  scn.text.renderable->mResources.animCharSet.reset();
  scn.jealousy.renderable->mResources.animCharSet.reset();
  scn.beer1.renderable->mResources.animCharSet.reset();
  scn.beer2.renderable->mResources.animCharSet.reset();
  scn.garlic1.renderable->mResources.animCharSet.reset();
  scn.garlic2.renderable->mResources.animCharSet.reset();

  scn.textBG.renderable->mResources.meshes.resize(0);
  scn.text.renderable->mResources.meshes.resize(0);
  scn.jealousy.renderable->mResources.meshes.resize(0);
  scn.beer1.renderable->mResources.meshes.resize(0);
  scn.beer2.renderable->mResources.meshes.resize(0);
  scn.garlic1.renderable->mResources.meshes.resize(0);
  scn.garlic2.renderable->mResources.meshes.resize(0);

  load(scn.bullet1,  "bull1\\dx9\\bull1.msk");
  load(scn.bullet2,  "bull2\\dx9\\bull2.msk");
  load(scn.expGirl1BG,"back_01\\dx9\\back_01.msk");
  load(scn.expGirl2BG,"back_02\\dx9\\back_02.msk");
  load(scn.expGirl1,  "exgirl1\\dx9\\exgirl1.msk");
  load(scn.expGirl2,  "exgirl2\\dx9\\exgirl2.msk");
}

void TestDemo::loadWindowScenes()
{
  scn.mix1.renderable->mResources.animCharSet.reset();
  scn.mix2.renderable->mResources.animCharSet.reset();
  scn.mix3.renderable->mResources.animCharSet.reset();
  scn.reload.renderable->mResources.animCharSet.reset();
  scn.m16.renderable->mResources.animCharSet.reset();
  scn.gun.renderable->mResources.animCharSet.reset();

  scn.mix1.renderable->mResources.meshes.resize(0);
  scn.mix2.renderable->mResources.meshes.resize(0);
  scn.mix3.renderable->mResources.meshes.resize(0);
  scn.reload.renderable->mResources.meshes.resize(0);
  scn.m16.renderable->mResources.meshes.resize(0);
  scn.gun.renderable->mResources.meshes.resize(0);

  //  scn.bullet1.renderable->mResources.animCharSet.reset();
  //  scn.expGirl1BG.renderable->mResources.animCharSet.reset();
  //  scn.expGirl1.renderable->mResources.animCharSet.reset();
  //  scn.bullet2.renderable->mResources.animCharSet.reset();

  load(scn.windowBarbie, "suicidebarbie1\\dx9\\suicidebarbie1.msk");
  load(scn.window, "window\\dx9\\window.msk");
}

void TestDemo::loadEndScenes() {
  scn.bullet1.renderable->mResources.animCharSet.reset();
  scn.expGirl1BG.renderable->mResources.animCharSet.reset();
  scn.expGirl1.renderable->mResources.animCharSet.reset();
  scn.bullet2.renderable->mResources.animCharSet.reset();

  scn.expGirl1BG.renderable->mResources.meshes.resize(0);
  scn.expGirl1.renderable->mResources.meshes.resize(0);
  scn.bullet2.renderable->mResources.meshes.resize(0);

  // TODO:
  // load(scn.endBack,  "suicidebarbie2\\dx9\\suicidebarbie_back2.msk");
  load(scn.end,  "suicidebarbie2\\dx9\\suicidebarbie2.msk");
}

void TestDemo::loadEnd()
{
  //  scn.expGirl2BG.renderable->mResources.animCharSet.reset();
  //  scn.expGirl2.renderable->mResources.animCharSet.reset();

  //  scn.expGirl2BG.renderable->mResources.meshes.resize(0);
  //  scn.expGirl2.renderable->mResources.meshes.resize(0);
}

void TestDemo::quitDemo() {
}

std::wstring TestDemo::getSceneName() {
  return std::wstring(L"TestDemo: TODO");
}