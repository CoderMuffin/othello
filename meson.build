project('othello', 'cpp', version: '0.1', default_options: ['cpp_std=c++17'])

# Compiler flags based on the platform
  if host_machine.system() == 'windows'
      compiler_flags = ['/external:anglebrackets', '/external:W0', '/W4', '/WX', '/Wall', '/wd4514', '/wd4668', '/wd5045', '/wd4820', '/wd4100', '/wd4365']
  else
      compiler_flags = ['-O3', '-Wall', '-Wpedantic']
  endif

# Include directories
lib_include = include_directories('lib')

# Source files
othello_sources = [
    'src/board.cpp',
    'src/nn.cpp',
    'src/util.cpp',
    'src/nnbatch.cpp',
]

main_sources = [
    'src/main.cpp',
    'src/input.cpp',
    'src/ui/ui.cpp',
    'src/ui/boardui.cpp',
]

test_sources = [
    'src/test.cpp',
]

# Define library and executables
othello_lib = static_library('othello', othello_sources,
    include_directories: lib_include,
    cpp_args: compiler_flags
)

sdl2_dep = dependency('sdl2', required: true)
main_executable = executable('main', main_sources,
    include_directories: lib_include,
    cpp_args: compiler_flags,
    link_with: [othello_lib],
    dependencies: [sdl2_dep]
)

test_executable = executable('main_test', test_sources,
    include_directories: lib_include,
    link_with: [othello_lib]
)


