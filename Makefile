MAKEFLAGS += --no-print-directory -j
.RECIPEPREFIX := >

PREMAKE_WORKSPACE_ROOT := src
PREMAKE_CONFIGURATIONS := debug release asan test

PMAKE = $(MAKE) -C $(PREMAKE_WORKSPACE_ROOT)

define SPECIAL_TARGETS :=
  default
  configure
  fclean
endef

.DEFAULT_GOAL := default

.PHONY: $(SPECIAL_TARGETS)

configure:
> premake5 gmake2

default: configure
> @$(PMAKE)

fclean: configure
> $(foreach c,$(PREMAKE_CONFIGURATIONS),$(PMAKE) clean config=$c;)

%: configure
> $(if $(filter-out $*,$(SPECIAL_TARGETS)),@$(PMAKE) $*)
