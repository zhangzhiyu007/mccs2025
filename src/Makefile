#------------------------------------------------------------------------------#
# Modernized legacy Makefile (OpenWrt ARM friendly, C++98 constrained)
#------------------------------------------------------------------------------#

# Toolchain prefix, e.g. CROSS=arm-openwrt-linux-gnueabi-
CROSS ?=

CC = $(if $(strip $(CROSS)),$(CROSS)gcc,gcc)
CXX = $(if $(strip $(CROSS)),$(CROSS)g++,g++)
AR = $(if $(strip $(CROSS)),$(CROSS)ar,ar)
LD = $(CXX)
STRIP = $(if $(strip $(CROSS)),$(CROSS)strip,strip)

# Verbose control: default quiet, use V=1 for full command lines.
V ?= 0
ifeq ($(V),1)
Q :=
ECHO_CC := @true
ECHO_CXX := @true
ECHO_LD := @true
else
Q := @
ECHO_CC := @echo "  CC  $<"
ECHO_CXX := @echo "  CXX $<"
ECHO_LD := @echo "  LD  $@"
endif

# Output layout
OBJDIR_RELEASE := ../OBJ
OUT_DIR_RELEASE := ../Release
OUT_RELEASE := $(OUT_DIR_RELEASE)/mcss

# Vendor SDK layout (override if needed)
API_DIR ?= ../api
API_INCDIR ?= $(API_DIR)/inc
API_LIBDIR ?= $(API_DIR)/lib
USE_API_LIBS ?= 1
API_LINK_LIBS ?= -lsocketapi -lEMFU_ARM_ODM
API_LDFLAGS ?=

# Optional OpenWrt staging/sysroot hint.
# Example: make STAGING_DIR=/path/to/openwrt/staging_dir/target-...
SYSROOT_FLAG :=
ifneq ($(strip $(STAGING_DIR)),)
SYSROOT_FLAG := --sysroot=$(STAGING_DIR)
endif

# Source collection: stable relative paths, no ./ prefix ambiguity
CPP_SRCS := $(shell find . -type f -name '*.cpp' | LC_ALL=C sort)
C_SRCS := $(shell find . -type f -name '*.c' | LC_ALL=C sort)
SRCS := $(CPP_SRCS) $(C_SRCS)
SRCS_NODOT := $(patsubst ./%,%,$(SRCS))

OBJ_RELEASE := $(addprefix $(OBJDIR_RELEASE)/,$(SRCS_NODOT:.cpp=.o))
OBJ_RELEASE := $(OBJ_RELEASE:.c=.o)
DEP_RELEASE := $(OBJ_RELEASE:.o=.d)

# Flags split by language role
CPPFLAGS += $(SYSROOT_FLAG) -I$(API_INCDIR)
CFLAGS += -Wall -O2 -MMD -MP
CXXFLAGS += -Wall -O2 -fexceptions -std=gnu++98 -MMD -MP
LDFLAGS += $(SYSROOT_FLAG) -s

# Link libs: keep dependency order left->right
BASE_LIBS := -lpthread -ldl -lrt
API_LIBS :=
ifeq ($(USE_API_LIBS),1)
API_LIBS += -L$(API_LIBDIR) $(API_LDFLAGS) $(API_LINK_LIBS)
endif
LDLIBS += $(BASE_LIBS) $(API_LIBS)

all: release

.PHONY: all clean release print-config

ifeq ($(USE_API_LIBS),1)
release: $(OUT_RELEASE)
else
release: $(OBJ_RELEASE)
	@echo "USE_API_LIBS=0: object compilation completed, skip final link."
endif

$(OUT_RELEASE): $(OBJ_RELEASE)
	$(ECHO_LD)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(LD) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(OBJDIR_RELEASE)/%.o: %.cpp
	$(ECHO_CXX)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJDIR_RELEASE)/%.o: %.c
	$(ECHO_CC)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(Q)rm -rf $(OUT_DIR_RELEASE) $(OBJDIR_RELEASE)

print-config:
	@echo "CROSS=$(CROSS)"
	@echo "CC=$(CC)"
	@echo "CXX=$(CXX)"
	@echo "LD=$(LD)"
	@echo "STAGING_DIR=$(STAGING_DIR)"
	@echo "API_INCDIR=$(API_INCDIR)"
	@echo "API_LIBDIR=$(API_LIBDIR)"
	@echo "USE_API_LIBS=$(USE_API_LIBS)"
	@echo "API_LINK_LIBS=$(API_LINK_LIBS)"

-include $(DEP_RELEASE)
