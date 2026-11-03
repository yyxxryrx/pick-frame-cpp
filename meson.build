project(
  'pick-frame-cpp',
  'cpp',
  version: '0.1',
  meson_version: '>= 1.3.0',
  default_options: ['warning_level=3', 'cpp_std=c++23'],
)

dependencies = [
  dependency('libavcodec'),
  dependency('libavformat'),
  dependency('libavutil'),
  dependency('libswscale'),
  dependency('cxxopts', fallback: ['cxxopts']),
]

sources = [
  'src/main.cpp',
  'src/cli.cpp',
  'src/cli.h',
  'src/sws.h',
  'src/util.cpp',
  'src/util.h',
  'src/types/av.h',
  'src/types/ffmpeg.h',
  'src/types/position.cpp',
  'src/types/position.h',
  'src/types/time.cpp',
  'src/types/time.h',
  'src/video/video_frame_reader.cpp',
  'src/video/video_frame_reader.h',
  'src/video/video_info_reader.cpp',
  'src/video/video_info_reader.h',
]

exe = executable(
  'pick-frame-cpp',
  sources,
  install: true,
  dependencies: dependencies,
)

test('basic', exe)
