import os

env = Environment(
    ENV = os.environ,
    CPPPATH = ['inkcpp/include', 'inkcpp_compiler/include', 'shared/private', 'shared/public'],
    CPPDEFINES = ['INK_COMPILER']
)

src = Glob("src/*.cpp")
src.extend(Glob("inkcpp_compiler/*.cpp"))
src.extend(Glob("inkcpp/*.cpp"))

env.Program("bin/compiler", src)
