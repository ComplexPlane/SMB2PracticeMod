ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

RUST_BUILD_DIR := $(CURDIR)/target/powerpc-unknown-eabi/release
ELF2REL_BUILD := $(CURDIR)/3rdparty/ttyd-tools/ttyd-tools/elf2rel/build
ELF2REL := $(ELF2REL_BUILD)/elf2rel
GCIPACK := $(CURDIR)/3rdparty/ttyd-tools/ttyd-tools/gcipack/gcipack.py
BANNERFILE := $(CURDIR)/images/banner_us.raw
ICONFILE := $(CURDIR)/images/icon_us.raw

CARGO_BASE := -Z build-std=core,alloc,panic_abort -Z build-std-features=panic_immediate_abort --target powerpc-unknown-eabi.json --release --no-default-features
RUSTFLAGS := -Zlocation-detail=none -Zfmt-debug=none
LINKER_FLAGS := -r -e _prolog -u _prolog -u _epilog -u _unresolved -Wl,--gc-sections -nostdlib -g -mno-sdata -mgcn -DGEKKO -mcpu=750 -meabi -mhard-float

all: mkb1 mkb2

mkb1: GAME_CODE := "GMBE8P"
mkb1: CARGO_FEATURES := "mkb1"
mkb1: SYMBOLS_FILE := "$(CURDIR)/src/mkb1/supermonkeyball.lst"
mkb1: SMB1PracticeMod.gci

mkb2: GAME_CODE := "GM2E8P"
mkb2: CARGO_FEATURES := "mkb2"
mkb2: SYMBOLS_FILE := "$(CURDIR)/src/mkb2/mkb2.us.lst"
mkb2: SMB2PracticeMod.gci

debug: RUSTFLAGS := ""
debug: CARGO_BASE := -Z build-std=core,alloc --target powerpc-unknown-eabi.json --release --no-default-features
debug: mkb1 mkb2

.PHONY: elf2rel
elf2rel:
	@echo "Compiling elf2rel..."
	mkdir -p $(ELF2REL_BUILD)
	cd $(ELF2REL_BUILD) && cmake ..
	$(MAKE) -C $(ELF2REL_BUILD) -f $(ELF2REL_BUILD)/Makefile

.PHONY: %.a
%.a:
	RUSTFLAGS="$(RUSTFLAGS)" cargo +nightly build $(CARGO_BASE) --features $(CARGO_FEATURES)
	@mv $(RUST_BUILD_DIR)/libsmb2_practice_mod.a $@

%.elf: %.a
	@$(DEVKITPPC)/bin/powerpc-eabi-gcc $(LINKER_FLAGS) -o $@ $<

.PRECIOUS: %.rel # I like to keep an eye on exact REL sizes
%.rel: %.elf elf2rel
	@$(ELF2REL) $< -s $(SYMBOLS_FILE) --rel-version 2 --rel-id 101

%.gci: %.rel
	@python3 $(GCIPACK) $< rel "Super Monkey Ball 2" "SMB2 Practice Mod" $(BANNERFILE) $(ICONFILE) $(GAME_CODE)

.PHONY: fix
fix:
	RUSTFLAGS="$(RUSTFLAGS)" cargo +nightly fix $(CARGO_BASE) --allow-dirty

.PHONY: clean
clean:
	@cargo clean
	@rm -f *.elf *.rel *.gci
	@rm -rf $(ELF2REL_BUILD)
	@rm -rf $(CURDIR)/build
