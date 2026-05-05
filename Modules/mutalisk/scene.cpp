#include "scene.h"

namespace mutalisk { namespace data
{

// scene
//
scene::scene()
: meshIds(0)
, lights(0)
, cameras(0)
, defaultCameraIndex(~0U)
, actors(0)
, animCharId("")
, defaultClipIndex(~0U)
{
}

} // namespace data 
} // namespace mutalisk
