import sys
import os
import platform

sys.path.insert(0, 'input')
import ninja_syntax

plat = platform.system()
bit64 = platform.architecture()[0] == '64bit'
joinp = os.path.join

if(bit64):
  fmoddir = 'x86_64'
else:
  fmoddir = 'x86'

def flags(input_dir):
  cflags = '-Iinc -Wall -Wextra -Werror -g'
  ldflags = ''
  if plat == 'Windows':
    cflags += ' -I_INPUT_/glfw/include -I_INPUT_/libpng/include -I_INPUT_/zlib/include -I_INPUT_/glew/include -I_INPUT_/fmod/win/inc -I_INPUT_/TinyMT '
    ldflags += ' -Wl,--stack,4194304 -L_INPUT_/glfw/lib-mingw -L_INPUT_/glew/lib -L_INPUT_/libpng/lib -L_INPUT_/fmod/win/lib _INPUT_/glfw/lib-mingw/glfw3dll.a -lglu32 -lopengl32 -lglew32 -lfmod -lpng _INPUT_/zlib/lib/zdll.lib -mwindows _INPUT_/TinyMT/tinymt/tinymt64.o'
  elif plat == 'Darwin':
    cflags += '  -fstack-protector-all -mmacosx-version-min=10.6 -isystem _INPUT_/fmod/inc -I_INPUT_/glfw/include -I_INPUT_/glew/include -I_INPUT_/libpng -I_INPUT_/TinyMT'
    ldflags += ' -mmacosx-version-min=10.6 -L_INPUT_/fmod/lib -L_INPUT_/glfw/build/src -L_INPUT_/libpng -L_INPUT_/zlib -L_INPUT_/glew/lib -framework OpenGL -framework Cocoa -framework IOKit -framework ForceFeedback -framework Carbon -framework CoreAudio -framework CoreVideo -framework AudioUnit -lpng -lfmod -lglfw3 -lGLEW -lz _INPUT_/TinyMT/tinymt/tinymt64.o'
  else:
    cflags += '  -fstack-protector-all -isystem _INPUT_/fmod/api/lowlevel/inc -I_INPUT_/glfw/include -I_INPUT_/TinyMT'
    ldflags += ' -Wl,-rpath=.,--enable-new-dtags -L_INPUT_/fmod/api/lowlevel/lib/%s -L_INPUT_/glfw/build/src -lglfw3 -lGLU -lGL -lGLEW -lm -lfmod -lX11 -lXxf86vm -lpthread -lXrandr -lXinerama -lXcursor -lXi -lpng -ldl _INPUT_/TinyMT/tinymt/tinymt64.o -lz' % fmoddir
  cflags = cflags.replace('_INPUT_', input_dir)
  ldflags = ldflags.replace('_INPUT_', input_dir)
  return (cflags, ldflags)

def rules(n, cflags, ldflags, scripts):
  n.variable('cflags', cflags)
  n.variable('ldflags', ldflags)
  n.newline()

  n.rule('cxx',
    command='gcc -MMD -MF $out.d $cflags -c $in -o $out',
    depfile='$out.d',
    description='CXX $out')
  n.rule('static',
    command='ar rcs $out $in')
  if plat == 'Darwin':
    n.rule('link',
      command='gcc $in $libs $ldflags -o $out && install_name_tool -change @rpath/libfmod.dylib @executable_path/libfmod.dylib $out && install_name_tool -change /usr/lib/libGLEW.1.13.0.dylib @executable_path/libGLEW.1.13.0.dylib $out',
      description='LINK $out')
  else:
    n.rule('link',
      command='gcc $in $libs $ldflags -o $out',
      description='LINK $out')
  n.rule('cp',
    command='cp $in $out',
    description='COPY $in $out')
  n.rule('model',
    command='python %s $in $out' % joinp(scripts,'process_model.py'),
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
  n.newline()
  return obj

def walk_data(n, data_in, data_out, validext=['png','ogg','obj','wav']):
  data = []
  for (dirpath, dirnames, filenames) in os.walk(data_in):
    for f in filenames:
      ext = f[-3:]
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
    targets += n.build(joinp(outdir, 'fmod.dll'), 'cp', joinp(inputs, 'fmod', 'win', 'lib', 'fmod.dll'))
    targets += n.build(joinp(outdir, 'glew32.dll'), 'cp', joinp(inputs, 'glew', 'lib', 'glew32.dll'))
    targets += n.build(joinp(outdir, 'glfw3.dll'), 'cp', joinp(inputs, 'glfw', 'lib-mingw', 'glfw3.dll'))
    targets += n.build(joinp(outdir, 'libpng3.dll'), 'cp', joinp(inputs, 'libpng', 'bin', 'libpng3.dll'))
    targets += n.build(joinp(outdir, 'zlib1.dll'), 'cp', joinp(inputs, 'zlib', 'zlib1.dll'))
  elif plat == 'Darwin':
    targets += n.build(joinp(outdir, 'libfmod.dylib'), 'cp', joinp(inputs, 'fmod', 'lib', 'libfmod.dylib'))
    targets += n.build(joinp(outdir, 'libGLEW.1.13.0.dylib'), 'cp', joinp(inputs, 'glew', 'lib', 'libGLEW.1.13.0.dylib'))
  else:
    targets += n.build(joinp(outdir, 'libfmod.so.5'), 'cp', joinp(inputs, 'fmod', 'api', 'lowlevel', 'lib', fmoddir, 'libfmod.so'))
  n.newline()
  return targets

def do_game(target, extra_cflags, data_types):
  srcdir = 'src'
  inputdir = joinp('whitgl', 'input')
  builddir = 'build'
  targetdir = joinp(builddir, 'out')
  objdir = joinp(builddir, 'obj')
  libdir = joinp(builddir, 'lib')
  data_in =  'data'
  data_out = joinp(targetdir, 'data')
  buildfile = open('build.ninja', 'w')
  n = ninja_syntax.Writer(buildfile)
  cflags, ldflags = flags(inputdir)
  cflags = cflags + ' -Iwhitgl/inc -Isrc ' + extra_cflags
  rules(n, cflags, ldflags)
  obj = walk_src(n, srcdir, objdir)
  whitgl = [joinp('whitgl','build','lib','whitgl.a')]
  targets = []
  targets += n.build(joinp(targetdir, target), 'link', obj+whitgl)
  n.newline()

  data = walk_data(n, data_in, data_out, data_types)

  targets += n.build('data', 'phony', data)
  n.newline()

  targets += copy_libs(n, inputdir, targetdir)

  n.build('all', 'phony', targets)
  n.default('all')

def main():
  target = 'whitgl.a'
  srcdir = 'src'
  examplesrcdir = 'example'
  inputdir = 'input'
  builddir = 'build'
  scriptsdir = 'scripts'
  exampledir = joinp(builddir, 'example')
  objdir = joinp(builddir, 'obj')
  libdir = joinp(builddir, 'lib')
  data_in =  joinp(examplesrcdir, 'data')
  data_out = joinp(exampledir, 'data')
  BUILD_FILENAME = 'build.ninja'
  buildfile = open(BUILD_FILENAME, 'w')
  n = ninja_syntax.Writer(buildfile)
  cflags, ldflags = flags('input')
  rules(n, cflags, ldflags, scriptsdir)
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
