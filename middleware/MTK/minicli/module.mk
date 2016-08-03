MINICLI_SRC = middleware/MTK/minicli

-include src_protected/module.mk
C_FILES += $(MINICLI_SRC)/src/minicli_api.c

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/$(MINICLI_SRC)/inc
