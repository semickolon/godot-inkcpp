#!/usr/bin/env python
import os

libname = "libinkcpp"

env = SConscript("godot-cpp/SConstruct")

env.Append(ENV=os.environ)
env.Append(CPPPATH=["inkcpp/include", "shared/private", "shared/public"])
env.Append(CPPDEFINES=["INK_ENABLE_GODOT"])

sources = Glob("src/*.cpp") + Glob("inkcpp/*.cpp")

if env["platform"] == "macos":
    platlibname = "{}.{}.{}".format(libname, env["platform"], env["target"])
    library = env.SharedLibrary(
        "bin/{}.framework/{}".format(platlibname, platlibname),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "bin/{}{}{}".format(libname, env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
