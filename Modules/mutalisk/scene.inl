#include <Modules/mutant/mutant.h>

using namespace mutant;

namespace mutalisk { namespace data
{

// scene::Node
//
template <typename In> In& operator>> (In& i, scene::Node& data)
{
  try
  {
    data.id = i.readDword();
    i.readString(data.nodeName);
    i.readType(data.worldMatrix);
    i.readType(data.active);
  } catch( EIoEof& ) {
    mutant_throw( "Unexpected end-of-file (file may be corrupted)" );
  } catch( EIoError& ) {
    mutant_throw( "Read/write error" );
  }
  return i;
}

template <typename Out> Out& operator<< (Out& o, scene::Node const& data)
{
  try
  {
    o.writeDword(data.id);
    o.writeString(data.nodeName);
    o.writeType(data.worldMatrix);
    o.writeType(data.active);
  } catch( EIoEof& ) {
    mutant_throw( "Unexpected end-of-file (file may be corrupted)" );
  } catch( EIoError& ) {
    mutant_throw( "Read/write error" );
  }
  return o;
}

// scene
//
template <typename In> In& operator>> (In& i, scene& data)
{
  try
  {
    unsigned versionCheck = (data.Version == i.readDword());
    ASSERT(versionCheck);

    // meshes
    data.meshIds.resize(i.readDword());
    std::cout << "Mesh count: " << data.meshIds.size() << std::endl;
    i.readArray(data.meshIds.begin(), data.meshIds.end());

    for (size_t k = 0; k < data.meshIds.size(); k++) {
      std::cout << "Mesh: " << data.meshIds[k] << std::endl;
    }

    // textures
    data.textureIds.resize(i.readDword());
    std::cout << "Texture count: " << data.textureIds.size() << std::endl;
    i.readArray(data.textureIds.begin(), data.textureIds.end());

    for (size_t k = 0; k < data.textureIds.size(); k++) {
      std::cout << "Texture: " << data.textureIds[k] << std::endl;
    }

    // shaders
    // data.shaderInputs.resize(i.readDword());
    // i.readArray(data.shaderInputs.begin(), data.shaderInputs.end());
    data.shaderLibraryVersion = i.readDword();

    // lights
    data.lights.resize(i.readDword());
    std::cout << "Light count: " << data.lights.size() << std::endl;
    for(size_t q = 0; q < data.lights.size(); ++q)
    {
      i >> data.lights[q].base();
      i.readType(data.lights[q].type);
      i.readType(data.lights[q].ambient);
      i.readType(data.lights[q].diffuse);
      i.readType(data.lights[q].specular);
      i.readType(data.lights[q].diffuseAux0);
      i.readType(data.lights[q].diffuseAux1);
      i.readType(data.lights[q].attenuation);
      i.readType(data.lights[q].theta);
      i.readType(data.lights[q].phi);
    }

    // cameras
    data.cameras.resize(i.readDword());
    for(size_t q = 0; q < data.cameras.size(); ++q)
    {
      i >> data.cameras[q].base();
      i.readType(data.cameras[q].background);
      i.readType(data.cameras[q].fov);
      i.readType(data.cameras[q].aspect);
    }
    data.defaultCameraIndex = i.readDword();

    std::cout << "Camera count: " << data.cameras.size() << ", default " << data.defaultCameraIndex << std::endl;

    // actors
    data.actors.resize(i.readDword());
    std::cout << "Actor count: " << data.actors.size() << std::endl;
    for(size_t q = 0; q < data.actors.size(); ++q)
    {
      i >> data.actors[q].base();
      data.actors[q].meshIndex = i.readDword();
      const int materials = i.readDword();
      data.actors[q].materials.resize(materials);

      size_t sss = data.actors[q].materials.size();
      std::cout << "Material count: " << sss << std::endl;

      for(size_t w = 0; w < data.actors[q].materials.size(); ++w)
      {
        data.actors[q].materials[w].shaderIndex = i.readDword();
        i >> data.actors[q].materials[w].shaderInput;
        // data.actors[q].materials[w].inputIndex = i.readDword();
      }
      data.actors[q].slice = i.readDword();
    }

    // clips
    i.readString( data.animCharId );
    data.defaultClipIndex = i.readDword();

  } catch(EIoEof&) {
    mutant_throw( "Unexpected end-of-file (file may be corrupted)" );
  } catch(EIoError&) {
    mutant_throw( "Read/write error" );
  }
  return i;
}

template <typename Out> Out& operator<< (Out& o, scene const& data)
{
  try
  {
    o.writeDword(data.Version);

    // meshes
    o.writeDword(data.meshIds.size());
    o.writeData(data.meshIds.begin(), data.meshIds.end());

    // textures
    o.writeDword(data.textureIds.size());
    o.writeData(data.textureIds.begin(), data.textureIds.end());

    // shaders
    // o.writeDword(data.shaderInputs.size());
    // o.writeData(data.shaderInputs.begin(), data.shaderInputs.end());
    o.writeDword(data.shaderLibraryVersion);

    // lights
    o.writeDword(data.lights.size());
    for( size_t q = 0; q < data.lights.size(); ++q )
    {
      o << data.lights[q].base();
      o.writeType(data.lights[q].type);
      o.writeType(data.lights[q].ambient);
      o.writeType(data.lights[q].diffuse);
      o.writeType(data.lights[q].specular);
      o.writeType(data.lights[q].diffuseAux0);
      o.writeType(data.lights[q].diffuseAux1);
      o.writeType(data.lights[q].attenuation);
      o.writeType(data.lights[q].theta);
      o.writeType(data.lights[q].phi);
    }

    // cameras
    o.writeDword(data.cameras.size());
    for(size_t q = 0; q < data.cameras.size(); ++q)
    {
      o << data.cameras[q].base();
      o.writeType(data.cameras[q].background);
      o.writeType(data.cameras[q].fov);
      o.writeType(data.cameras[q].aspect);
    }
    o.writeDword(data.defaultCameraIndex);

    // actors
    o.writeDword(data.actors.size());
    for(size_t q = 0; q < data.actors.size(); ++q)
    {
      o << data.actors[q].base();
      o.writeDword(data.actors[q].meshIndex);
      o.writeDword(data.actors[q].materials.size());
      for(size_t w = 0; w < data.actors[q].materials.size(); ++w)
      {
        o.writeDword(data.actors[q].materials[w].shaderIndex);
        //o.writeDword(data.actors[q].materials[w].inputIndex);
        o << data.actors[q].materials[w].shaderInput;
      }
      o.writeDword(data.actors[q].slice);
    }

    // clips
    o.writeString(data.animCharId);
    o.writeDword(data.defaultClipIndex);

  } catch( EIoEof& ) {
    mutant_throw( "Unexpected end-of-file (file may be corrupted)" );
  } catch( EIoError& ) {
    mutant_throw( "Read/write error" );
  }
  return o;
}

} // namespace data
} // namespace mutalisk
