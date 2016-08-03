LIBGNSS   = middleware/MTK/gnss

ifeq ($(PRODUCT_VERSION),2523)
BOARD_SRC = driver/board/mt25x3_hdk
endif

C_FILES  += $(LIBGNSS)/src/gnss_api.c    \
            $(LIBGNSS)/src/gnss_driver.c
C_FILES  += $(BOARD_SRC)/gnss/gnss_config.c

#################################################################################
# include path

CFLAGS 	+= -I./include
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/gnss/inc
