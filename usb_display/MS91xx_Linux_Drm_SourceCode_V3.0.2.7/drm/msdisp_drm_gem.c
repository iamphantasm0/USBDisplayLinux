/* Copyright (C) 2023 MacroSilicon Technology Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * msdisp_drm_gem.c -- Drm driver for MacroSilicon chip 913x and 912x
 */


#include <linux/sched.h>
#include <linux/vmalloc.h>  /* For vmap and vunmap functions */
#include <linux/version.h>
#include <linux/vmalloc.h>  /* For vmap and vunmap functions */
#if KERNEL_VERSION(5, 18, 0) <= LINUX_VERSION_CODE
#include <linux/mm.h>
#endif
#if KERNEL_VERSION(5, 18, 0) <= LINUX_VERSION_CODE
#elif KERNEL_VERSION(5, 11, 0) <= LINUX_VERSION_CODE
#include <linux/dma-buf-map.h>
#endif
#if KERNEL_VERSION(5, 16, 0) <= LINUX_VERSION_CODE
#include <drm/drm_prime.h>
#include <drm/drm_file.h>
#elif KERNEL_VERSION(5, 5, 0) <= LINUX_VERSION_CODE || defined(EL8)
#else
#include <drm/drmP.h>
#endif
#include "msdisp_drm_drv.h"
#include <linux/shmem_fs.h>
#include <linux/dma-buf.h>
#include <drm/drm_cache.h>

#if KERNEL_VERSION(5, 16, 0) <= LINUX_VERSION_CODE
MODULE_IMPORT_NS(DMA_BUF);
#endif

#if KERNEL_VERSION(5, 11, 0) <= LINUX_VERSION_CODE || defined(EL8)
static int msdisp_drm_prime_pin(struct drm_gem_object *obj);
static void msdisp_drm_prime_unpin(struct drm_gem_object *obj);

static const struct vm_operations_struct msdisp_drm_gem_vm_ops = {
	.fault = msdisp_drm_gem_fault,
	.open = drm_gem_vm_open,
	.close = drm_gem_vm_close,
};

static struct drm_gem_object_funcs gem_obj_funcs = {
	.free = msdisp_drm_gem_free_object,
	.pin = msdisp_drm_prime_pin,
	.unpin = msdisp_drm_prime_unpin,
	.vm_ops = &msdisp_drm_gem_vm_ops,
	.export = drm_gem_prime_export,
	.get_sg_table = msdisp_drm_prime_get_sg_table,
};
#endif

static bool msdisp_drm_was_called_by_mutter(void)
{
	char task_comm[TASK_COMM_LEN] = { 0 };

	get_task_comm(task_comm, current);

	return strcmp(task_comm, "gnome-shell") == 0;
}

static bool msdisp_drm_gem_object_use_import_attach(struct drm_gem_object *obj)
{
	if (!obj || !obj->import_attach) {
		return false;
	}

	return true;
	//return obj->import_attach && 
	//       strcmp(obj->import_attach->dmabuf->owner->name, "amdgpu") != 0;
}

uint32_t msdisp_drm_gem_object_handle_lookup(struct drm_file *filp,
				       struct drm_gem_object *obj)
{
	uint32_t it_handle = 0;
	struct drm_gem_object *it_obj = NULL;

	spin_lock(&filp->table_lock);
	idr_for_each_entry(&filp->object_idr, it_obj, it_handle) {
		if (it_obj == obj)
			break;
	}
	spin_unlock(&filp->table_lock);

	if (!it_obj)
		it_handle = 0;

	return it_handle;
}

struct msdisp_drm_gem_object *msdisp_drm_gem_alloc_object(struct drm_device *dev,
					      size_t size)
{
	struct msdisp_drm_gem_object *obj;

	obj = kzalloc(sizeof(*obj), GFP_KERNEL);
	if (obj == NULL)
		return NULL;

	if (drm_gem_object_init(dev, &obj->base, size) != 0) {
		kfree(obj);
		return NULL;
	}

#if KERNEL_VERSION(5, 4, 0) <= LINUX_VERSION_CODE || defined(EL8)
	dma_resv_init(&obj->_resv);
#else
	reservation_object_init(&obj->_resv);
#endif
	obj->resv = &obj->_resv;

#if KERNEL_VERSION(5, 11, 0) <= LINUX_VERSION_CODE || defined(EL8)
	obj->base.funcs = &gem_obj_funcs;
#endif

	obj->allow_sw_cursor_rect_updates = false;

	mutex_init(&obj->pages_lock);

	return obj;
}

static int msdisp_drm_gem_create(struct drm_file *file,
		struct drm_device *dev, uint64_t size, uint32_t *handle_p)
{
	struct msdisp_drm_gem_object *obj;
	int ret;
	u32 handle;

	size = roundup(size, PAGE_SIZE);

	obj = msdisp_drm_gem_alloc_object(dev, size);
	if (obj == NULL)
		return -ENOMEM;

	ret = drm_gem_handle_create(file, &obj->base, &handle);
	if (ret) {
		drm_gem_object_release(&obj->base);
		kfree(obj);
		return ret;
	}
#if KERNEL_VERSION(5, 9, 0) <= LINUX_VERSION_CODE || defined(EL8)
	drm_gem_object_put(&obj->base);
#else
	drm_gem_object_put_unlocked(&obj->base);
#endif
	obj->allow_sw_cursor_rect_updates = msdisp_drm_was_called_by_mutter();
	*handle_p = handle;
	return 0;
}

static int msdisp_drm_align_pitch(int width, int cpp)
{
	int aligned = width;
	int pitch_mask = 0;

	switch (cpp) {
	case 1:
		pitch_mask = 255;
		break;
	case 2:
		pitch_mask = 127;
		break;
	case 3:
	case 4:
		pitch_mask = 63;
		break;
	}

	aligned += pitch_mask;
	aligned &= ~pitch_mask;
	return aligned * cpp;
}

int msdisp_drm_dumb_create(struct drm_file *file, struct drm_device *dev, struct drm_mode_create_dumb *args)
{
	args->pitch = msdisp_drm_align_pitch(args->width, DIV_ROUND_UP(args->bpp, 8));

	args->size = args->pitch * args->height;
	return msdisp_drm_gem_create(file, dev, args->size, &args->handle);
}

int msdisp_drm_gem_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int ret;

	ret = drm_gem_mmap(filp, vma);
	if (ret)
		return ret;
	
#if KERNEL_VERSION(6, 6, 0) <= LINUX_VERSION_CODE
	vm_flags_clear(vma, VM_PFNMAP);
	vm_flags_set(vma, VM_MIXEDMAP);
#else
	vma->vm_flags &= ~VM_PFNMAP;
	vma->vm_flags |= VM_MIXEDMAP;
#endif

	return ret;
}

#if KERNEL_VERSION(4, 17, 0) <= LINUX_VERSION_CODE
vm_fault_t msdisp_drm_gem_fault(struct vm_fault *vmf)
{
	struct vm_area_struct *vma = vmf->vma;
#else
int msdisp_drm_gem_fault(struct vm_fault *vmf)
{
	struct vm_area_struct *vma = vmf->vma;
#endif
	struct msdisp_drm_gem_object *obj = to_msdisp_drm_bo(vma->vm_private_data);
	struct page *page;
	unsigned int page_offset;
	int ret = 0;

	page_offset = (vmf->address - vma->vm_start) >> PAGE_SHIFT;

	if (!obj->pages)
		return VM_FAULT_SIGBUS;

	page = obj->pages[page_offset];
	ret = vm_insert_page(vma, vmf->address, page);
	switch (ret) {
	case -EAGAIN:
	case 0:
	case -ERESTARTSYS:
		return VM_FAULT_NOPAGE;
	case -ENOMEM:
		return VM_FAULT_OOM;
	default:
		return VM_FAULT_SIGBUS;
	}
	return VM_FAULT_SIGBUS;
}

static int msdisp_drm_gem_get_pages(struct msdisp_drm_gem_object *obj,
			      __always_unused gfp_t gfpmask)
{
	struct page **pages;

	if (obj->pages)
		return 0;

	pages = drm_gem_get_pages(&obj->base);

	if (IS_ERR(pages))
		return PTR_ERR(pages);

	obj->pages = pages;

#if defined(CONFIG_X86)
	drm_clflush_pages(obj->pages, DIV_ROUND_UP(obj->base.size, PAGE_SIZE));
#endif

	return 0;
}

static void msdisp_drm_gem_put_pages(struct msdisp_drm_gem_object *obj)
{
	if (obj->base.import_attach) {
		kvfree(obj->pages);
		obj->pages = NULL;
		return;
	}

	drm_gem_put_pages(&obj->base, obj->pages, false, false);
	obj->pages = NULL;
}

static int msdisp_drm_pin_pages(struct msdisp_drm_gem_object *obj)
{
	int ret = 0;

	mutex_lock(&obj->pages_lock);
	if (obj->pages_pin_count++ == 0) {
		ret = msdisp_drm_gem_get_pages(obj, GFP_KERNEL);
		if (ret)
			obj->pages_pin_count--;
	}
	mutex_unlock(&obj->pages_lock);
	return ret;
}

static void msdisp_drm_unpin_pages(struct msdisp_drm_gem_object *obj)
{
	mutex_lock(&obj->pages_lock);
	if (--obj->pages_pin_count == 0)
		msdisp_drm_gem_put_pages(obj);
	mutex_unlock(&obj->pages_lock);
}

int msdisp_drm_gem_vmap(struct msdisp_drm_gem_object *obj)
{
	int page_count = DIV_ROUND_UP(obj->base.size, PAGE_SIZE);
	int ret;

	if (msdisp_drm_gem_object_use_import_attach(&obj->base)) {
#if KERNEL_VERSION(5, 18, 0) <= LINUX_VERSION_CODE
		struct iosys_map map = IOSYS_MAP_INIT_VADDR(NULL);
#elif KERNEL_VERSION(5, 11, 0) <= LINUX_VERSION_CODE || defined(EL8)
		struct dma_buf_map map = DMA_BUF_MAP_INIT_VADDR(NULL);
#endif

#if KERNEL_VERSION(5, 11, 0) <= LINUX_VERSION_CODE || defined(EL8)
		ret = dma_buf_vmap(obj->base.import_attach->dmabuf, &map);
		if (ret)
			return -ENOMEM;
		obj->vmapping = map.vaddr;
		obj->vmap_is_iomem = map.is_iomem;
#else
		obj->vmapping = dma_buf_vmap(obj->base.import_attach->dmabuf);
		if (!obj->vmapping)
			return -ENOMEM;
#endif
		return 0;
	}

	ret = msdisp_drm_pin_pages(obj);
	if (ret)
		return ret;

	obj->vmapping = vmap(obj->pages, page_count, 0, PAGE_KERNEL);
	if (!obj->vmapping)
		return -ENOMEM;
	return 0;
}

void msdisp_drm_gem_vunmap(struct msdisp_drm_gem_object *obj)
{
	if (msdisp_drm_gem_object_use_import_attach(&obj->base)) {
#if KERNEL_VERSION(5, 18, 0) <= LINUX_VERSION_CODE
		struct iosys_map map = IOSYS_MAP_INIT_VADDR(NULL);

		if (obj->vmap_is_iomem)
			iosys_map_set_vaddr_iomem(&map, obj->vmapping);
		else
			iosys_map_set_vaddr(&map, obj->vmapping);

		dma_buf_vunmap(obj->base.import_attach->dmabuf, &map);

#elif KERNEL_VERSION(5, 11, 0) <= LINUX_VERSION_CODE || defined(EL8)
		struct dma_buf_map map;

		if (obj->vmap_is_iomem)
			dma_buf_map_set_vaddr_iomem(&map, obj->vmapping);
		else
			dma_buf_map_set_vaddr(&map, obj->vmapping);

		dma_buf_vunmap(obj->base.import_attach->dmabuf, &map);
#else
		dma_buf_vunmap(obj->base.import_attach->dmabuf, obj->vmapping);
#endif
		obj->vmapping = NULL;
		return;
	}

	if (obj->vmapping) {
		vunmap(obj->vmapping);
		obj->vmapping = NULL;
	}

	msdisp_drm_unpin_pages(obj);
}

void msdisp_drm_gem_free_object(struct drm_gem_object *gem_obj)
{
	struct msdisp_drm_gem_object *obj = to_msdisp_drm_bo(gem_obj);

	if (obj->vmapping)
		msdisp_drm_gem_vunmap(obj);

	if (gem_obj->import_attach)
		drm_prime_gem_destroy(gem_obj, obj->sg);

	if (obj->pages)
		msdisp_drm_gem_put_pages(obj);

	if (gem_obj->dev->vma_offset_manager)
		drm_gem_free_mmap_offset(gem_obj);
#if KERNEL_VERSION(5, 4, 0) <= LINUX_VERSION_CODE || defined(EL8)
	dma_resv_fini(&obj->_resv);
#else
	reservation_object_fini(&obj->_resv);
#endif
	obj->resv = NULL;
	mutex_destroy(&obj->pages_lock);
}

/*
 * the dumb interface doesn't work with the GEM straight MMAP
 * interface, it expects to do MMAP on the drm fd, like normal
 */
int msdisp_drm_gem_mmap_offset(struct drm_file *file,
		  struct drm_device *dev, uint32_t handle, uint64_t *offset)
{
	struct msdisp_drm_gem_object *gobj;
	struct drm_gem_object *obj;
	int ret = 0;

	mutex_lock(&dev->struct_mutex);
	obj = drm_gem_object_lookup(file, handle);
	if (obj == NULL) {
		ret = -ENOENT;
		goto unlock;
	}
	gobj = to_msdisp_drm_bo(obj);

	ret = msdisp_drm_pin_pages(gobj);
	if (ret)
		goto out;

	ret = drm_gem_create_mmap_offset(obj);
	if (ret)
		goto out;

	*offset = drm_vma_node_offset_addr(&gobj->base.vma_node);

 out:
	drm_gem_object_put(&gobj->base);
 unlock:
	mutex_unlock(&dev->struct_mutex);
	return ret;
}

#if KERNEL_VERSION(5, 11, 0) <= LINUX_VERSION_CODE || defined(EL8)
static int msdisp_drm_prime_pin(struct drm_gem_object *obj)
{
	struct msdisp_drm_gem_object *bo = to_msdisp_drm_bo(obj);

	return msdisp_drm_pin_pages(bo);
}

static void msdisp_drm_prime_unpin(struct drm_gem_object *obj)
{
	struct msdisp_drm_gem_object *bo = to_msdisp_drm_bo(obj);

	msdisp_drm_unpin_pages(bo);
}
#endif


struct sg_table * msdisp_drm_prime_get_sg_table(struct drm_gem_object *obj)
{
	struct msdisp_drm_gem_object *bo = to_msdisp_drm_bo(obj);
	#if KERNEL_VERSION(5, 10, 0) <= LINUX_VERSION_CODE || defined(EL8)
		return drm_prime_pages_to_sg(obj->dev, bo->pages, bo->base.size >> PAGE_SHIFT);
	#else
		return drm_prime_pages_to_sg(bo->pages, bo->base.size >> PAGE_SHIFT);
	#endif
}

struct drm_gem_object * msdisp_drm_prime_import_sg_table(struct drm_device *dev,
			   struct dma_buf_attachment *attach,
			   struct sg_table *sg)
{
	struct msdisp_drm_gem_object *obj;
	int npages;

	obj = msdisp_drm_gem_alloc_object(dev, attach->dmabuf->size);
	if (IS_ERR(obj))
		return ERR_CAST(obj);

	npages = DIV_ROUND_UP(attach->dmabuf->size, PAGE_SIZE);
	DRM_DEBUG_PRIME("Importing %d pages\n", npages);
	obj->pages = kvmalloc_array(npages, sizeof(struct page *), GFP_KERNEL);
	if (!obj->pages) {
		msdisp_drm_gem_free_object(&obj->base);
		return ERR_PTR(-ENOMEM);
	}

#if KERNEL_VERSION(5, 12, 0) <= LINUX_VERSION_CODE || defined(EL8)
	drm_prime_sg_to_page_array(sg, obj->pages, npages);
#else
	drm_prime_sg_to_page_addr_arrays(sg, obj->pages, NULL, npages);
#endif
	obj->sg = sg;

	return &obj->base;
}

