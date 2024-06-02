###############################################################################
#                                                                             #
# Makefile
#
# Description:
# 	the makefile for lmeer project. It compiles the code and creates both
# 	static and dynamic libraries under $(DIR_OUT_LIB). It also install the
# 	libraries - coping them to the specified location, there are two modes of
# 	installation (dynamic library, static library) which can be selected using
# 	$(OUT_MOD) in config file (config.mk). uninstall will remove the
# 	system-wide files copied using the install target, and clear will only
# 	remove the local object files created from 'all' target.
#
# Author:	Erfan Shoara (erfan@shoara.net)
# Date:		Feb 17, 2024
#
# Targets:
# 	> all:			the default target, creates all obj/lib files locally
# 	> install:		copy the selected lib to $(DIR_DST*), and compile if needed
# 	> clean:		removes local obj/lib files
# 	> uninstall:	removes the lib and headers from $(DIR_DST*)
#
# Variables:
# 	> OUT_NAM := lmeer
# 		the name of the library used for creating the header file and
# 		libraries.
#
# 	> DIR_DST_INC := /usr/include
# 		the directory where the header file should be copied to.
#
# 	> DIR_DST_LIB_STA := /usr/lib
# 		the directory where the static library should be placed.
#
# 	> DIR_DST_LIB_DYN := /usr/lib
# 		the directory where the dynamic library should be placed.
#
# 	> ARC := ar
# 		the program used for archiving static objects into the static library.
#
# 	> ARC_FLG := rcs
# 		the options passed to the archive program set by $(ARV)
#
# 	> CC := gcc
# 		the c compiler program. NOTE, all variables starting with $(FLG_*) are
# 		the options passed to $(CC) respective to the mode of
# 		compilation/linking.
#
# Config File:	config.mk
#                                                                             #
###############################################################################


# include
# {
include config.mk
# }


# var
# {

# dir/file
# {
# .c files
DIR_SRC := src
# .h files
DIR_INC := inc

# .o files
DIR_OUT_OBJ := $(DIR_OUT)/obj
# .a and .so files
DIR_OUT_LIB := $(DIR_OUT)/lib
DIR_OUT_OBJ_STA := $(DIR_OUT_OBJ)/sta
DIR_OUT_OBJ_DYN := $(DIR_OUT_OBJ)/dyn


OUT_NAM := lmeer
INC_OUT:= $(OUT_NAM).h
LIB_OUT_STA := lib$(OUT_NAM).a
LIB_OUT_DYN := lib$(OUT_NAM).so

LST_SRC := $(wildcard $(DIR_SRC)/*.c)
LST_OBJ_STA := $(patsubst $(DIR_SRC)/%.c,$(DIR_OUT_OBJ_STA)/%.o,$(LST_SRC))
LST_OBJ_DYN := $(patsubst $(DIR_SRC)/%.c,$(DIR_OUT_OBJ_DYN)/%.o,$(LST_SRC))

# }

# archiver, compiler and flags
# {
ARC := ar
ARC_FLG := rcs

CC := gcc
# the include flag after declaring the include dir
FLG_INC := -I$(DIR_INC)
# flags for compilation
FLG_CPL := $(FLG_INC) -c -Wall -Wextra -Werror
FLG_CPL_STA := $(FLG_CPL)
FLG_CPL_DYN := $(FLG_CPL) -fpic
# flags for linking
FLG_LNK := -shared
# }
# }


# target
# {
# all
# {
.PHONY: all
all: $(DIR_OUT_LIB)/$(LIB_OUT_STA) $(DIR_OUT_LIB)/$(LIB_OUT_DYN)

# archiving static object files to static lib
$(DIR_OUT_LIB)/$(LIB_OUT_STA): $(LST_OBJ_STA) | $(DIR_OUT_LIB)
	$(ARC) $(ARC_FLG) $@ $^

# linking dynamic object files into a shared lib
$(DIR_OUT_LIB)/$(LIB_OUT_DYN): $(LST_OBJ_DYN) | $(DIR_OUT_LIB)
	$(CC) $(FLG_LNK) -o $@ $^

# compiling static objects
$(LST_OBJ_STA): $(DIR_OUT_OBJ_STA)/%.o: $(DIR_SRC)/%.c | $(DIR_OUT_OBJ_STA)
	$(CC) $(FLG_CPL_STA) -o $@ $<

# compiling dynamic objects
$(LST_OBJ_DYN): $(DIR_OUT_OBJ_DYN)/%.o: $(DIR_SRC)/%.c | $(DIR_OUT_OBJ_DYN)
	$(CC) $(FLG_CPL_DYN) -o $@ $<

$(DIR_OUT_LIB) $(DIR_OUT_OBJ_STA) $(DIR_OUT_OBJ_DYN):
	mkdir -p $@
# }

# clean
# {
.PHONY: clean
clean:
	rm -rf $(DIR_OUT)
# }


# install
# {
.PHONY: install
ifeq ($(OUT_MOD),STA)
# installing as static lib
install: $(DIR_OUT_LIB)/$(LIB_OUT_STA)
	@echo "[MAKE] installing as static library..."
	cp $< $(DIR_DST_LIB_STA)/$(LIB_OUT_STA)
	cp $(DIR_INC)/$(INC_OUT) $(DIR_DST_INC)/$(INC_OUT)
	# TODO:need to set the permissions - i don't like this fix
	chmod 755 $(DIR_DST_LIB_STA)/$(LIB_OUT_STA)
	chmod 644 $(DIR_DST_INC)/$(INC_OUT)
else
# installing as shared lib
install: $(DIR_OUT_LIB)/$(LIB_OUT_DYN)
	@echo "[MAKE] installing as shared library..."
	cp $< $(DIR_DST_LIB_DYN)/$(LIB_OUT_DYN)
	cp $(DIR_INC)/$(INC_OUT) $(DIR_DST_INC)/$(INC_OUT)
	# TODO:need to set the permissions - i don't like this fix
	chmod 644 $(DIR_DST_LIB_DYN)/$(LIB_OUT_DYN)
	chmod 644 $(DIR_DST_INC)/$(INC_OUT)
endif

# }

# uninstall
# {
# uninstalling both static and dynamic libs
.PHONY: uninstall
uninstall:
	rm -f $(DIR_DST_LIB_STA)/$(LIB_OUT_STA)
	rm -f $(DIR_DST_LIB_DYN)/$(LIB_OUT_DYN)
	rm -f $(DIR_DST_INC)/$(INC_OUT)
# }
# }

