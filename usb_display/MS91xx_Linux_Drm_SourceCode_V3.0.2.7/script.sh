# First, backup the file
cp drm/Kbuild drm/Kbuild.backup

# Then create a new file with clean entries
cat > drm/Kbuild << EOF
usb_hal_o += \$(addprefix ../usb_hal/, \$(USB_HAL))
usbdisp_drm-y := msdisp_plat_drv.o msdisp_plat_dev.o msdisp_drm_drv.o  msdisp_drm_modeset.o  msdisp_drm_gem.o  \\
	msdisp_drm_fb.o  msdisp_drm_encoder.o  msdisp_drm_connector.o msdisp_drm_interface.o msdisp_drm_sysfs.o msdisp_common_util.o msdisp_drm_mode.o
usbdisp_usb-y := msdisp_usb_drv.o ms9132_hal.o \$(usb_hal_o)
obj-m := usbdisp_drm.o usbdisp_usb.o 

ccflags-y := -isystem include/drm -I\$(HAL_PATH) \$(CFLAGS) \$(EL8FLAG) \$(RPIFLAG)
CFLAGS_usbdisp_usb.o += -I\$(HAL_PATH)

# Disable error warnings
ccflags-y += -Wno-error
ccflags-y += -Wno-error=incompatible-pointer-types
ccflags-y += -Wno-error=missing-prototypes
ccflags-y += -Wno-incompatible-pointer-types
ccflags-y += -Wno-missing-prototypes
EOF
