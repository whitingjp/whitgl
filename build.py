import sys
import os
import platform

sys.path.insert(0, 'input')
import ninja_syntax

plat = platform.system()
bit64 = platform.architecture()[0] == '64bit'
joinp = os.path.join

BUILD_FILENAME = 'build.ninja'
buildfile = open(BUILD_FILENAME, 'w')
n = ninja_syntax.Writer(buildfile)

objext = '.o'

name = 'app'
if(plat == 'Windows'):
  target = name+'.exe'
else:
  target = name
srcdir = 'src'
inputdir = 'input'
builddir = 'build'
outdir = joinp(builddir, 'out')
objdir = joinp(builddir, 'obj')
data_in = 'data'
data_out = joinp(outdir, 'data')
targets = []

cflags = ''
cflags += '-Iinc'
cflags += ' -Wall -Wextra -Werror'
cflags += ' -g'

if(bit64):
  fmodlib = 'fmodex64'
else:
  fmodlib = 'fmodex'

ldflags = ''
if(plat == 'Windows'):
  cflags += ' -Iinput/glfw/include -Iinput/soil/src -Iinput/glew/include -Iinput/fmod/win/inc '
  ldflags += ' -Linput/glfw/lib-mingw -Linput/glew/lib -Linput/soil/lib -Linput/fmod/win/lib  -lsoil -lglfw -lglu32 -lopengl32 -lglew32 -lfmodex '
else:
  cflags += ' -isystem input/fmod/api/inc -Iinput/glfw/include '
  ldflags += ' -Wl,-rpath=.,--enable-new-dtags -Linput/fmod/api/lib -Linput/glfw/build/src -lglfw3 -lGLU -lGL -lGLEW -lSOIL -lm -l%s -lX11 -lXxf86vm -lpthread -lXrandr -lXi ' % fmodlib

# clfags += -pg
# ldflags += -pg

n.variable('cflags', cflags)
n.variable('ldflags', ldflags)
n.newline()

n.rule('cxx',
  command='gcc -MMD -MF $out.d $cflags -c $in -o $out',
  depfile='$out.d',
  description='CXX $out')
n.rule('link',
  command='gcc $in $libs $ldflags -o $out',
  description='LINK $out')
n.rule('cp',
  command='cp $in $out',
  description='COPY $in $out')
n.newline()

obj = []
for (dirpath, dirnames, filenames) in os.walk(srcdir):
  for f in filenames:
    _, ext = os.path.splitext(f)
    if ext == '.c':
      s = os.path.relpath(joinp(dirpath, f), srcdir)
      o = s.replace('.c', '.o')
      obj += n.build(joinp(objdir, o), 'cxx', joinp(srcdir, s))
n.newline()

targets += n.build(joinp(outdir, target), 'link', obj)
n.newline()

data = []
for (dirpath, dirnames, filenames) in os.walk(data_in):
  for f in filenames:
    s = os.path.relpath(joinp(dirpath, f), data_in)
    data += n.build(joinp(data_out, s), 'cp', joinp(data_in, s))
n.newline()

targets += n.build('data', 'phony', data)
n.newline()

if(plat == 'Windows'):
  targets += n.build(joinp(outdir, 'fmodex.dll'), 'cp', joinp(inputdir, 'fmod', 'fmodex.dll'))
  targets += n.build(joinp(outdir, 'glew32.dll'), 'cp', joinp(inputdir, 'glew', 'lib', 'glew32.dll'))
  targets += n.build(joinp(outdir, 'run_windowed.bat'), 'cp', joinp('windows', 'run_windowed.bat'))
  targets += n.build(joinp(outdir, 'run_fullscreen.bat'), 'cp', joinp('windows', 'run_fullscreen.bat'))
else:
  fmodso = 'lib%s.so' % fmodlib
  targets += n.build(joinp(outdir, fmodso), 'cp', joinp(inputdir, 'fmod', 'api', 'lib', fmodso))
n.newline()

n.build('all', 'phony', targets)
n.default('all')
