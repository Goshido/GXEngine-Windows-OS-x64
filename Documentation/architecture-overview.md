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

Application code must be placed in `GXGame` successor class and send to `GXCore` object via `Start` method:

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

* _Animations_
* _Config_
* _Fonts_
* _Locale_
* _Meshes_
* _Shaders_
* _Skeletons_
* _Skins_
* _Sounds_
* _Textures_

`Animations` directory holds skeletal mesh animations. Animation file has proprietary format. _GXEngine_ repo contains source code for building native plugin for _3ds Studio Max 2018_. This plugin allows user to export animations to GXEngine format. _GXEngine_ animation system supports up to `4` bones per vertex, up to `80` bones per mesh with maximum bone name up to `64` bytes. Bone name has `UTF-8` encoding.

`Config` directory contains configuration files for _GXEngine_ deploy.

`Fonts` directory has font files in `.ttf` format.

`Locale` directory contains translation files. This files must have `UTF-8` encoding. Files have format:

```
<key>~<value>~
...
```

Examples:

```
hello~Hello~

boobs~Boobs
will safe the world!~

button1~Button1~

button2~Button2~

english~English~

russian~Russian~

Main menu->File~File~

Main menu->File->New~New~

Main menu->File->New (Hotkey)~Ctrl+N~
```

Current _GXEngine_ implementation has predefined slots for two languges: `English` and `Russian`. But any unicode code points is supported (if target font does).

`Meshes` directory contains mesh geometry files. Mesh geometry files could have formats:

* `.obj`
* `.stm`
* `.skm`
* `.mesh`

`.obj` must have triangle mesh format, _normal_ data and _UV_ data. Material sections are ignored.<br/>
`.stm` is proprietary (but deprecated) format for static meshes.<br/>
`.skm` is proprietary (but deprecated) format for skeletal meshes.<br/>
`.mesh` is proprietary (recommended) format for skeletal meshes. _GXEngine_ repo contains source code for building native plugin for _3ds Studio Max 2018_. This plugin allows user to export meshes to _GXEngine_ format.

`Shaders` directory contains source shader code and precompiled shader code. Current implementation supports shaders for OpenGL 3.3 core profile (GLSL).

`Skeletons` directory contains skeleton information for skeletal meshes. _GXEngine_ repo contains source code for building native plugin for _3ds Studio Max 2018_. This plugin allows user to export skeleton to _GXEngine_ format. _GXEngine_ animation system supports up to `4` bones per vertex, up to `80` bones per mesh with maximum bone name up to `64` bytes. Bone name has `UTF-8` encoding.

`Skins` directory contains skin information for skeletal meshes. _GXEngine_ repo contains source code for building native plugin for _3ds Studio Max 2018_. This plugin allows user to export skin data to GXEngine format. _GXEngine_ animation system supports up to `4` bones per vertex, up to `80` bones per mesh with maximum bone name up to `64` bytes. Bone name has `UTF-8` encoding.

`Sounds` directory contains sound resources. Supported formats are:

* `.ogg`

`Textures` directory contains texture resources. Supported formats are:

* `.hdr`
* `.jpg`
* `.png`
* `.tga`
* `.tex`

`.hdr` is HDR 4 channel 16 bit image format.<br/>
`.jpg` is `JPG|JPEG` format.<br/>
`.png` is `PNG` format.<br/>
`.tga` is `TARGA` format.<br/>
`.tex` is _GXEngine_ proprietary uncompressed image format.

_GXEngine_ supports [equirectangular](https://en.wikipedia.org/wiki/Equirectangular_projection) textures for _cube map_ initialization.
