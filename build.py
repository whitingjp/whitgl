import sys
import os
import platform

sys.path.insert(0, 'input')
import ninja_syntax

plat = platform.system()
if plat.startswith('MINGW32_NT'):
  plat = 'Windows'
bit64 = platform.architecture()[0] == '64bit'
joinp = os.path.join

def flags(input_dir):
  cflags = '-Iinc -Wall -Wextra -Werror'
  optimizing = False
  for arg in sys.argv:
    if arg == 'optimize':
      optimizing = True
  if optimizing:
    cflags += ' -O2'
  else:
    cflags += ' -g'
  ldflags = ''
  if plat == 'Windows':
    cflags += ' -D WHITGL_WINDOWS -I_INPUT_/glfw/include -I_INPUT_/libpng -I_INPUT_/zlib -I_INPUT_/glew/include  -I_INPUT_/irrklang/include -I_INPUT_/TinyMT'
    ldflags += ' -Wl,--stack,4194304 -L_INPUT_/glfw/lib-mingw -L_INPUT_/glew/lib/Release/Win32 -L_INPUT_/libpng -L_INPUT_/zlib -L_INPUT_/irrklang/bin/win32-gcc _INPUT_/glfw/lib-mingw/libglfw3dll.a -lglew32s -lglu32 -lopengl32  -lirrKlang -lpng -lz -mwindows _INPUT_/TinyMT/tinymt/tinymt64.o -lstdc++'
  elif plat == 'Darwin':
    cflags += ' -D WHITGL_OSX -fstack-protector-all -mmacosx-version-min=10.6 -isystem _INPUT_/irrklang/include -I_INPUT_/glfw/include -I_INPUT_/glew/include -I_INPUT_/libpng -I_INPUT_/TinyMT'
    ldflags += ' -mmacosx-version-min=10.6 -L_INPUT_/irrklang/bin/macosx-gcc -L_INPUT_/glfw/build/src -L_INPUT_/libpng -L_INPUT_/zlib -L_INPUT_/glew/lib -framework OpenGL -framework Cocoa -framework IOKit -framework ForceFeedback -framework Carbon -framework CoreAudio -framework CoreVideo -framework AudioUnit -lpng -lirrklang -lglfw3 -lGLEW -lz _INPUT_/TinyMT/tinymt/tinymt64.o'
  else:
    cflags += ' -D WHITGL_LINUX -fstack-protector-all -isystem _INPUT_/irrklang/include -I_INPUT_/glfw/include -I_INPUT_/TinyMT'
    ldflags += ' -Wl,-rpath=.,--enable-new-dtags -L_INPUT_/irrklang/bin/linux-gcc-64  -L_INPUT_/glfw/build/src -lglfw3 -lGLU -lGL -lGLEW -lm -lIrrKlang -lX11 -lXxf86vm -lpthread -lXrandr -lXinerama -lXcursor -lXi -lpng -ldl _INPUT_/TinyMT/tinymt/tinymt64.o -lz -lstdc++'
  cflags = cflags.replace('_INPUT_', input_dir)
  ldflags = ldflags.replace('_INPUT_', input_dir)
  return (cflags, ldflags)

def rules(n):
  n.rule('cxx',
    command='gcc -MMD -MF $out.d $cflags -c $in -o $out',
    depfile='$out.d',
    description='CXX $out')
  n.rule('static',
    command='ar rcs $out $in')
  if plat == 'Darwin':
    n.rule('link',
      command='gcc $in $libs $ldflags -o $out && install_name_tool -change /usr/local/lib/libirrklang.dylib @executable_path/libirrklang.dylib $out && install_name_tool -change /usr/local/lib/libGLEW.2.1.0.dylib @executable_path/libGLEW.2.1.0.dylib $out',
      description='LINK $out')
    n.rule('icon',
      command='$scriptsdir/osx/create_icns.sh $in $builddir/icon $out',
      description='ICON $out')
  else:
    n.rule('link',
      command='gcc $in $libs $ldflags -o $out',
      description='LINK $out')
  n.rule('cp',
    command='cp $in $out',
    description='COPY $in $out')
  n.rule('model',
    command='python $scriptsdir/process_model.py $in $out',
    description='MODEL $in $out')
  n.newline()

def walk_src(n, path, objdir):
  obj = []
  for (dirpath, dirnames, filenames) in os.walk(path):
    for f in filenames:
      _, ext = os.path.splitext(f)
      if ext == '.c':
        s = os.path.relpath(joinp(dirpath, f), path)
        o = s.replace('.c', '.o')
        obj += n.build(joinp(objdir, o), 'cxx', joinp(path, s))
      if ext == '.cpp':
        s = os.path.relpath(joinp(dirpath, f), path)
        o = s.replace('.cpp', '.o')
        obj += n.build(joinp(objdir, o), 'cxx', joinp(path, s))
  n.newline()
  return obj

def walk_data(n, data_in, data_out, validext=['png','ogg','obj','wav']):
  data = []
  for (dirpath, dirnames, filenames) in os.walk(data_in):
    for f in filenames:
      _, ext = os.path.splitext(f)
      ext = ext[1:]
      valid = False
      for e in validext:
        if ext == e:
          valid = True
      if not valid:
        continue
      s = os.path.relpath(joinp(dirpath, f), data_in)
      src = joinp(data_in, s)
      dst = joinp(data_out, s)
      rule = 'cp'
      if ext == 'obj':
        rule = 'model'
        dst = dst[:-3]+'wmd'
      data += n.build(dst, rule, src)
  n.newline()
  return data

def copy_libs(n, inputs, outdir):
  targets = []
  if plat == 'Windows':
    targets += n.build(joinp(outdir, 'irrKlang.dll'), 'cp', joinp(inputs, 'irrklang', 'bin', 'win32-gcc', 'irrKlang.dll'))
    targets += n.build(joinp(outdir, 'glfw3.dll'), 'cp', joinp(inputs, 'glfw', 'lib-mingw', 'glfw3.dll'))
    targets += n.build(joinp(outdir, 'zlib1.dll'), 'cp', joinp(inputs, 'zlib', 'zlib1.dll'))
  elif plat == 'Darwin':
    targets += n.build(joinp(outdir, 'libirrklang.dylib'), 'cp', joinp(inputs, 'irrklang', 'bin', 'macosx-gcc', 'libirrklang.dylib'))
    targets += n.build(joinp(outdir, 'libGLEW.2.1.0.dylib'), 'cp', joinp(inputs, 'glew', 'lib', 'libGLEW.2.1.0.dylib'))
  else:
    targets += n.build(joinp(outdir, 'libIrrKlang.so'), 'cp', joinp(inputs, 'irrklang', 'bin', 'linux-gcc-64', 'libIrrKlang.so'))
  n.newline()
  return targets

def do_game(name, extra_cflags, data_types):
  target = name.lower()
  srcdir = 'src'
  inputdir = joinp('whitgl', 'input')
  builddir = 'build'
  targetdir = joinp(builddir, 'out')
  if plat == 'Darwin':
    packagedir = joinp(targetdir, '%s.app' % name, 'Contents')
    executabledir = joinp(packagedir, 'MacOS')
    data_out = joinp(packagedir, 'Resources', 'data')
  else:
    executabledir = targetdir
    data_out = joinp(targetdir, 'data')
  objdir = joinp(builddir, 'obj')
  libdir = joinp(builddir, 'lib')
  data_in =  'data'
  if not os.path.exists('build'):
    os.makedirs('build')
  buildfile = open(joinp('build', 'build.ninja'), 'w')
  n = ninja_syntax.Writer(buildfile)
  n.variable('builddir', builddir)
  n.variable('scriptsdir', joinp('whitgl','scripts'))
  n.newline()
  cflags, ldflags = flags(inputdir)
  cflags = cflags + ' -Iwhitgl/inc -Isrc ' + extra_cflags
  n.variable('cflags', cflags)
  n.variable('ldflags', ldflags)
  n.newline()
  rules(n)
  obj = walk_src(n, srcdir, objdir)
  whitgl = [joinp('whitgl','build','lib','whitgl.a')]
  targets = []
  targets += n.build(joinp(executabledir, target), 'link', obj+whitgl)
  n.newline()

  data = walk_data(n, data_in, data_out, data_types)

  targets += n.build('data', 'phony', data)
  n.newline()

  targets += copy_libs(n, inputdir, executabledir)

  if plat == 'Darwin':
    targets += n.build(joinp(packagedir, 'Info.plist'), 'cp', joinp(data_in, 'osx', 'Info.plist'))
    targets += n.build(joinp(packagedir, 'Resources', 'Icon.icns'), 'icon', joinp('art', 'icon', 'icon.png'))

  n.build('all', 'phony', targets)
  n.default('all')

def main():
  target = 'whitgl.a'
  srcdir = 'src'
  examplesrcdir = 'example'
  inputdir = 'input'
  builddir = 'build'
  exampledir = joinp(builddir, 'example')
  objdir = joinp(builddir, 'obj')
  libdir = joinp(builddir, 'lib')
  data_in =  joinp(examplesrcdir, 'data')
  data_out = joinp(exampledir, 'data')
  if not os.path.exists('build'):
    os.makedirs('build')
  buildfile = open(joinp('build', 'build.ninja'), 'w')
  n = ninja_syntax.Writer(buildfile)
  n.variable('builddir', builddir)
  n.variable('scriptsdir', 'scripts')
  n.newline()
  cflags, ldflags = flags('input')
  n.variable('cflags', cflags)
  n.variable('ldflags', ldflags)
  n.newline()
  rules(n)
  # Library
  obj = walk_src(n, srcdir, objdir)
  staticlib = n.build(joinp(libdir, target), 'static', obj)
  targets = []
  targets += staticlib
  n.newline()

  # Example
  obj = walk_src(n, examplesrcdir, objdir)
  targets += n.build(joinp(exampledir, 'example'), 'link', obj+staticlib)
  n.newline()

  data = walk_data(n, data_in, data_out)

  targets += n.build('data', 'phony', data)
  n.newline()

  targets += copy_libs(n, inputdir, exampledir)

  n.build('all', 'phony', targets)
  n.default('all')

if __name__ == '__main__':
  main()
