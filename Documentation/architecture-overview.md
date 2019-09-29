# Architecture overview

## Module architecture

GXEngine has module architecture. Each core module is singleton.

* `GXCore` (_TODO: insert reference_)
* `GXInput` (_TODO: insert reference_)
* `GXRender` (_TODO: insert reference_)
* `GXPhysics` (_TODO: insert reference_)
* `GXSoundMixer` (_TODO: insert reference_)
* `GXNetwork` (_TODO: insert reference_)

3rd party libraries are isolated from engine code via satelite library: _GXEngine_. This library is responsible for native initialization:

* _FreeType_
* _Vorbis|Ogg_
* _OpenAL_
* _XInput_
* _PhysX 3.3.4_ (_deprecated_)

Application code must be placed in `GXGame` successor class and send to `GXCore` object:

```
// version 1.2

#include <GXEngine/GXCrashReport.h>
#include <GXEngine/GXCore.h>
#include <GXEngine_Editor_Mobile/EMGame.h>


int wmain ( int /*argc*/, LPWSTR /*commandline*/ )
{
    GXCrashReport crashReport;

    GXCore& core = GXCore::GetInstance ();
    EMGame* game = new EMGame ();
    core.Start ( *game );

    delete &core;
    return 0;
}
```


`GXCore::Start` method starts the application.

## Directory structure

There are predefined directories with project resources:

* Animations
* Config
* Fonts
* Locale
* Meshes
* Shaders
* Skeletons
* Skins
* Sounds
* Textures

`Animations` directory holds skeletal mesh animations. Animation file has proprietary format. GXEngine repo contains source code for building native plugin for _3ds Studio Max 2018_. This plugin allows user to export animations to GXEngine format. GXEngine animation engine supports up to `4` bones per vertex, up to `80` bones per mesh with maximum bone name up to `64` bytes. Bone name has `UTF-8` encoding.

`Config` directory contains configuration files for GXEngine deploy.

`Fonts` directory has font files in `.ttf` format.

`Meshes` directory contains mesh geometry files. Mesh geometry files could have formats:

* `.obj`
* `.stm`
* `.skm`
* `.mesh`

`.obj` must have triangle mesh format, _normal_ data and _UV_ data. Material sections is ignored. This file format is the most portable. So GXEngine has loader for this format.
`.stm` is proprietary (but deprecated) format for static meshes.
`.skm` is proprietary (but deprecated) format for skeletal meshes.
`.mesh` is proprietary (recommended) format for skeletal meshes. GXEngine repo contains source code for building native plugin for _3ds Studio Max 2018_. This plugin allows user to export meshes to GXEngine format.

`Shaders` directory contains source shader code and precompiled shader code. Current implementation supports shaders for OpenGL 3.3 core profile (GLSL).

`Skeletons` directory contains skeleton information for skeletal meshes. GXEngine repo contains source code for building native plugin for _3ds Studio Max 2018_. This plugin allows user to export skeleton to GXEngine format. GXEngine animation engine supports up to `4` bones per vertex, up to `80` bones per mesh with maximum bone name up to `64` bytes. Bone name has `UTF-8` encoding.

`Skins` directory contains skin information for skeletal meshes. GXEngine repo contains source code for building native plugin for _3ds Studio Max 2018_. This plugin allows user to export skin data to GXEngine format. GXEngine animation engine supports up to `4` bones per vertex, up to `80` bones per mesh with maximum bone name up to `64` bytes. Bone name has `UTF-8` encoding.

`Sounds` directory contains sound resources. Supported formats are:

* ogg

`Textures` directory contains texture resources. Supported formats are:

* .hdr
* .jpg
* .png
* .tga
* .tex

`.hdr` is HDR 4 channel 16 bit image format.
`.jpg` is `JPG|JPEG` format.
`.png` is `PNG` format
`.tga` is `TARGA` format
`.tex` is GXEngine proprietary uncompressed image format

GXEngine supports equirectangular textures for _cube map_ initialization.
