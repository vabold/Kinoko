#!/usr/bin/env python3

from glob import glob
import io
import json
import os
import struct
import sys
from vendor.ninja_syntax import Writer

class TestCase:
    def __init__(self, key, value):
        self.name = str.encode(key) + b'\x00'
        self.rkgPath = str.encode("../"+value["rkgPath"]) + b'\x00'
        self.krkgPath = str.encode("../"+value["krkgPath"]) + b'\x00'
        self.targetFrame = value["targetFrame"]

# Parse test cases from JSON
tests = []

with open('testCases.json') as f:
    data = json.load(f)

    for key, value in data.items():
        tests.append(TestCase(key, value))

TEST_CASE_HEADER = b'TSTH'
TEST_CASE_FOOTER = b'TSTF'

BINARY_VER_MAJOR = 1
BINARY_VER_MINOR = 0

# Generate binary (enforce big-endian)
os.makedirs("out", exist_ok=True)
with open('out/testCases.bin', 'wb') as f:
    # Number of test cases
    f.write(struct.pack('>H', len(tests)))

    # Major version
    f.write(struct.pack('>H', BINARY_VER_MAJOR))

    # Minor version
    f.write(struct.pack('>H', BINARY_VER_MINOR))

    for test in tests:
        # Alignment check header
        f.write(struct.pack('>4s', TEST_CASE_HEADER))

        # Total memory size for this test case (excluding header/footer)
        nameLen = len(test.name)
        rkgLen = len(test.rkgPath)
        krkgLen = len(test.krkgPath)
        totalSize = 2 + nameLen + 2 + rkgLen + 2 + krkgLen + 2
        f.write(struct.pack('>H', totalSize))

        # Test name
        f.write(struct.pack('>H', nameLen))
        f.write(struct.pack('>' + str(nameLen) + 's', test.name))

        # RKG
        f.write(struct.pack('>H', rkgLen))
        f.write(struct.pack('>' + str(rkgLen) + 's', test.rkgPath))

        # KRKG
        f.write(struct.pack('>H', krkgLen))
        f.write(struct.pack('>' + str(krkgLen) + 's', test.krkgPath))

        # Total frames of run sync expected
        f.write(struct.pack('>H', test.targetFrame))

        # Alignment check footer
        f.write(struct.pack('<4s', TEST_CASE_FOOTER))


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
    '-fstack-protector-strong',
    '-isystem', '.',
    '-isystem', 'include',
    '-isystem', 'source',
    '-isystem', 'vendor',
    '-isystem', 'build',
    '-std=c++23',
    '-Wall',
    '-Werror',
    '-Wextra',
    '-Wno-delete-non-virtual-dtor',
    '-Wno-packed-bitfield-compat',
    '-Wsuggest-override',
]

target_cflags = [
    '-O3',
]

debug_cflags = [
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

code_in_files = [file for file in glob('**/*.cc', recursive=True)]

target_code_out_files = []
debug_code_out_files = []

for in_file in code_in_files:
    _, ext = os.path.splitext(in_file)

    target_out_file = os.path.join('$builddir', in_file + '.o')
    target_code_out_files.append(target_out_file)

    debug_out_file = os.path.join('$builddir', in_file + 'D.o')
    debug_code_out_files.append(debug_out_file)

    n.build(
        target_out_file,
        ext[1:],
        in_file,
        variables={
            'ccflags': ' '.join([*common_ccflags, *target_cflags])
        }
    )
    n.newline()

    n.build(
        debug_out_file,
        ext[1:],
        in_file,
        variables={
            'ccflags': ' '.join([*common_ccflags, *debug_cflags])
        }
    )
    n.newline()


n.build(
    os.path.join('$outdir', f'kinoko{file_extension}'),
    'ld',
    target_code_out_files,
    variables={
        'ldflags': ' '.join([
            *common_ldflags,
        ])
    },
)

n.build(
    os.path.join('$outdir', f'kinokoD{file_extension}'),
    'ld',
    debug_code_out_files,
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
