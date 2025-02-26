#ifndef __MSFB_H__
#define __MSFB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif   

#include <linux/fb.h>

#define MSFB_HDMI_HPD_DISCONNECTED       0
#define MSFB_HDMI_HPD_CONNECTED          1

#define FBIOGET_HDMI_HPD_MSFB       _IOR('F', 0x90, unsigned int)



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif