VPATH=$(shell find ./src -mindepth 1 -type d -printf '%p:')
src=$(shell find ./src/octal \( -name \*.cpp \) -print)
lib_dir=../lib
obj_dir=obj
objs=$(foreach f,$(src:%.cpp=%.o), $(obj_dir)/$(notdir $f))
inc=-I./src/
targ=$(lib_dir)/liboctal.so
cflags=-g -fdeclspec -fPIC -std=c++20
ldflags=-lc -ldl
defines="-D_DEBUG -DEXPORT"
# defines for platform
platform=linux
ifeq ($(platform), linux)
	ldflags+=-lvulkan -lxcb -lX11 -lX11-xcb
	objs+= $(obj_dir)/linux.o
endif

.PHONY:
	clean

all: $(targ)

clean:
	rm -rf $(obj_dir)/*.o
	rm -rf $(lib_dir)/*.so

$(targ): $(objs)
	clang++ -o $(targ) $(objs) -shared $(cflags) $(defines) $(ldflags)

$(obj_dir)/%.o : %.cpp
	clang++ -c $< -fPIC -o $(obj_dir)/$(notdir $@) $(inc) $(cflags)
