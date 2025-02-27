# USB Display Linux Driver Development Guide

## Build Commands
- FrameBuffer: `cd usb_display/FrameBuffer_SourceCode_V3.0.2.9/ && make clean && make`
- DRM Driver: `cd usb_display/MS91xx_Linux_Drm_SourceCode_V3.0.2.7/ && make clean && make`
- Load Modules: `sudo insmod ./drm/usbdisp_drm.ko && sudo insmod ./drm/usbdisp_usb.ko`
- Check Modules: `lsmod | grep usbdisp`
- Restart Display: `systemctl restart display-manager`

## Code Style Guidelines
- Follow Linux kernel style for C code
- Use 4-space tab indentation
- Function names in snake_case
- Macros in ALL_CAPS
- Braces on new lines for function definitions
- Group includes: system headers, Linux kernel, DRM, local headers
- Include copyright notices at file top
- Use kernel version macros for compatibility
- Document with comments near the relevant code
- Clean warnings when building (from script.sh)

## Requirements
- Linux kernel 4.19+
- X11 desktop environment