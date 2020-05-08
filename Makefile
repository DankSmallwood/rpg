MAKEFLAGS+=--no-builtin-rules
BUILDDIR=build
PROFILE=release

DEPDIR:=$(BUILDDIR)/$(PROFILE)/deps
DEPFLAGS=-MT $@ -MMD -MP -MF $(DEPDIR)/$*.d -fsyntax-only

TARGET:=$(BUILDDIR)/$(PROFILE)/rpg
.DEFAULT_GOAL=$(TARGET)

CC=clang
CFORMAT=clang-format

CFLAGS=-Wall -std=c11 -Isrc
CFLAGS_debug=-g
CFLAGS_release=-O3
CFLAGS+=$(CFLAGS_$(PROFILE))
CFLAGS+=-I$(BUILDDIR)/$(PROFILE)

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC=$(subst src/,,$(call rwildcard,src,*.c))
OBJ=$(patsubst %.c,$(BUILDDIR)/$(PROFILE)/%.o,$(SRC))

GEN_Y=$(subst src/,,$(call rwildcard,src,*.y))
GEN_Z=$(patsubst %.y,$(BUILDDIR)/$(PROFILE)/%.z,$(GEN_Y))

-include $(wildcard $(DEPFILES))

$(TARGET): $(GEN_Z) $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $@

.SECONDEXPANSION:
$(BUILDDIR)/$(PROFILE)/%.z: src/%.y $$(dir src/%)_all.x
	@echo $@...
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -P -E -x c $< >$@2
	@mv $@2 $@
	@clang-format -i $@

.PHONY:
%/_all.x: build/util/allx
	@echo $@...
	@build/util/allx $(dir $@)

$(DEPDIR)/%.d: $(GEN_Z)
$(DEPDIR)/%.d: src/%.c
	@echo $@...
	@mkdir -p $(dir $@)
	-@$(CC) $(CFLAGS) $(DEPFLAGS) $<

DEPFILES:=$(SRC:%.c=$(DEPDIR)/%.d)
$(DEPFILES):


$(BUILDDIR)/$(PROFILE)/%.o: src/%.c $(DEPDIR)/%.d
	@echo $@...
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/util/%: util/%.c
	@echo $@...
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $^ -o $@

.phony: clean
clean:
	@rm -rf $(BUILDDIR)
