#
# makefile for h5000-logger project
#

#################################################################################
# PREAMBLE
#################################################################################

# Identify the root directory (with src, include, build, etc. directories)
# (This assumes the makefile is in ./build/gcc/)
#PROJDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../../)
PROJDIR := $(CURDIR)/../..
export PROJDIR
OBJDIR	:= $(PROJDIR)/.obj
BINDIR	:= $(PROJDIR)

# Compile and link flag definitions
COMMON_CFLAGS	:= -Wall -Wno-unknown-pragmas -Wno-format -Werror -Wno-error=sequence-point -Wno-sequence-point -Wno-psabi
DEBUG_CFLAGS    := -Wno-inline -ggdb -DDEBUG
RELEASE_CFLAGS  := -O3 -DNDEBUG
DEBUG_LDFLAGS	:= -g
GITVERSION	:=$(shell git describe --long --tags --dirty 2>/dev/null | sed s/^v//)

####
#### TODO: Allow release builds
####
CFLAGS += -c $(COMMON_CFLAGS) $(DEBUG_CFLAGS)
LDFLAGS += $(COMMON_LDFLAGS) $(DEBUG_LDFLAGS)

#################################################################################
# RECIPES
#################################################################################
$(OBJDIR)/%.o : %.cpp
	@# show target name (without path)
	@echo "Building $(@F)"
	@# compile the specified cpp file ('$<'), including a .d file as output
	@$(CXX) -MMD $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -fPIC -O0 -o $@ $<

#################################################################################
# PROJECT-SPECIFIC BITS
#################################################################################
APP_NAME := h5000-logger

# Identify source folders (will compile all .cpp files in these folders)
SOURCES	:= \
	$(notdir $(wildcard $(PROJDIR)/src/*.cpp)) \
	$(notdir $(wildcard $(PROJDIR)/src/json/*.cpp))

VPATH	= \
	$(PROJDIR)/src:\
	$(PROJDIR)/src/json

# Library dependencies (for link time)
LIBS		:= \
	-lpthread

# Include directories (for compile time)
INCLUDES	+= \
			-I $(PROJDIR)/include

APPDIR	:= $(BINDIR)


#################################################################################
# POSTAMBLE AND TARGETS
#################################################################################

ifeq ($(MAKECMDGOALS),clean)
    # doing a clean, so no output directory creation necessary
else
dummy := $(shell test -d $(OBJDIR) || mkdir -p $(OBJDIR))
dummy := $(shell test -d $(BINDIR) || mkdir -p $(BINDIR))
endif

# Build the application
$(APPDIR)/$(APP_NAME): $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
	@echo 'Building $(@F)'
	@$(CXX) $(LINKFLAGS) $(LIB_PATHS) -Wl,-rpath,'$$ORIGIN' -o $@ $+ $(LIBS)
	@echo 'Finished'
	@echo ' '

# "all" target
default: all

all: printversion $(BINDIR)/$(APP_NAME) printcomplete

printversion:
	@echo "Building $(APP_NAME) version $(GITVERSION)"

printcomplete:
	@echo "Completed successfully"

clean:
	@echo "Cleaning files in $(OBJDIR) and target in $(BINDIR)"
	-$(RM) -r $(OBJDIR)
	-$(RM) $(BINDIR)/$(APP_NAME)
	-@echo ' '

# Incorporate dependency information from previous 'makes' (if available)
-include $(patsubst %.cpp,$(OBJDIR)/%.d,$(SOURCES))

.PHONY:	all clean dependents
.SECONDARY: 
