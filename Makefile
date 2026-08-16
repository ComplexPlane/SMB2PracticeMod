#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/gamecube_rules

ifeq ($(REGION),)

us:
	@$(MAKE) --no-print-directory REGION=us
clean:
	@$(MAKE) --no-print-directory clean_target REGION=us

# Unexport some compiler vars exported by devkitppc as they interfere
# when we build elf2rel, which uses the system compiler
unexport AS
unexport CC
unexport CXX
unexport AR
unexport OBJCOPY
unexport STRIP
unexport NM
unexport RANLIB

.PHONY: all clean us

else

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	SMB2PracticeMod
BUILD		:=	build
# Find command is invalid during recursive Make execution in build dir, ust ignore it for now
SOURCES		:=	$(shell find src 2>/dev/null)
DATA		:=	data  
INCLUDES	:=	src

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

MACHDEP		= -mno-sdata -mgcn -DGEKKO -mcpu=750 -meabi -mhard-float

GIT_HASH := $(shell git rev-parse --short=8 HEAD 2>/dev/null || echo unknown)
ifneq ($(strip $(shell git status --porcelain 2>/dev/null)),)
GIT_HASH := $(GIT_HASH)-dirty
endif

# -Wno-write-strings because some GC SDK functions take non-const char *,
# and Ghidra can't represent const char * anyhow
# -fmacro-prefix-map makes __FILE__ macro use filepaths relative to the source dir
CFLAGS		= -nostdlib -ffreestanding -ffunction-sections -fdata-sections -g -Os \
			  -Wall -Wno-write-strings -Wno-address-of-packed-member \
			  -Werror -Wshadow -Wimplicit-fallthrough \
			  -fmacro-prefix-map=$(abspath $(CURDIR)/../src)=. -DGIT_HASH=\"$(GIT_HASH)\" \
			  $(MACHDEP) $(INCLUDE)
CXXFLAGS	= -fno-exceptions -fno-rtti -std=c++20 $(CFLAGS)
ASFLAGS     = -mregnames # Don't require % in front of register names

LDFLAGS		= -r -e _prolog -u _prolog -u _epilog -u _unresolved -Wl,--gc-sections -nostdlib -g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
# Temporarily remove the math library to avoid conflicts
# LIBS	:= -lm
LIBS	:= 

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

# For REL linking
export LDFILES		:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.ld)))
export MAPFILE		:= $(CURDIR)/src/mkb/mkb2.$(REGION).lst
# Symbols added by this mod, kept separate so re-exporting the map above
# does not clobber them. Concatenated with the map before elf2rel runs.
export EXTMAPFILE	:= $(CURDIR)/src/mkb/mkb2.$(REGION).ext.lst
export BANNERFILE	:= $(CURDIR)/images/banner_us.raw
export ICONFILE		:= $(CURDIR)/images/icon_us.raw

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD) \
			-I$(LIBOGC_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
			-L$(LIBOGC_LIB)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean_target

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean_target:
	@echo clean ... $(GAMECODE)
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol $(OUTPUT).rel $(OUTPUT).gci

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).gci: $(OUTPUT).rel $(BANNERFILE) $(ICONFILE) gcipack
$(OUTPUT).rel: $(OUTPUT).elf $(MAPFILE) $(EXTMAPFILE) elf2rel
$(OUTPUT).elf: $(LDFILES) $(OFILES)

$(OFILES_SOURCES) : $(HFILES)

# Keep the commit shown on the About screen current across incremental builds.
.PHONY: force_git_hash
menu_defn.o: force_git_hash

# REL linking
%.rel: %.elf
	@echo elf2rel ... $(notdir $@)
	@{ cat $(MAPFILE); echo; cat $(EXTMAPFILE); } > combined.lst
	@$(ELF2REL) $< combined.lst $@ 101 2
	
%.gci: %.rel
	@echo gcipack ... $(notdir $@)
	@$(GCIPACK) $< "rel" "Super Monkey Ball 2" "SMB2 Practice Mod" $(BANNERFILE) $(ICONFILE) GM2E8P
	
#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.jpg.o	%_jpg.h :	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
# build tools
#---------------------------------------------------------------------------------

HOST_CXX ?= c++
ELF2REL_SRC := $(CURDIR)/../3rdparty/elf2rel
ELF2REL := $(CURDIR)/elf2rel
ELF2REL_HEADERS := $(ELF2REL_SRC)/elf2rel.h $(wildcard $(ELF2REL_SRC)/elfio/*.hpp)

elf2rel: $(ELF2REL)

$(ELF2REL): $(ELF2REL_SRC)/elf2rel.cpp $(ELF2REL_HEADERS)
	@echo "Compiling elf2rel..."
	@mkdir -p $(CURDIR)/build
	@$(HOST_CXX) -std=c++20 -O2 -I$(ELF2REL_SRC) $< -o $@

GCIPACK := $(CURDIR)/gcipack
gcipack: $(GCIPACK)
$(GCIPACK): $(CURDIR)/../3rdparty/gcipack/gcipack.cpp
	@echo "Compiling gcipack..."
	@mkdir -p $(CURDIR)/build
	@$(HOST_CXX) -std=c++20 -O2 $< -o $@

.PHONY: elf2rel gcipack

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
endif
