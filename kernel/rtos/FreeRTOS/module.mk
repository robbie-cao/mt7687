
###################################################
# Sources
RTOS_SRC = kernel/rtos/FreeRTOS/Source

RTOS_FILES =	$(RTOS_SRC)/tasks.c \
		$(RTOS_SRC)/list.c \
		$(RTOS_SRC)/queue.c \
		$(RTOS_SRC)/timers.c \
		$(RTOS_SRC)/event_groups.c \
		$(RTOS_SRC)/portable/GCC/ARM_CM4F/port.c \
		$(RTOS_SRC)/portable/MemMang/heap_4.c

ifeq ($(PRODUCT_VERSION),7687)

RTOS_FILES +=   $(RTOS_SRC)/portable/GCC/mt7687/port_tickless.c
endif

ifeq ($(PRODUCT_VERSION),2523)
RTOS_FILES += 	$(RTOS_SRC)/portable/GCC/mt2523/port_tickless.c 
CFLAGS     += -I$(SOURCE_DIR)/$(RTOS_SRC)/portable/GCC/mt2523
endif

C_FILES += $(RTOS_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(RTOS_SRC)/include
CFLAGS += -I$(SOURCE_DIR)/$(RTOS_SRC)/portable/GCC/ARM_CM4F
CFLAGS += -I$(SOURCE_DIR)/kernel/service/inc
