# Project Name
TARGET = adsrWithPots

# Sources
CPP_SOURCES = adsrWithPots.cpp

# Library Locations
LIBDAISY_DIR ?= ../../../libDaisy
DAISYSP_DIR ?= ../../../DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
