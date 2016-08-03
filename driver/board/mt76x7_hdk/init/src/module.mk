
INIT_SRC = driver/board/mt76x7_hdk/init

ifeq ($(MTK_MINICLI_ENABLE),y)
C_FILES  += $(INIT_SRC)/src/board_cli.c
endif

#################################################################################
#include path
CFLAGS 	+= -Iinc

