#!/usr/bin/env python3

import io
import os
import sys
from vendor.ninja_syntax import Writer

out_buf = io.StringIO()
n = Writer(out_buf)

file_extension = ''
if sys.platform.startswith('win32'):
    file_extension = '.exe'

n.variable('ninja_required_version', '1.3')
n.newline()

n.variable('builddir', 'build')
n.variable('outdir', 'out')
n.newline()

n.variable('compiler', 'g++')
n.newline()

common_ccflags = [
    '-DREVOLUTION',
    '-fno-asynchronous-unwind-tables',
    '-fno-exceptions',
    '-fno-rtti',
    '-fshort-wchar',
    '-isystem', '.',
    '-isystem', 'include',
    '-isystem', 'payload',
    '-isystem', 'vendor',
    '-isystem', 'build',
    '-std=c++23',
    '-Wall',
    '-Werror=vla',
    '-Werror=uninitialized',
    '-Wextra',
    '-Wno-delete-non-virtual-dtor',
    '-Wno-packed-bitfield-compat',
    '-Wsuggest-override',
]

target_cflags = [
    '-O0',
    '-ggdb',
]

common_ldflags = []

n.rule(
    'cc',
    command='$compiler -MD -MT $out -MF $out.d $ccflags -c $in -o $out',
    depfile='$out.d',
    deps='gcc',
    description='CC $out',
)
n.newline()

n.rule(
    'ld',
    command='$compiler $ldflags $in -o $out',
    description='LD $out',
)

code_in_files = [
    os.path.join('include', 'Common.cc'),
    os.path.join('source', 'abstract', 'Archive.cc'),
    os.path.join('source', 'abstract', 'File.cc'),
    os.path.join('source', 'egg', 'core', 'Decomp.cc'),
    os.path.join('source', 'egg', 'core', 'Archive.cc'),
    os.path.join('source', 'egg', 'math', 'Math.cc'),
    os.path.join('source', 'egg', 'math', 'Vector.cc'),
    os.path.join('source', 'game', 'kart', 'KartParam.cc'),
    os.path.join('source', 'game', 'system', 'DvdArchive.cc'),
    os.path.join('source', 'game', 'system', 'MultiDvdArchive.cc'),
    os.path.join('source', 'game', 'system', 'RaceConfig.cc'),
    os.path.join('source', 'game', 'system', 'ResourceManager.cc'),
    os.path.join('source', 'host', 'main.cc'),
    os.path.join('source', 'host', 'System.cc'),
]

code_out_files = []

for in_file in code_in_files:
    _, ext = os.path.splitext(in_file)
    out_file = os.path.join('$builddir', in_file + '.o')
    code_out_files.append(out_file)
    n.build(
        out_file,
        ext[1:],
        in_file,
        variables={
            'ccflags': ' '.join([*common_ccflags, *target_cflags])
        }
    )
    n.newline()


n.build(
    os.path.join('$outdir', f'kinoko{file_extension}'),
    'ld',
    code_out_files,
    variables={
        'ldflags': ' '.join([
            *common_ldflags,
        ])
    },
)

n.variable('configure', 'configure.py')
n.newline()

n.rule(
    'configure',
    command=f'{sys.executable} $configure',
    generator=True,
)
n.build(
    'build.ninja',
    'configure',
    implicit=[
        '$configure',
        os.path.join('vendor', 'ninja_syntax.py'),
    ],
)

with open('build.ninja', 'w') as out_file:
    out_file.write(out_buf.getvalue())
n.close()
