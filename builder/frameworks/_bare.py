from SCons.Script import Import
from os.path import join

Import("env")

board = env.BoardConfig()

env.Append(

    ASFLAGS = ["-x", "assembler-with-cpp"],

    CCFLAGS=[
        # CPU flags
        "-march=rv32emac",
        "-mabi=ilp32e",
        "-mcmodel=medany",
        "-msmall-data-limit=8",

        # Default compiler configuration
        "-fmessage-length=0",
        "-fsigned-char",
        "-ffunction-sections",
        "-fdata-sections",
        "-fno-common",

        # Error & Warning
        "-Wall",
        "-Werror=all",
        "-Wno-error=unused-function",
        "-Wno-error=unused-but-set-variable",
        "-Wno-error=unused-variable",
        "-Wno-error=deprecated-declarations",
        "-Wextra",
        "-Wno-unused-parameter",
        "-Wno-sign-compare",

        #Debug & optimize flags
        "-O2"
    ],

    CFLAGS = [
        "-std=gnu11"
    ],

    CXXFLAGS = [
        "-std=gnu++17"
    ],

    CPPDEFINES = [
    ],

    LINKFLAGS=[
        "-march=rv32emac",
        "-mabi=ilp32e",
        "-mcmodel=medany",
        "-msmall-data-limit=8",
        "-nostartfiles",
        "-Wl,--gc-sections",
        "--specs=nano.specs"
    ],

    LIBS=["c"]
)

ldscript = env.GetProjectOption("ldscript","")
if not ldscript:
    ldscript = join(env.PioPlatform().get_dir(), "misc", env.BoardConfig().id + ".lds")
    
env.Replace( LDSCRIPT_PATH=ldscript )

# copy CCFLAGS to ASFLAGS (-x assembler-with-cpp mode)
env.Append(ASFLAGS=env.get("CCFLAGS", [])[:])
