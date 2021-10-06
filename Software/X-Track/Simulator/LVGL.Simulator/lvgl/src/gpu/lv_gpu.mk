CSRCS += lv_gpu_nxp_pxp.c
CSRCS += lv_gpu_nxp_pxp_osa.c
CSRCS += lv_gpu_nxp_vglite.c
CSRCS += lv_gpu_stm32_dma2d.c
CSRCS += lv_gpu_sdl.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu"

include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/gpu/sdl/lv_gpu_sdl.mk