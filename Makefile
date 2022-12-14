ARTIFACT = spark_demo_app

#Build architecture/variant string, possible values: x86, armv7le, etc...
PLATFORM ?= aarch64

#Build profile, possible values: release, debug, profile, coverage
BUILD_PROFILE ?= release

CONFIG_NAME ?= $(PLATFORM)-$(BUILD_PROFILE)
OUTPUT_DIR = build/$(CONFIG_NAME)
TARGET = $(OUTPUT_DIR)/$(ARTIFACT)

#Compiler definitions

CC = qcc -lang-c -Vgcc_nto$(PLATFORM)
CXX = qcc -lang-c++ -std=c++11 -Vgcc_ntoaarch64 -EL -DVARIANT_le -DBUILDENV_qss -DGFX3D_DEBUG_MSG -Vgcc_ntoaarch64 -c -Wc,-Wall -Wc,-fpermissive  -Wc,-Wno-unused-variable

LD = $(CC)

#User defined include/preprocessor flags and libraries
INCLUDES += -Iinclude -Ithird_party/glm/include
LIBS += -lscreen -lGLESv2 -lEGL -lm -lstdc++ -lc

#Compiler flags for build profiles
CCFLAGS_release += -O2
CCFLAGS_debug += -g -O0 -DRC_DEBUG_MSG -DRC_PROFILE
#Generic compiler flags (which include build type flags)
CCFLAGS_all += -Wall -fmessage-length=0 -D_QNX_SOURCE -Vgcc_ntoaarch64  -Wc,-Wall -Wc,-fpermissive -Wall -EL -DVARIANT_le -DBUILDENV_qss -DRC_MTHREADS=3
CCFLAGS_all += $(CCFLAGS_$(BUILD_PROFILE))
#Shared library has to be compiled with -fPIC
#CCFLAGS_all += -fPIC
LDFLAGS_all += $(LDFLAGS_$(BUILD_PROFILE))
LIBS_all += $(LIBS_$(BUILD_PROFILE)) -lang-c++ -std=c++14 -Vgcc_ntoaarch64 -EL -DVARIANT_le -DBUILDENV_qss -DGFX3D_DEBUG_MSG -DVARIANT_g 
DEPS = -Wp,-MMD,$(@:%.o=%.d),-MT,$@

#Macro to expand files recursively: parameters $1 -  directory, $2 - extension, i.e. cpp
rwildcard = $(wildcard $(addprefix $1/*.,$2)) $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2))

#Source list
#SRCS = gears.c
SRCS += $(call rwildcard, src, c cpp)
SRCS += demos/src/RainDemo.cpp

#Object files list
OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o, $(basename $(SRCS))))

#Compiling rule
$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<
$(OUTPUT_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<

#Compiling rule
#$(OUTPUT_DIR)/%.o: %.cpp
#	@mkdir -p $(dir $@)
#   $(CXX) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $<

#Linking rule
$(TARGET):$(OBJS)
	$(LD) -o $(TARGET) $(LDFLAGS_all) $(LDFLAGS) $(OBJS) $(LIBS_all) $(LIBS)

#Rules section for default compilation and linking
all: $(TARGET)

clean:
	rm -fr $(OUTPUT_DIR)

rebuild: clean all

#Inclusion of dependencies (object files to source and includes)
-include $(OBJS:%.o=%.d)