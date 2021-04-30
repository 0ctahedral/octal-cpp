src=$(shell find ./src/octal \( -name \*.cpp \) -print)
obj=$(src:%.cpp=%.o)
lib_dir=../lib
obj_dir=obj
obj_files=$(foreach f,$(obj), $(obj_dir)/$(notdir $f))
inc=-I./src/
targ=octal
cflags=-g -fdeclspec -fPIC -std=c++20
ldflags=-lc -ldl
defines="-D_DEBUG -DEXPORT"

# defines for platform
platform=linux
ifeq ($(platform), linux)
	plat_flags=-lvulkan -lxcb -lX11 -lX11-xcb
endif

.PHONY:
	clean

all: engine

clean:
	rm -rf $(obj_dir)/*.o
	rm -rf $(lib_dir)/*.so

engine: $(obj) $(platform)
	clang++ -o $(lib_dir)/lib$(targ).so $(obj_dir)/*.o -shared $(cflags) $(defines) $(ldflags) $(plat_flags)

linux:
	clang++ -c src/platform/linux/linux.cpp -o $(obj_dir)/$@.o $(inc) $(cflags)

%.o : %.cpp
	clang++ -c $< -fPIC -o $(obj_dir)/$(notdir $@) $(inc) $(cflags)
