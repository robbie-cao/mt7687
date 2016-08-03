
LZMA_SRC = middleware/third_party/lzma_decoder

C_FILES  += $(LZMA_SRC)/src/lzma_decode_interface.c \
			$(LZMA_SRC)/src/lzma_decoder.c

			 

#################################################################################
#include path
CFLAGS 	+= -I./inc
CFLAGS  += -I../../../driver/chip/mt7687/inc
CFLAGS  += -I../../../driver/board/mt25x3_hdk/bootloader/core/inc
