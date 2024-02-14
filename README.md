# AV-Filter-EDOF

Enhance video with AV-Viewer in combination with this EDOF filter (Extended Depth of Focus)! Get more digital focus when using lenses with a narrow focus area. For use in microscopy or photography.

## Key Features

1. Increases perceived depth of field: Makes foreground and background elements appear sharper and in focus.
2. Improved image clarity: Enhances details and textures across the entire image.
3. Compatible with various lenses: Works with lenses designed for narrow focus areas, like microscope objectives or telephoto lenses.

## Installation:

1. Download the AV-Filter-EDOF project files.
2. Place the files in the appropriate directory within your AV-Viewer's plugin structure (refer to your AV-Viewer's documentation for specific instructions).
3. Restart your AV-Viewer application.

```bash
mkdir build
git clone git@github.com:dirkjanbuter/AV-Filter-EDOF.git
cd AV-Filter-EDOF
make
```

## Usage

1. Access the plugin's interface or configuration options within your AV-Viewer.
2. Adjust the available controls to personalize the EDOF effect according to your desired outcome.
3. Preview the changes in real-time to visualize the impact on your AV content.

```bash
cd ../build
./av-viewer 640 360 30 filter_edof.so ffffffff -
```
```bash
./av-viewer 640 360 30 filter_player.so ffffffff filename.mp4 filter_edof.so ffffffff -
```

## Compatibility

List the specific versions of AV-Viewer this plugin is compatible with:

* [AV-Viewer 1.0](https://github.com/dirkjanbuter/AV-Viewer)

## GNU General Public License v3 (GPL-3)

You may copy, distribute and modify the software as long as you track changes/dates in source files. Any modifications to or software including (via compiler) GPL-licensed code must also be made available under the GPL along with build & install instructions.

## Contribution

Contributing a video & audio filter in C requires code clarity, modularity, error handling, and performance optimization. Follow coding standards, use common data types, define formats, write documentation and tests, and manage resources well. Consider security, minimize dependencies, and contribute to the community.

## Additional Notes

* Only tested on Linux will probably work on Mac OS X and Windows.
* For example output see [YouTube video](https://youtu.be/NCdL2Mem2B8?si=EqnxtUDK32lWdCPd)
* If you have touble with running or compiling this code, feel free to contact me.
