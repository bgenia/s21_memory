# Make configuration

.RECIPEPREFIX = >

MAKEFLAGS += --no-print-directory --jobs --output-sync

export MANAGED_BUILD = true

.DEFAULT_GOAL = all

# Solution: Simple Navigator

PROJECT_ROOT = $(CURDIR)

SOURCE_DIRECTORY = src

BUILD_DIRECTORY = build
BINARY_DIRECTORY = bin

export SOURCE_ROOT = $(PROJECT_ROOT)/$(SOURCE_DIRECTORY)
export BUILD_ROOT = $(PROJECT_ROOT)/$(BUILD_DIRECTORY)/$(BUILD_TYPE)
export BINARY_ROOT = $(PROJECT_ROOT)/$(BINARY_DIRECTORY)/$(BUILD_TYPE)

export BUILD_TYPE ?= release

# Compiler

export CXX = g++
export CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -pedantic
export CPPFLAGS = -MMD -MP
export LDFLAGS = $(addprefix -L$(BINARY_ROOT)/,$(LIBRARIES))

# Projects

s21_memory_cli: s21_memory

TARGETS = $(LIBRARIES) $(EXECUTABLES)

LIBRARIES = s21_memory

EXECUTABLES = s21_memory_cli

TEST_EXECUTABLES = s21_memory_test

PROJECTS = $(LIBRARIES) $(EXECUTABLES)

# Build modifiers

RAW_MODIFIERS = $(filter @%,$(MAKECMDGOALS))
MODIFIERS = $(RAW_MODIFIERS::%=%)

ifneq ($(findstring debug,$(MODIFIERS)),)
	BUILD_TYPE = debug
	CXXFLAGS += -g
	CPPFLAGS += -DDEBUG
else ifneq ($(findstring sanitize,$(MODIFIERS)),)
	BUILD_TYPE = sanitize
	CXXFLAGS += -g -fsanitize=address -fsanitize=undefined
	CPPFLAGS += -DDEBUG
else ifneq ($(findstring coverage,$(MODIFIERS)),)
	BUILD_TYPE = coverage
	CXXFLAGS += -g -O0 -coverage
	CPPFLAGS += -DNDEBUG
else ifneq ($(or $(findstring release,$(MODIFIERS)),$(if $(MAKECMDGOALS),,release)),)
	BUILD_TYPE = release
	CXXFLAGS += -O2
	CPPFLAGS += -DNDEBUG
endif

.PHONY: $(RAW_MODIFIERS)

ifeq ($(filter-out $(RAW_MODIFIERS),$(MAKECMDGOALS)),)
	$(firstword $(RAW_MODIFIERS)): $(.DEFAULT_GOAL)
endif

# Build targets

.PHONY: all
all: $(PROJECTS)

# Subject compatibility fix, not recommended for direct use
.PHONY: force-project
%.a: force-project
>	$(MAKE) $*

.PHONY: $(PROJECTS)
$(PROJECTS):
>	$(MAKE) -C $(SOURCE_DIRECTORY)/$@

$(addsuffix @%,$(PROJECTS)): force-project
>	$(MAKE) -C $(SOURCE_DIRECTORY)/$(@:%@$*=%) $*

# Utility targets

.PHONY: mostlyclean
mostlyclean:
>	$(RM) -r $(BUILD_DIRECTORY)

.PHONY: clean
clean: mostlyclean
>	$(RM) -r $(BINARY_DIRECTORY)

# Test executables

.PHONY: $(TEST_EXECUTABLES)
$(TEST_EXECUTABLES): export BUILD_TYPE = coverage
$(TEST_EXECUTABLES): export LDFLAGS = $(addprefix -L$(PROJECT_ROOT)/$(BINARY_DIRECTORY)/coverage/,$(LIBRARIES))
$(TEST_EXECUTABLES):
>	$(MAKE) $(LIBRARIES) @coverage
>	$(MAKE) -C $(SOURCE_DIRECTORY)/$@
>	$(BINARY_ROOT)/$@/$@

.PHONY: .test
.test: $(TEST_EXECUTABLES)

.PHONY: .coverage
.coverage: .test
.coverage: BUILD_TYPE = coverage
.coverage:
>	lcov -t simple_navigator -c -d $(BUILD_ROOT) -o $(BINARY_ROOT)/coverage.info
>	genhtml $(BINARY_ROOT)/coverage.info -o $(BINARY_ROOT)/report


# Help target

define HELP_MESSAGE
Usage: make [target][@task] [@modifiers...]

Projects:
	all - build all projects
	$(TARGETS) - build specified project

	$(TEST_EXECUTABLES) - build secified test executable

Special targets:
	.test - build and run all test executables
	.covereage - run all tests and create a coverage report

Tasks:
	mostlyclean - remove build files
	clean - remove build and binary files

	! Task syntax allows to specify task for specific project, e.g.: make graph@mostlyclean
	  will call mostlyclean task for graph project. See project's Makefile for more information.

Modifiers:
	debug - build debug version (includes debug symbols)
	sanitize - build sanitized version (includes address and undefined sanitizers)
	release - build release version (default, no debug symbols, optimizations)
	coverage - build with coverage data

	! Modifier syntax allows to specify modifier for specific project, e.g.: make graph@mostlyclean @debug
	  will call mostlyclean task for graph project with debug modifier. See project's Makefile for more information

endef

.PHONY: help
help:
>	$(info $(HELP_MESSAGE))
