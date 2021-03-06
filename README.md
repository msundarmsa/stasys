# STASYS
[comment]: <> (...What is STASYS about...)

## Compilation
### Prerequisites
    Qt Creator 5 (to launch GUI)
    CMake (for CLI)
    Portaudio
    OpenCV
### Compiling Command-Line-Interface
```bash
cd build
cmake ..
make
```

### Compiling GUI
  Build using Qt Creator

## Deploying
### MacOS
1. Build `Release` with Qt Creator
2. Copy Info.plist into `build/qmake_release/stasys-qt.app/Contents`
3. Deploy to dmg using `macdeployqt`
```bash
cd build/qmake_release
cp ../../Info.plist stasys-qt.app/Contents
macdeployqt stasys-qt.app -dmg -qmldir="<path to stasys>" 
```
