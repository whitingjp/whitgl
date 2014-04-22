import sys
import os
import platform

sys.path.insert(0, 'input')
import ninja_syntax

plat = platform.system()
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

ldflags = ''
if(plat == 'Windows'):
  cflags += ' -Iinput/glfw/include -Iinput/fmod/win/inc'
  ldflags += ' -Linput/glfw/lib-mingw -Linput/fmod/win/lib -lglfw -lglu32 -lopengl32 -lfmodex'
else:
  cflags += ' -Iinput/fmod/linux/inc'
  ldflags += ' -Wl,-rpath=.,--enable-new-dtags -Linput/fmod/linux/lib -lglfw -lGLU -lGL -lGLEW -lfmodex-4.44.15'

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
else:
  targets += n.build(joinp(outdir, 'libfmodex-4.44.15.so'), 'cp', joinp(inputdir, 'fmod', 'linux', 'lib', 'libfmodex-4.44.15.so'))
n.newline()

n.build('all', 'phony', targets)
n.default('all')
