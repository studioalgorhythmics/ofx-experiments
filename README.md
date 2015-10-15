# ofx-experiments
Experiments with ofx


## ofxSimpleGuiToo
This is a fork of the [gui addon](https://github.com/memo/ofxSimpleGuiToo) originally wrote by [Memo](http://www.memo.tv).
It features a few changes to make it work with OpenFrameworks 0.8.x

## ofAlgorhythmicSorting
A port to OpenFrameworks 0.8.x of the original project by the same name.

### Changes:
* fixed the Xcode project to work with newer OpenFrameworks versions:
  * architecture set to `32bit`
  * base SDK set to `10.8`
  * compiler and linker conflicts solved
* `portaudio` compiled as a `dylib` library from the latest stable source code, included into the pjoject and bundled into the application
* updated version of `ofxSimpleGuiToo` included into the project and used to create the GUI
* removed the old deprecated `ofxThread` and `ofxMutex` references and moved to the new ofThread and ofMutex classes, embedded into OpenFrameworks
* various minor fixes to avoid conflicts.

### Dependencies:
* OpenFrameworks 0.8.4
* ofxXmlSettings
* [ofxMSAInteractiveObject](https://github.com/memo/ofxMSAInteractiveObject)
* ofxSimpleGuiToo (the custom fork included in this repository)

## ofPrimeSieve
A port to OpenFrameworks 0.8.x of the original project by the same name.

### Changes:
* fixed the Xcode project to work with newer OpenFrameworks versions:
  * architecture set to `32bit`
  * base SDK set to `10.8`
  * compiler and linker conflicts solved
* `portaudio` compiled as a `dylib` library from the latest stable source code, included into the pjoject and bundled into the application
* updated version of `ofxSimpleGuiToo` included into the project and used to create the GUI
* various minor fixes to avoid conflicts.

### Dependencies:
* OpenFrameworks 0.8.4
* ofxXmlSettings
* [ofxMSAInteractiveObject](https://github.com/memo/ofxMSAInteractiveObject)
* ofxSimpleGuiToo (the custom fork included in this repository)