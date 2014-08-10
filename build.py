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
  cflags += ' -Iinput/glfw/include -Iinput/soil/include -Iinput/glew/include -Iinput/fmod/win/inc '
  ldflags += ' -Linput/glfw/lib-mingw -Linput/glew/lib -Linput/soil -Linput/fmod/win/lib  -lSOIL input/glfw/lib-mingw/glfw3dll.a -lglu32 -lopengl32 -lglew32 -lfmodex -lpng '
else:
  cflags += ' -isystem input/fmod/api/inc -Iinput/glfw/include '
  ldflags += ' -Wl,-rpath=.,--enable-new-dtags -Linput/fmod/api/lib -Linput/glfw/build/src -lglfw3 -lGLU -lGL -lGLEW -lSOIL -lm -l%s -lX11 -lXxf86vm -lpthread -lXrandr -lXi -lpng' % fmodlib

# clfags += -pg
# ldflags += -pg

n.variable('cflags', cflags)
n.variable('ldflags', ldflags)
n.newline()

n.rule('cxx',
  command='gcc -MMD -MF $out.d $cflags -c $in -o $out',
  depfile='$out.d',
  description='CXX $out')
n.rule('static',
  command='ar rcs $out $in')
n.rule('link',
  command='gcc $in $libs $ldflags -o $out',
  description='LINK $out')
n.rule('cp',
  command='cp $in $out',
  description='COPY $in $out')
n.newline()

def walk_src(n, path):
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

# Library
obj = walk_src(n, srcdir)
staticlib = n.build(joinp(libdir, target), 'static', obj) 
targets += staticlib
n.newline()

# Example
obj = walk_src(n, examplesrcdir)
targets += n.build(joinp(exampledir, 'example'), 'link', obj+staticlib)
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
  targets += n.build(joinp(exampledir, 'fmodex.dll'), 'cp', joinp(inputdir, 'fmod', 'fmodex.dll'))
  targets += n.build(joinp(exampledir, 'glew32.dll'), 'cp', joinp(inputdir, 'glew', 'lib', 'glew32.dll'))
  targets += n.build(joinp(exampledir, 'glfw3.dll'), 'cp', joinp(inputdir, 'glfw', 'lib-mingw', 'glfw3.dll'))
else:
  fmodso = 'lib%s.so' % fmodlib
  targets += n.build(joinp(exampledir, fmodso), 'cp', joinp(inputdir, 'fmod', 'api', 'lib', fmodso))
n.newline()

n.build('all', 'phony', targets)
n.default('all')
