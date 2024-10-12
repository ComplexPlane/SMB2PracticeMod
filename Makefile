ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

RUST_BUILD_DIR := $(CURDIR)/target/powerpc-unknown-eabi/release
ELF2REL_BUILD := $(CURDIR)/3rdparty/ttyd-tools/ttyd-tools/elf2rel/build
ELF2REL := $(ELF2REL_BUILD)/elf2rel
GCIPACK := $(CURDIR)/3rdparty/ttyd-tools/ttyd-tools/gcipack/gcipack.py
BANNERFILE := $(CURDIR)/images/banner_us.raw
ICONFILE := $(CURDIR)/images/icon_us.raw

LINKER_FLAGS := -r -e _prolog -u _prolog -u _epilog -u _unresolved -Wl,--gc-sections -nostdlib -g -mno-sdata -mgcn -DGEKKO -mcpu=750 -meabi -mhard-float

all: SMB2PracticeMod.gci

.PHONY: elf2rel
elf2rel:
	@echo "Compiling elf2rel..."
	mkdir -p $(ELF2REL_BUILD)
	cd $(ELF2REL_BUILD) && cmake ..
	$(MAKE) -C $(ELF2REL_BUILD) -f $(ELF2REL_BUILD)/Makefile

src/mkb.rs: cppsrc/mkb/mkb2_ghidra.h
	bindgen $< --use-core -o $@ -- --target=powerpc-unknown-eabi

$(RUST_BUILD_DIR)/libsmb2_practice_mod.a: src/mkb.rs
	cargo +nightly build -Z build-std=core,alloc --target powerpc-unknown-eabi.json --release

SMB2PracticeMod.elf: $(RUST_BUILD_DIR)/libsmb2_practice_mod.a
	$(DEVKITPPC)/bin/powerpc-eabi-gcc $(LINKER_FLAGS) -o $@ $<

SMB2PracticeMod.rel: elf2rel SMB2PracticeMod.elf
	$(ELF2REL) SMB2PracticeMod.elf -s $(CURDIR)/cppsrc/mkb/mkb2.us.lst --rel-version 2 --rel-id 101

SMB2PracticeMod.gci: SMB2PracticeMod.rel
	python3 $(GCIPACK) $< rel "Super Monkey Ball 2" "SMB2 Practice Mod" $(BANNERFILE) $(ICONFILE) GM2E8P

.PHONY: clean
clean:
	@cargo clean
	@rm -f SMB2PracticeMod.elf SMB2PracticeMod.rel SMB2PracticeMod.gci
	@rm -rf $(ELF2REL_BUILD)
