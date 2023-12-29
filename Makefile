N64_INST := /opt/libdragon
N64_EMULATOR := /Applications/ares.app/Contents/MacOS/ares
ROM_NAME := BulletDemo

$(info $$N64_INST directory is: [${N64_INST}])

BUILD_DIR=build
include $(N64_INST)/include/n64.mk

assets_ttf = $(wildcard assets/*.ttf)
assets_png = $(wildcard assets/*.png)

assets_conv = $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
              $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite)))
OBJS = $(BUILD_DIR)/lib/microui.o \
	$(BUILD_DIR)/lib/microuiN64.o

filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -s 9 -o filesystem "$<"

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"
# add the header files
header = $(wildcard src/*.hpp) $(wildcard lib/*.h)
src = $(wildcard src/*.cpp) $(wildcard lib/*.c)

all: $(ROM_NAME).z64

$(BUILD_DIR)/$(ROM_NAME).dfs: $(assets_conv)
$(BUILD_DIR)/$(ROM_NAME).elf: $(src:%.c=$(BUILD_DIR)/%.o) $(src:%.cpp=$(BUILD_DIR)/%.o) $(OBJS)

## Ad the .h files to the include path, specifically src/asset/cube.h
INCLUDE+=-I/opt/libdragon/include -Iinclude -Isrc
MAPCMD+=-Map
CFLAGS+=-I/opt/libdragon/include -I/opt/libdragon/include/bullet -std=c11
CXXFLAGS+=-Isrc -I/opt/libdragon/include -I/opt/libdragon/include/bullet -std=c++14
LDFLAGS+=-g -lstdc++ -L/opt/libdragon/lib -ldragon -lc -lm -ldragonsys,--start-group -lBullet3Common -lBulletDynamics -lBulletCollision -lBulletInverseDynamics -lBulletSoftBody -lLinearMath,--end-group,--gc-sections


$(ROM_NAME).z64: N64_ROM_TITLE="$(ROM_NAME)"
$(ROM_NAME).z64: $(BUILD_DIR)/$(ROM_NAME).dfs

run: $(ROM_NAME).z64
	$(N64_EMULATOR) $(ROM_NAME).z64

clean:
	rm -rf $(BUILD_DIR) filesystem $(ROM_NAME).z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean