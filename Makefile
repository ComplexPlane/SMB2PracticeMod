#---------------------------------------------------------------------------------
# Change directory into build dir
#---------------------------------------------------------------------------------

# If we're in the repository root
ifeq ($(wildcard Makefile),Makefile)

all: %
%:
	mkdir -p $(CURDIR)/build
	$(MAKE) $(MAKECMDGOALS) -C $(CURDIR)/build -f $(CURDIR)/Makefile

.PHONY: all

# If we're in the build dir
else

#---------------------------------------------------------------------------------
# Set up DevKitPPC
#---------------------------------------------------------------------------------

# Clear the implicit built in rules
.SUFFIXES:

ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/gamecube_rules

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

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
ROOT		:=	$(abspath $(CURDIR)/..)
TARGET		:=	$(notdir $(ROOT))
SOURCES		:=	$(ROOT)/src $(wildcard $(ROOT)/src/*)
DATA		:=	data
INCLUDES	:=	$(ROOT)/src/include

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

MACHDEP		= -mno-sdata -mgcn -DGEKKO -mcpu=750 -meabi -mhard-float

# TODO remove TTYD_US define
CFLAGS		= -nostdlib -ffreestanding -ffunction-sections -fdata-sections -g -Os -Wall -Werror $(MACHDEP) $(INCLUDE) -DTTYD_US
CXXFLAGS	= -fno-exceptions -fno-rtti -std=gnu++17 $(CFLAGS)

FUNCWRAPS	= -Wl,--wrap,sprintf -Wl,--wrap,strcmp -Wl,--wrap,strncmp -Wl,--wrap,strcpy -Wl,--wrap,strncpy -Wl,--wrap,strcat -Wl,--wrap,strlen -Wl,--wrap,memcpy -Wl,--wrap,memset -Wl,--wrap,sysMsec2Frame -Wl,--wrap,keyGetButton -Wl,--wrap,keyGetButtonTrg -Wl,--wrap,seqGetSeq -Wl,--wrap,seqGetNextSeq -Wl,--wrap,swByteGet -Wl,--wrap,swByteSet -Wl,--wrap,swClear -Wl,--wrap,swSet -Wl,--wrap,marioStGetSystemLevel -Wl,--wrap,marioStSystemLevel -Wl,--wrap,marioGetPartyId -Wl,--wrap,marioGetExtraPartyId -Wl,--wrap,marioGetPtr -Wl,--wrap,partyGetPtr -Wl,--wrap,pouchGetPtr -Wl,--wrap,btlGetScreenPoint -Wl,--wrap,evtGetWork -Wl,--wrap,eventStgNum -Wl,--wrap,eventStgDtPtr -Wl,--wrap,winGetPtr -Wl,--wrap,winOpenEnable -Wl,--wrap,CARDClose -Wl,--wrap,DCFlushRange -Wl,--wrap,ICInvalidateRange -Wl,--wrap,__udivdi3 -Wl,--wrap,__umoddi3

LDFLAGS		= -r -e _prolog -u _prolog -u _epilog -u _unresolved -Wl,--gc-sections -nostdlib -g $(MACHDEP) -Wl,-Map,$(notdir $@).map $(FUNCWRAPS)

# Platform options
GAMECODE = "G8ME"
PRINTVER = "US"

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

#---------------------------------------------------------------------------------
# tool paths
#---------------------------------------------------------------------------------
TTYDTOOLS := $(ROOT)/dep/ttyd-tools/ttyd-tools
ELF2REL_BUILD := $(TTYDTOOLS)/elf2rel/build
ELF2REL	:=	$(ELF2REL_BUILD)/elf2rel
GCIPACK	:=	/usr/bin/env python3 $(TTYDTOOLS)/gcipack/gcipack.py

OUTPUT	:=	$(ROOT)/$(TARGET)

VPATH	:=	$(SOURCES) $(foreach dir,$(DATA),$(CURDIR)/$(dir))

DEPSDIR	:=	$(CURDIR)/$(BUILD)

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

OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

# For REL linking
LDFILES		:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.ld)))
MAPFILE		:= $(ROOT)/src/include/ttyd.us.lst
BANNERFILE	:= $(ROOT)/src/images/banner_us.raw
ICONFILE		:= $(ROOT)/src/images/icon_us.raw

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR) \
			-I$(LIBOGC_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
			-L$(LIBOGC_LIB)

OUTPUT	:=	$(CURDIR)/$(TARGET)

#---------------------------------------------------------------------------------

################

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).gci: $(OUTPUT).rel $(BANNERFILE) $(ICONFILE)
	@echo packing ... $(notdir $@)
	@$(GCIPACK) $(OUTPUT).rel "rel" "Paper Mario" "Practice Codes ($(PRINTVER))" $(BANNERFILE) $(ICONFILE) $(GAMECODE)

# REL linking
$(OUTPUT).rel: $(OUTPUT).elf $(MAPFILE) elf2rel
	@echo output ... $(notdir $@)
	$(ELF2REL) $(OUTPUT).elf -s $(MAPFILE)

$(OUTPUT).elf: $(LDFILES) $(OFILES)

$(OFILES_SOURCES) : $(HFILES)

# For now, make elf2rel a phony target
elf2rel:
	@echo "Compiling elf2rel..."
	mkdir -p $(ELF2REL_BUILD)
	cd $(ELF2REL_BUILD) && cmake ..
	$(MAKE) -C $(ELF2REL_BUILD) -f $(ELF2REL_BUILD)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo Cleaning...
	@rm -fr $(CURDIR) $(OUTPUT).elf $(OUTPUT).dol $(OUTPUT).rel $(OUTPUT).gci $(ELF2REL_BUILD)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.jpg.o	%_jpg.h :	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

.PHONY: clean elf2rel

endif
