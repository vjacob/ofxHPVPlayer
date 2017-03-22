# ofxHPVPlayer

## Description

The HPV (High Performance Video) Eco-system is a cross-platform C++ toolset for playing back high resolution and high framerate video content. 

The HPV Eco-system was originally created by the University of Hasselt/iMinds for the European funded FP7 programma [Dreamspace](http://www.dreamspaceproject.eu/) to build
an open and cross-platform toolset for the Unity game-engine to realize immersive video installation using multiple calibrated projectors. This project was baptized 'Holo_ToolSet'.
This software, as well as working Windows 64bit plugin for Windows and example Unity projects, can be found in the original repository of the project [Holo_ToolSet](https://github.com/HasseltVR/Holo_Toolset).

ofxHPVPlayer is a partial fork of this technology allowing to use and playback HPV encoded video files in openFrameworks.

![alt text](/images/iabr.png "Use of HPV tech in an immersive installation @ IABR Rotterdam 2016")

## Features

- Play back `FullHD/4K/8K video files` at high framerates
	- Max achievable framerate is limited by the performance of your computer (HDD read speed, CPU speed, throughput speed of PCI-Express bus)
- Optimized for playing back `multiple videofiles` at the same time.
- Supports `blitting` (direct CPU texture to GPU texture) and `double buffered` playback (on OpenGL, using Pixel Buffer Objects)
- Self-contained custom HPV file format with `no dependencies` to platform specific media frameworks.
- Frames are compressed using texture compression methods (DXT). `Open source GUI HPV encoder` is provided for Windows & Mac
	- Supported compression types are:
		- `DXT1 (no alpha)`
			- reasonable image quality
		- `DXT5 (alpha)`
			- reasonable image quality + alpha
		- `SCALED DXT5 (CoCg_Y)`
			- good image quality, no alpha
			- bigger filesize

	- The encoder expects an image sequence where each frame is a separate image with a incremental number in the filename. 
	Supported filetypes are: `png, jpeg, jpg, tga, gif, bmp, psd, gif, hdr, pic, ppm, pgm` 
 
- Frames are then further compressed via [LZ4](https://github.com/lz4/lz4) HQ to get even smaller file sizes.
- Each videoplayer generates `playback state events` that can be captured in the openFrameworks application.
- `Render backend agnostic`, can be attached to OpenGL or DirectX context
- `Extensible format` that can contain multiple texture compression formats. Succesful tests have been made with `BPTC` and `ASTC` which will be available in a future update.

![alt text](/images/hpv_creator.png "The HPV Creator")

## Conditions

- OpenGL: needs at least an OpenGL 4.1 context
- Doesn't support audio playback (yet). Check the example-slave-to-audio example on how to achieve audio-video sync with the built-in openFrameworks audioplayer.

## Getting Started

- Creating HPV files with the HPVCreator. The latest release for Windows and Mac can be found on the original [Holo_ToolSet release page](https://github.com/HasseltVR/Holo_Toolset/releases).
- Generate the example project files using the openFrameworks [Project Generator](http://openframeworks.cc/learning/01_basics/how_to_add_addon_to_project/).
- For a good start, try to compile the example-controls example and play around with the GUI

## Documentation

- Check the examples showcasing different HPV functionalities.
- Pre-compressed HPV video files accompanying the examples can be found [here](https://goo.gl/UGv5TP)
- **example-simple**: This is a barebones example on how to play an HPV file.
- **example-controls**: This showcases (almost) all functionality available in the HPV system.
- **example-slave-to-audio**: Sync an HPV video file to an audio file with **exactly** the same length.
- **example-360video**: Playback 360 Equirectangular video files (handy for VR). Switch between equirectangular (latlong) and perspective modes.
- **example-sync-multiple-videos**: This example syncs 4 FullHD files that were the result of splitting one 4K video file. 

![alt text](/images/example-controls.png "HPV Example showcasing all controls")

## Compatibility

- Tested on Windows 7, 8 and 10.
- Tested on Max OSX 10.10.5. 
- Should work in Linux, but yet to be tested.

## Licensing

See `LICENSE.md`

## Included libraries
- `LZ4` [BSD-2-Clause](https://opensource.org/licenses/BSD-2-Clause)
- The HPV encoder uses one file of the `HAP Encoder`, namely [YCoCgDXT.cpp](https://github.com/Vidvox/hap-qt-codec/blob/master/source/YCoCgDXT.cpp)
- The HPV encoder is made with QT5 Open-source framework.
- Originally inspired on Roxlu's/Lab101 CinderBlock [Cinder-DXT](https://github.com/lab101/Cinder-DXT)

## Contributing

Pull Requests are welcomed:

1. Fork this repository.
2. Create your feature branch (`git checkout -b my-new-feature`).
3. Commit your changes (`git commit -am 'Add some feature'`).
4. Push to the branch (`git push origin my-new-feature`).
5. Create new Pull Request.
