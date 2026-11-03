# pick-frame-cpp

## Build

> You should have installed `ffmpeg` in your system

### Build with Meson

#### 1. setup the project (only run once)

```bash
# the native-file flag is optional
meson setup --native-file native-clang.ini build
```

#### 2. compile the project

```bash
meson compile -C build
```

### Build with Cmake

#### 1. setup the project (only run once)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

#### 2. build the project

```bash
cmake --build build
```

