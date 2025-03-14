#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_MITIGATION_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

KSYMTAB_FUNC(msdisp_platform_get_device, "", "");
KSYMTAB_FUNC(msdisp_platform_get_plat_device_index, "", "");
KSYMTAB_FUNC(msdisp_drm_get_free_device, "", "");
KSYMTAB_FUNC(msdisp_drm_get_drm_device_index, "", "");
KSYMTAB_FUNC(msdisp_drm_get_free_pipeline_index, "", "");
KSYMTAB_FUNC(msdisp_drm_get_pipeline_global_id, "", "");
KSYMTAB_FUNC(msdisp_drm_get_pipeline_kobject, "", "");
KSYMTAB_FUNC(msdisp_drm_register_usb_hal, "", "");
KSYMTAB_FUNC(msdisp_drm_unregister_usb_hal, "", "");
KSYMTAB_FUNC(msdisp_drm_get_kfifo, "", "");

SYMBOL_CRC(msdisp_platform_get_device, 0x04a22db9, "");
SYMBOL_CRC(msdisp_platform_get_plat_device_index, 0xf2fb00a8, "");
SYMBOL_CRC(msdisp_drm_get_free_device, 0x92155ece, "");
SYMBOL_CRC(msdisp_drm_get_drm_device_index, 0x319ac990, "");
SYMBOL_CRC(msdisp_drm_get_free_pipeline_index, 0xf16c3a67, "");
SYMBOL_CRC(msdisp_drm_get_pipeline_global_id, 0x5b2acca5, "");
SYMBOL_CRC(msdisp_drm_get_pipeline_kobject, 0x3bb43fc4, "");
SYMBOL_CRC(msdisp_drm_register_usb_hal, 0x9e4c5666, "");
SYMBOL_CRC(msdisp_drm_unregister_usb_hal, 0x7814ef90, "");
SYMBOL_CRC(msdisp_drm_get_kfifo, 0x4439df48, "");

static const char ____versions[]
__used __section("__versions") =
	"\x14\x00\x00\x00\x32\xb4\x35\x8a"
	"sme_me_mask\0"
	"\x20\x00\x00\x00\x73\x63\x30\x3f"
	"drm_framebuffer_cleanup\0"
	"\x30\x00\x00\x00\x8f\x0f\x04\x9b"
	"drm_atomic_helper_plane_destroy_state\0\0\0"
	"\x14\x00\x00\x00\xb5\xec\x90\xfb"
	"filp_open\0\0\0"
	"\x14\x00\x00\x00\xee\xd3\xb7\x61"
	"drm_open\0\0\0\0"
	"\x1c\x00\x00\x00\x0a\x34\xb5\xf1"
	"drm_mode_vrefresh\0\0\0"
	"\x14\x00\x00\x00\x4a\x72\xe8\x13"
	"drm_poll\0\0\0\0"
	"\x14\x00\x00\x00\xa7\xc7\x1a\xb1"
	"__drm_err\0\0\0"
	"\x1c\x00\x00\x00\x23\xfd\xcf\xa6"
	"drm_gem_get_pages\0\0\0"
	"\x18\x00\x00\x00\xff\x19\x8d\x51"
	"dma_buf_vmap\0\0\0\0"
	"\x24\x00\x00\x00\x86\x99\xe1\xa9"
	"drm_kms_helper_poll_init\0\0\0\0"
	"\x20\x00\x00\x00\x48\xae\xff\xe0"
	"drm_framebuffer_init\0\0\0\0"
	"\x2c\x00\x00\x00\x3c\xbe\xca\xe5"
	"drm_plane_enable_fb_damage_clips\0\0\0\0"
	"\x18\x00\x00\x00\x78\x18\x6d\x01"
	"dev_set_name\0\0\0\0"
	"\x24\x00\x00\x00\x19\xc8\x15\x20"
	"platform_driver_unregister\0\0"
	"\x1c\x00\x00\x00\xc4\x25\x2b\x34"
	"param_ops_ushort\0\0\0\0"
	"\x28\x00\x00\x00\xee\x56\x75\xc5"
	"drm_atomic_add_affected_planes\0\0"
	"\x20\x00\x00\x00\x4f\x61\xce\x57"
	"dma_buf_end_cpu_access\0\0"
	"\x1c\x00\x00\x00\x7c\x80\x1d\x84"
	"drm_mode_probed_add\0"
	"\x18\x00\x00\x00\xe4\x60\x84\x9e"
	"dma_resv_init\0\0\0"
	"\x28\x00\x00\x00\xc7\x9b\x8a\x8f"
	"drm_atomic_helper_disable_plane\0"
	"\x18\x00\x00\x00\x6c\x64\xe9\x87"
	"__kernel_write\0\0"
	"\x24\x00\x00\x00\xc0\x8e\xa5\xd1"
	"drm_prime_sg_to_page_array\0\0"
	"\x1c\x00\x00\x00\x4e\x78\x33\xf7"
	"device_initialize\0\0\0"
	"\x1c\x00\x00\x00\x6b\x21\x87\xda"
	"drm_mode_object_get\0"
	"\x10\x00\x00\x00\x38\xdf\xac\x69"
	"memcpy\0\0"
	"\x10\x00\x00\x00\x83\x12\x96\x94"
	"vunmap\0\0"
	"\x10\x00\x00\x00\xba\x0c\x7a\x03"
	"kfree\0\0\0"
	"\x30\x00\x00\x00\x34\xbf\x1c\x15"
	"drm_atomic_helper_crtc_destroy_state\0\0\0\0"
	"\x1c\x00\x00\x00\xae\x35\x72\x38"
	"drm_crtc_cleanup\0\0\0\0"
	"\x24\x00\x00\x00\xb0\x5f\x96\x73"
	"drm_gem_create_mmap_offset\0\0"
	"\x18\x00\x00\x00\x55\x48\x0e\xdc"
	"timer_delete\0\0\0\0"
	"\x1c\x00\x00\x00\x57\xfe\x1e\x78"
	"drm_encoder_cleanup\0"
	"\x1c\x00\x00\x00\x64\xee\x10\x70"
	"drm_gem_object_init\0"
	"\x24\x00\x00\x00\xa9\xaa\xda\x0e"
	"drm_gem_prime_handle_to_fd\0\0"
	"\x18\x00\x00\x00\x52\x0f\x76\xdb"
	"__kfifo_free\0\0\0\0"
	"\x20\x00\x00\x00\x4b\x36\x7e\xc3"
	"drmm_mode_config_init\0\0\0"
	"\x20\x00\x00\x00\x0b\x05\xdb\x34"
	"_raw_spin_lock_irqsave\0\0"
	"\x18\x00\x00\x00\x64\xbd\x8f\xba"
	"_raw_spin_lock\0\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x1c\x00\x00\x00\x78\x80\xb8\xef"
	"drm_mode_object_put\0"
	"\x20\x00\x00\x00\xce\x9b\x20\xbd"
	"drm_gem_object_lookup\0\0\0"
	"\x24\x00\x00\x00\xe9\xe8\x3d\x4a"
	"drm_crtc_send_vblank_event\0\0"
	"\x28\x00\x00\x00\x17\x8d\xaa\xb8"
	"drm_connector_attach_encoder\0\0\0\0"
	"\x24\x00\x00\x00\x97\x70\x48\x65"
	"__x86_indirect_thunk_rax\0\0\0\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x20\x00\x00\x00\x5f\x69\x96\x02"
	"refcount_warn_saturate\0\0"
	"\x24\x00\x00\x00\x72\xb2\x1e\xbc"
	"drm_atomic_helper_commit\0\0\0\0"
	"\x14\x00\x00\x00\x8a\x3f\x57\x06"
	"put_device\0\0"
	"\x20\x00\x00\x00\x94\xb7\xee\x82"
	"drm_atomic_helper_check\0"
	"\x34\x00\x00\x00\x31\x84\xfa\x80"
	"drm_atomic_helper_connector_destroy_state\0\0\0"
	"\x20\x00\x00\x00\xda\xd0\x3a\xc0"
	"root_device_unregister\0\0"
	"\x1c\x00\x00\x00\x16\xe2\x17\xc9"
	"drm_clflush_pages\0\0\0"
	"\x10\x00\x00\x00\x94\xb6\x16\xa9"
	"strnlen\0"
	"\x18\x00\x00\x00\xa3\x48\x7c\xc5"
	"idr_get_next\0\0\0\0"
	"\x18\x00\x00\x00\xe3\xb8\xcb\x0d"
	"const_pcpu_hot\0\0"
	"\x18\x00\x00\x00\xb6\x27\xd6\xbd"
	"drm_gem_mmap\0\0\0\0"
	"\x30\x00\x00\x00\xba\x79\x0a\x76"
	"drm_atomic_helper_plane_duplicate_state\0"
	"\x14\x00\x00\x00\x6e\x8a\x26\x7d"
	"_dev_info\0\0\0"
	"\x20\x00\x00\x00\xab\x03\x6b\x70"
	"drm_gem_prime_import\0\0\0\0"
	"\x14\x00\x00\x00\xe3\xc1\xae\xc6"
	"drm_ioctl\0\0\0"
	"\x1c\x00\x00\x00\xf6\x3b\xa6\x86"
	"drm_plane_cleanup\0\0\0"
	"\x24\x00\x00\x00\x3f\xb2\x16\x00"
	"dma_buf_begin_cpu_access\0\0\0\0"
	"\x28\x00\x00\x00\xb3\x1c\xa2\x87"
	"__ubsan_handle_out_of_bounds\0\0\0\0"
	"\x24\x00\x00\x00\x3b\x26\x7e\x95"
	"drm_gem_prime_fd_to_handle\0\0"
	"\x1c\x00\x00\x00\x77\xa3\x80\xd6"
	"drm_gem_object_free\0"
	"\x20\x00\x00\x00\x34\xc3\x1d\x81"
	"usb_unregister_notify\0\0\0"
	"\x24\x00\x00\x00\x86\xc0\xdb\xdf"
	"drm_atomic_helper_dirtyfb\0\0\0"
	"\x1c\x00\x00\x00\xf7\x8d\x9d\x3b"
	"drm_gem_vm_close\0\0\0\0"
	"\x18\x00\x00\x00\x75\x1a\xf5\x26"
	"vm_insert_page\0\0"
	"\x18\x00\x00\x00\x3d\x5e\x8d\xdc"
	"drm_gem_vm_open\0"
	"\x18\x00\x00\x00\x80\x04\x26\x0e"
	"drm_dev_unplug\0\0"
	"\x1c\x00\x00\x00\xb7\x47\xd2\x52"
	"drm_connector_init\0\0"
	"\x14\x00\x00\x00\xd2\x19\xbc\x57"
	"down_write\0\0"
	"\x14\x00\x00\x00\x3e\x1e\x7c\x48"
	"_dev_err\0\0\0\0"
	"\x14\x00\x00\x00\x25\x7a\x80\xce"
	"up_write\0\0\0\0"
	"\x14\x00\x00\x00\xb3\x31\x2a\xf4"
	"device_add\0\0"
	"\x20\x00\x00\x00\x37\xb0\xa5\xd0"
	"__root_device_register\0\0"
	"\x14\x00\x00\x00\xd1\x73\xd2\x24"
	"add_timer\0\0\0"
	"\x24\x00\x00\x00\x2e\x5e\x38\x55"
	"__x86_indirect_thunk_r14\0\0\0\0"
	"\x24\x00\x00\x00\xe2\x9f\xbc\x49"
	"platform_device_unregister\0\0"
	"\x1c\x00\x00\x00\xaf\xe8\x5c\x6c"
	"drm_add_edid_modes\0\0"
	"\x14\x00\x00\x00\xb8\x83\x8c\xc3"
	"mod_timer\0\0\0"
	"\x14\x00\x00\x00\xc9\x64\x69\xe0"
	"noop_llseek\0"
	"\x14\x00\x00\x00\x62\x3b\xb0\xf9"
	"drm_read\0\0\0\0"
	"\x18\x00\x00\x00\x33\x62\x05\xd7"
	"dma_resv_fini\0\0\0"
	"\x1c\x00\x00\x00\x63\xa5\x03\x4c"
	"random_kmalloc_seed\0"
	"\x20\x00\x00\x00\xc1\x7e\x13\x77"
	"drm_gem_object_release\0\0"
	"\x14\x00\x00\x00\x4b\x8d\xfa\x4d"
	"mutex_lock\0\0"
	"\x1c\x00\x00\x00\x63\xef\xaa\xbb"
	"drm_crtc_vblank_on\0\0"
	"\x10\x00\x00\x00\x45\x6d\x94\x3c"
	"vmap\0\0\0\0"
	"\x20\x00\x00\x00\x47\x95\xdc\x3d"
	"drm_gem_handle_create\0\0\0"
	"\x28\x00\x00\x00\x73\x2b\xa6\x73"
	"drm_atomic_helper_plane_reset\0\0\0"
	"\x24\x00\x00\x00\x18\xd7\x1f\x1c"
	"drm_crtc_init_with_planes\0\0\0"
	"\x1c\x00\x00\x00\xab\x72\xaa\xd3"
	"drm_mode_duplicate\0\0"
	"\x28\x00\x00\x00\x14\x3b\xb0\xed"
	"drm_helper_mode_fill_fb_struct\0\0"
	"\x18\x00\x00\x00\x9f\x0c\xfb\xce"
	"__mutex_init\0\0\0\0"
	"\x18\x00\x00\x00\xb5\x79\xca\x75"
	"__fortify_panic\0"
	"\x18\x00\x00\x00\xe1\x27\x36\x99"
	"__drm_dev_dbg\0\0\0"
	"\x20\x00\x00\x00\xde\xf8\x78\xfc"
	"drm_prime_pages_to_sg\0\0\0"
	"\x24\x00\x00\x00\x70\xce\x5c\xd3"
	"_raw_spin_unlock_irqrestore\0"
	"\x14\x00\x00\x00\xbb\x37\x98\x2a"
	"device_del\0\0"
	"\x24\x00\x00\x00\x61\xd2\x8c\xd3"
	"__default_kernel_pte_mask\0\0\0"
	"\x20\x00\x00\x00\x53\x48\xab\x5b"
	"__devm_drm_dev_alloc\0\0\0\0"
	"\x14\x00\x00\x00\x5a\x1d\xe1\x92"
	"_dev_warn\0\0\0"
	"\x2c\x00\x00\x00\x1a\xc2\x0c\x0a"
	"drm_connector_update_edid_property\0\0"
	"\x18\x00\x00\x00\x24\xfd\xd7\xd3"
	"__get_task_comm\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x28\x00\x00\x00\xc3\x7c\xa7\x7b"
	"drm_atomic_helper_set_config\0\0\0\0"
	"\x18\x00\x00\x00\x0f\xb8\xd0\x57"
	"dma_buf_vunmap\0\0"
	"\x34\x00\x00\x00\x5c\xa4\xeb\x22"
	"drm_atomic_helper_connector_duplicate_state\0"
	"\x10\x00\x00\x00\x5a\x25\xd5\xe2"
	"strcmp\0\0"
	"\x20\x00\x00\x00\x69\xe2\x4f\x2c"
	"drm_connector_cleanup\0\0\0"
	"\x10\x00\x00\x00\xa6\x50\xba\x15"
	"jiffies\0"
	"\x24\x00\x00\x00\xdb\xa0\x79\x0f"
	"__platform_driver_register\0\0"
	"\x24\x00\x00\x00\x7e\x70\xea\xf3"
	"drm_universal_plane_init\0\0\0\0"
	"\x28\x00\x00\x00\x4d\x4b\xae\x78"
	"drm_atomic_helper_crtc_reset\0\0\0\0"
	"\x10\x00\x00\x00\xfd\xf9\x3f\x3c"
	"sprintf\0"
	"\x2c\x00\x00\x00\x61\xe5\x48\xa6"
	"__ubsan_handle_shift_out_of_bounds\0\0"
	"\x1c\x00\x00\x00\xe9\x8d\x7d\x31"
	"drm_encoder_init\0\0\0\0"
	"\x28\x00\x00\x00\x0d\xa0\x3d\xb4"
	"platform_device_register_full\0\0\0"
	"\x18\x00\x00\x00\x2a\xcb\x6c\xdd"
	"drm_vblank_init\0"
	"\x18\x00\x00\x00\x38\xf0\x13\x32"
	"mutex_unlock\0\0\0\0"
	"\x18\x00\x00\x00\x39\x63\xf4\xc6"
	"init_timer_key\0\0"
	"\x24\x00\x00\x00\x21\x14\x0b\x44"
	"drm_gem_free_mmap_offset\0\0\0\0"
	"\x14\x00\x00\x00\x0f\x8f\xad\xa7"
	"filp_close\0\0"
	"\x20\x00\x00\x00\xee\xfb\xb4\x10"
	"__kmalloc_cache_noprof\0\0"
	"\x1c\x00\x00\x00\x27\x5e\xbc\xd0"
	"drm_crtc_vblank_off\0"
	"\x30\x00\x00\x00\x14\x25\xfa\x87"
	"drm_helper_probe_single_connector_modes\0"
	"\x20\x00\x00\x00\x71\x97\xb3\xd5"
	"drm_crtc_handle_vblank\0\0"
	"\x18\x00\x00\x00\xf5\x68\x1f\x9a"
	"dma_set_mask\0\0\0\0"
	"\x2c\x00\x00\x00\x52\x38\x20\x1e"
	"drm_atomic_helper_connector_reset\0\0\0"
	"\x20\x00\x00\x00\xa5\xe9\x82\xe9"
	"drm_mode_config_reset\0\0\0"
	"\x18\x00\x00\x00\xda\x38\xeb\x55"
	"drm_format_info\0"
	"\x1c\x00\x00\x00\xc6\xaf\xbb\x89"
	"usb_register_notify\0"
	"\x2c\x00\x00\x00\xc6\xfa\xb1\x54"
	"__ubsan_handle_load_invalid_value\0\0\0"
	"\x18\x00\x00\x00\x89\x21\x9f\x13"
	"__kfifo_alloc\0\0\0"
	"\x10\x00\x00\x00\x9c\x53\x4d\x75"
	"strlen\0\0"
	"\x10\x00\x00\x00\x6e\x75\xa1\x7a"
	"kvfree\0\0"
	"\x30\x00\x00\x00\x18\xac\xe2\xeb"
	"drm_atomic_helper_crtc_duplicate_state\0\0"
	"\x1c\x00\x00\x00\x34\x4b\xb5\xb5"
	"_raw_spin_unlock\0\0\0\0"
	"\x28\x00\x00\x00\x80\x49\xdc\x68"
	"drm_atomic_helper_update_plane\0\0"
	"\x18\x00\x00\x00\x76\xe5\x57\x50"
	"drm_get_edid\0\0\0\0"
	"\x20\x00\x00\x00\x4c\xa1\xf2\x31"
	"__kvmalloc_node_noprof\0\0"
	"\x1c\x00\x00\x00\xab\xe8\x6f\xd5"
	"drm_gem_put_pages\0\0\0"
	"\x1c\x00\x00\x00\xb9\xb8\x97\xf4"
	"drm_dev_register\0\0\0\0"
	"\x28\x00\x00\x00\xf1\x3b\x3d\x41"
	"drm_gem_plane_helper_prepare_fb\0"
	"\x18\x00\x00\x00\x1d\x07\x60\x20"
	"kmalloc_caches\0\0"
	"\x20\x00\x00\x00\x48\x23\x42\x36"
	"drm_prime_gem_destroy\0\0\0"
	"\x14\x00\x00\x00\xb8\xd4\xfd\xbb"
	"drm_release\0"
	"\x20\x00\x00\x00\x91\xf5\xa0\x60"
	"drm_gem_prime_export\0\0\0\0"
	"\x18\x00\x00\x00\xde\x9f\x8a\x25"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "CCABA296FCA74F2676CC176");
