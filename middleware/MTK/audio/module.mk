
###################################################

AUDIO_SRC = middleware/MTK/audio
AUDIO_FILES = $(AUDIO_SRC)/src/audio_middleware_api.c \
			  $(AUDIO_SRC)/src/audio_nvdm.c

C_FILES += $(AUDIO_FILES) 

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/inc

