#!/usr/bin/env python3

import glob
import os
import re
import io
import sys
import sysconfig
import subprocess
from dataclasses import dataclass, field
from vendor.ninja_syntax import Writer
from tools.generate_tests import generate_tests

generate_tests()

# Ninja variables and rules
out_buf = io.StringIO()
n = Writer(out_buf)

build_dir = "build"

n.variable("ninja_required_version", "1.3")
n.variable("builddir", build_dir)
n.variable("outdir", "out")
n.variable("compiler", "g++")
n.variable("ar", "ar")
n.newline()
n.rule("cc", command="$compiler -MD -MT $out -MF $out.d $ccflags -c $in -o $out", depfile="$out.d", deps="gcc", description="CC $out")
n.rule("ar", command="$ar rcs $out $in", description="AR $out")
n.newline()
n.rule("ld", command="$compiler $in $ldflags -o $out", description="LD $out")
n.newline()
n.rule("ld_shared", command="$compiler -shared $in $ldflags -o $out", description="LD_SHARED $out")
n.newline()

# Download dependancies
deps_path = os.path.join(build_dir, "_deps")
os.makedirs(deps_path, exist_ok=True)
nanobind_path = os.path.join(deps_path, "nanobind")
robin_path = os.path.join(deps_path, "robin-map")

if not os.path.isdir(nanobind_path):
    subprocess.run(["git", "clone", "https://github.com/wjakob/nanobind", nanobind_path], check=True)

if not os.path.isdir(robin_path):
    subprocess.run(["git", "clone", "https://github.com/Tessil/robin-map.git", robin_path], check=True)

python_include_path = sysconfig.get_paths()["include"]
nanobind_src_path = os.path.join(nanobind_path, "src")
nanobind_include_path = os.path.join(nanobind_path, "include")
robin_include_path = os.path.join(robin_path, "include")
python_lib_path = os.path.join(sysconfig.get_paths()["data"], "libs", f"libpython{sys.version_info.major}.{sys.version_info.minor}.a")

# Flags
common_ccflags = [
    "-DREVOLUTION",
    "-fno-asynchronous-unwind-tables",
    "-fno-exceptions",
    "-fshort-wchar",
    "-fstack-protector-strong",
    "-isystem", "include",
    "-isystem", "source",
    "-Wall",
    "-Werror",
    "-Wextra",
    "-Wno-delete-non-virtual-dtor",
    "-Wno-packed-bitfield-compat",
    "-Wsuggest-override",
    "-std=c++23",
    "-fPIC",
    "-fno-exceptions",
]

bindings_ccflags = [
    "-DREVOLUTION",
    "-fstack-protector-strong",
    "-isystem", "include",
    "-isystem", "source",
    "-isystem", python_include_path,
    "-isystem", nanobind_include_path,
    "-isystem", robin_include_path,
    "-Wall",
    "-Werror",
    "-Wextra",
    "-Wno-delete-non-virtual-dtor",
    "-Wno-packed-bitfield-compat",
    "-Wsuggest-override",
    "-std=c++23",
    "-fPIC",
]

# Define build types
@dataclass
class Build:
    name: str
    suffix: str
    flags: list[str]
    obj_files: dict = field(default_factory=lambda: {
        "common": [],
        "main": [],
        "bindings": []
    })

builds = [
    Build(name="target", suffix="", flags=["-O3"]),
    Build(name="debug", suffix="D", flags=["-DBUILD_DEBUG", "-O0", "-ggdb"])
]

# Compile files
def GetFiles():
    sep = re.escape(os.path.sep)
    main_regex = re.compile(rf".*{sep}host{sep}main\.cc$")
    bindings_regex = re.compile(rf".*{sep}host{sep}KBind.*\.cc$")

    main_files = []
    bindings_files = []
    common_files = []

    for file in glob.glob(os.path.join("source", "**", "*.cc"), recursive=True):
        if main_regex.search(file):
            main_files.append(file)
            continue

        if bindings_regex.search(file):
            bindings_files.append(file)
            continue

        common_files.append(file)

    bindings_files.append(os.path.join(nanobind_src_path, "nb_combined.cpp"))

    return common_files, main_files, bindings_files

def Compile(files, builds, obj_key, flags):
    for in_file in files:
        base_name, _ = os.path.splitext(in_file)
        for build in builds:
            out_file = os.path.join("$builddir", f"{base_name}{build.suffix}.o")
            build.obj_files[obj_key].append(out_file)
            n.build(out_file, "cc", in_file, variables={"ccflags": " ".join([*flags, *build.flags])})

common_files, main_files, bindings_files = GetFiles()
Compile(common_files, builds, "common", common_ccflags)
Compile(main_files, builds, "main", common_ccflags)
Compile(bindings_files, builds, "bindings", bindings_ccflags)
n.newline()

# Link files
binary_extension = ".exe" if sys.platform.startswith("win32") else ""
lib_extension = ".pyd" if sys.platform.startswith("win32") else ".so"

for build in builds:
    # libkinoko
    libkinoko_name = f"libkinoko{build.suffix}.a"
    libkinoko_path = os.path.join("$builddir", libkinoko_name)
    libkinoko_objs = build.obj_files["common"]
    n.build(libkinoko_path, "ar", libkinoko_objs)
    n.newline()

    # kinoko
    exe_name = f"kinoko{build.suffix}{binary_extension}"
    exe_path = os.path.join("$outdir", exe_name)
    exe_obj = build.obj_files["main"] + [libkinoko_path]
    n.build(exe_path, "ld", exe_obj)

    # bindings
    lib_name = f"bindings{build.suffix}{lib_extension}"
    lib_path = os.path.join("$outdir", lib_name)
    lib_objs = build.obj_files["bindings"] + [libkinoko_path]
    
    # Python library
    n.build(lib_path, "ld_shared", lib_objs, variables={"ldflags": f"{sysconfig.get_config_var('LDFLAGS')} -L{os.path.dirname(python_lib_path)} -lpython{sys.version_info.major}.{sys.version_info.minor}"})
    n.newline()

n.rule("configure", command=f"{sys.executable} configure.py", generator=True)
n.build("build.ninja", "configure", implicit=["configure.py", os.path.join("vendor", "ninja_syntax.py")])
n.newline()

with open("build.ninja", "w") as f:
    f.write(out_buf.getvalue())
n.close()