#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
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
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	src $(wildcard src/*)
DATA		:=	data
INCLUDES	:=	src/include

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
ifneq ($(BUILD),$(notdir $(CURDIR)))

#---------------------------------------------------------------------------------
# tool paths
#---------------------------------------------------------------------------------
export TTYDTOOLS := $(CURDIR)/dep/ttyd-tools/ttyd-tools
export ELF2REL_BUILD := $(TTYDTOOLS)/elf2rel/build
export ELF2REL	:=	$(ELF2REL_BUILD)/elf2rel
export GCIPACK	:=	/usr/bin/env python3 $(TTYDTOOLS)/gcipack/gcipack.py

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
export MAPFILE		:= $(CURDIR)/src/include/ttyd.us.lst
export BANNERFILE	:= $(CURDIR)/src/images/banner_us.raw
export ICONFILE		:= $(CURDIR)/src/images/icon_us.raw

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

#---------------------------------------------------------------------------------
$(BUILD): elf2rel
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

# For now, run this phony target every time to build elf2rel
elf2rel:
	@echo "Compiling elf2rel..."
	mkdir -p $(ELF2REL_BUILD)
	cd $(ELF2REL_BUILD) && cmake ..
	$(MAKE) -C $(ELF2REL_BUILD) -f $(ELF2REL_BUILD)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo Cleaning...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol $(OUTPUT).rel $(OUTPUT).gci $(ELF2REL_BUILD)

#---------------------------------------------------------------------------------

.PHONY: $(BUILD) clean elf2rel

else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).gci: $(OUTPUT).rel $(BANNERFILE) $(ICONFILE)
$(OUTPUT).rel: $(OUTPUT).elf $(MAPFILE)
$(OUTPUT).elf: $(LDFILES) $(OFILES)

$(OFILES_SOURCES) : $(HFILES)

# REL linking
%.rel: %.elf
	@echo output ... $(notdir $@)
	@$(ELF2REL) $< -s $(MAPFILE)
	
%.gci: %.rel
	@echo packing ... $(notdir $@)
	@$(GCIPACK) $< "rel" "Paper Mario" "Practice Codes ($(PRINTVER))" $(BANNERFILE) $(ICONFILE) $(GAMECODE)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.jpg.o	%_jpg.h :	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
