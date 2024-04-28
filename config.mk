###############################################################################
#                                                                             #
# config.mk
#
# Description:
#
# Author:	Erfan Shoara (erfan@shoara.net)
# Date:		Apr 28, 2024
#
#
# Config - Variables:
# 	> OUT_MOD := DYN
# 		if set to STA, it will install the static library, otherwise (anything
# 		else) will install the dynamic library.
#
# 	> DIR_OUT := out
# 		The name of the directory that all object files (*.o) and libraries
# 		(.a, .so) will be placed from compilation.
#
# 	> DIR_DST_INC := /usr/include
# 		the directory to where the header (.h) file is place from installation.
#
# 	> DIR_DST_LIB_STA := /usr/lib
# 		the directory to where the static library (.a) file is place from
# 		installation - only in (OUT_MOD := STA).
#
# 	> DIR_DST_LIB_DYN := /usr/lib
# 		the directory to where the shared library (.so) file is place from
# 		installation - only in (OUT_MOD := DYN).
#
#                                                                             #
###############################################################################

# compilation output dir
DIR_OUT := out

# compile/install mode - dynamic or static
OUT_MOD := DYN

# dirs for installation
DIR_DST_INC := /usr/include
DIR_DST_LIB_STA := /usr/lib
DIR_DST_LIB_DYN := /usr/lib
