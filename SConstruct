import sys
import os

INCLUDES = [
    '.',
    ]

cross_builder_env = Environment(
    ENV = os.environ,
    CROSS = '/home/rje/angstrom-current/tmp/cross/armv5te/bin/arm-angstrom-linux-gnueabi-',
    AS = '${CROSS}gcc',
    CC = '${CROSS}gcc',
    CXX = '${CROSS}g++',
    OBJCOPY = '${CROSS}objcopy',
    AR = '${CROSS}ar',
    RANLIB = '${CROSS}ranlib',
    SIZE = '${CROSS}size',
    DEBUG= '-ggdb',
    OPTIM= '-O2',
    CPPPATH=INCLUDES,
    CXXFLAGS = "$CCFLAGS",
    CCFLAGS = "-Wall $DEBUG $OPTIM",
    ASFLAGS="-c $CCFLAGS",
    LINKFLAGS = "$CCFLAGS",
    LIBS = [],
    LIBPATH = [],
    )

env = cross_builder_env.Clone()

env.Program('i2c_write','i2c_write.c')
env.Program('i2c_read','i2c_read.c')
env.Program('i2c_readbyte','i2c_readbyte.c')
env.Program('mem','mem.cpp')



