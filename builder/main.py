import sys
from os.path import join

from SCons.Script import (ARGUMENTS, COMMAND_LINE_TARGETS, AlwaysBuild,
                          Default, DefaultEnvironment)


env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()

env.Replace(
    AR="riscv-nuclei-elf-gcc-ar",
    AS="riscv-nuclei-elf-as",
    CC="riscv-nuclei-elf-gcc",
    GDB="riscv-nuclei-elf-gdb",
    CXX="riscv-nuclei-elf-g++",
    OBJCOPY="riscv-nuclei-elf-objcopy",
    RANLIB="riscv-nuclei-elf-gcc-ranlib",
    SIZETOOL="riscv-nuclei-elf-size",

    ARFLAGS=["rc"],

    SIZEPRINTCMD='$SIZETOOL -d $SOURCES',

    PROGSUFFIX=".elf"
)

# Allow user to override via pre:script
if env.get("PROGNAME", "program") == "program":
    env.Replace(PROGNAME="firmware")

env.Append(
    BUILDERS=dict(
        ElfToBin=Builder(
            action=env.VerboseAction(" ".join([
                "$OBJCOPY",
                "-O",
                "binary",
                "$SOURCES",
                "$TARGET"
            ]), "Building $TARGET"),
            suffix=".bin"
        ),
        ElfToHex=Builder(
            action=env.VerboseAction(" ".join([
                "$OBJCOPY",
                "-O",
                "ihex",
                "$SOURCES",
                "$TARGET"
            ]), "Building $TARGET"),
            suffix=".hex"
        )
    )
)

pioframework = env.get("PIOFRAMEWORK", [])

if not pioframework:
    env.SConscript("frameworks/_bare.py", exports="env")

#
# Target: Build executable and linkable firmware
#

target_elf = None
if "nobuild" in COMMAND_LINE_TARGETS:
    target_elf = join("$BUILD_DIR", "${PROGNAME}.elf")
    target_firm = join("$BUILD_DIR", "${PROGNAME}.bin")
    target_hex = join("$BUILD_DIR", "${PROGNAME}.hex")
else:
    target_elf = env.BuildProgram()
    target_firm = env.ElfToBin(join("$BUILD_DIR", "${PROGNAME}"), target_elf)
    target_hex = env.ElfToHex(join("$BUILD_DIR", "${PROGNAME}"), target_elf)

AlwaysBuild(env.Alias("nobuild", target_firm))
target_buildprog = env.Alias("buildprog", target_firm, target_firm)
target_buildhex = env.Alias("buildhex", target_hex, target_hex)

#
# Target: Print binary size
#

target_size = env.Alias(
    "size", target_elf,
    env.VerboseAction("$SIZEPRINTCMD", "Calculating size $SOURCE"))
AlwaysBuild(target_size)


#
# Target: Upload by default .elf file
#
upload_protocol = env.subst("$UPLOAD_PROTOCOL")
upload_source = target_firm
upload_actions = []

upload_script = env.GetProjectOption("openocd_cfg","")
if not upload_script:
    upload_script = join(platform.get_dir(), "misc", env.BoardConfig().id + ".openocd.cfg")

openocd_args = [
    "-f", '"' + upload_script + '"',
    "-c", "init; halt;",
    # "-c", "program {$SOURCE}; softreset; exit 0;"
    "-c", "program {$SOURCE}; exit 0;"
]
env.Replace(
    UPLOADER="openocd",
    UPLOADERFLAGS=openocd_args,
    UPLOADCMD="$UPLOADER $UPLOADERFLAGS")
upload_source = target_elf
upload_actions = [env.VerboseAction("$UPLOADCMD", "Uploading $SOURCE")]


AlwaysBuild(env.Alias("upload", upload_source, upload_actions))


#
# Setup default targets
#

Default([target_buildprog, target_buildhex, target_size])
