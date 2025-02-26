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
 * usb_hal_thread.c -- Drm driver for MacroSilicon chip 913x and 912x
 */
 
#include <linux/slab.h>
#include <linux/scatterlist.h>  /* For struct sg_table */
#include <linux/usb.h>
#include <linux/kfifo.h>

#include "usb_hal_interface.h"
#include "usb_hal_dev.h"
#include "usb_hal_event.h"
#include "usb_hal_thread.h"
#include "hal_adaptor.h"

struct usb_hal_api_context {
	struct completion	done;
	int			status;
};

static void usb_hal_api_blocking_completion(struct urb *urb)
{
	struct usb_hal_api_context *ctx = urb->context;

	ctx->status = urb->status;
	complete(&ctx->done);
}

static int usb_hal_start_wait_urb(struct urb *urb, int timeout, int *actual_length)
{
	struct usb_hal_api_context ctx;
	unsigned long expire;
	int retval;

	init_completion(&ctx.done);
	urb->context = &ctx;
	urb->actual_length = 0;
	retval = usb_submit_urb(urb, GFP_NOIO);
	if (unlikely(retval))
		goto out;

	expire = timeout ? msecs_to_jiffies(timeout) : MAX_SCHEDULE_TIMEOUT;
	if (!wait_for_completion_timeout(&ctx.done, expire)) {
		usb_kill_urb(urb);
		retval = (ctx.status == -ENOENT ? -ETIMEDOUT : ctx.status);
	} else
		retval = ctx.status;
out:
	if (actual_length)
		*actual_length = urb->actual_length;

	return retval;
}

static int usb_hal_dev_draw_cursor(struct usb_hal_dev* usb_dev)
{
	u8 *pimage;
	u8 *pmouse;
	u8 *pold;  
	int row,col;
	int start_row, end_row, start_col, end_col;
	int img_stride = usb_dev->mode.width * 3;

	//remove old cursor if exist
	struct usb_hal_cursor_buffer* cursor_buf = &usb_dev->old_cursor_buf;
	if(cursor_buf->valid) {
		start_row = cursor_buf->y1;
		start_col = cursor_buf->x1;
		
		end_row = cursor_buf->y2;
		end_col = cursor_buf->x2;

		//copy back the old mouse
		pimage = usb_dev->usb_buf.buf + start_row * img_stride + start_col * 3;
		pold = cursor_buf->buf;

		for (row = start_row; row < end_row; row++) {
			memcpy(pimage, pold, (end_col - start_col) * 3);
			pimage += img_stride;
			pold += (end_col - start_col) * 3;
		}
	}

	//add new cursor if exist
	cursor_buf = &usb_dev->cursor_buf;
	if(cursor_buf->valid) {
		start_row = cursor_buf->y + cursor_buf->y1;
		if (start_row < 0) start_row = 0;
		start_col = cursor_buf->x + cursor_buf->x1;
		if (start_col < 0) start_col = 0;	
		
		end_row = cursor_buf->y + cursor_buf->y2;
		if (end_row > usb_dev->mode.height) end_row = usb_dev->mode.height;
		end_col = cursor_buf->x + cursor_buf->x2;
		if (end_col > usb_dev->mode.width) end_col = usb_dev->mode.width;

		if (start_row >= end_row || start_col >= end_col) {
			usb_dev->old_cursor_buf.valid = false;
		}
		else
		{
			//copy back the old mouse
			pimage = usb_dev->usb_buf.buf + start_row * img_stride + start_col * 3;
			pmouse = usb_dev->cursor_buf.buf + 
					((start_row - cursor_buf->y) * USB_HAL_CURSOR_WIDTH + 
					(start_col - cursor_buf->x) ) * 4;
			pold = usb_dev->old_cursor_buf.buf;

			for (row = start_row; row < end_row; row++) {
				u8 *pimage_row = pimage;
				u8 *pmouse_row = pmouse;

				memcpy(pold, pimage_row, (end_col-start_col) * 3);

				for (col = start_col; col < end_col; col++) {
					// u8 image_b = pimage_row[0];
					// u8 image_g = pimage_row[1]
					// u8 image_r = pimage_row[2];
					// u8 mouse_b = pmouse_row[0];
					// u8 mouse_g = pmouse_row[1]
					// u8 mouse_r = pmouse_row[2];
					

					if (pmouse_row[3] >= 192) {
						pimage_row[0] = pmouse_row[0];
						pimage_row[1] = pmouse_row[1];
						pimage_row[2] = pmouse_row[2];
					} else if (pmouse_row[3] >= 128) {
						pimage_row[0] = ((pmouse_row[0] >> 2) + (pimage_row[0] >> 2) + (pimage_row[0] >> 1));
						pimage_row[1] = ((pmouse_row[1] >> 2) + (pimage_row[1] >> 2) + (pimage_row[1] >> 1));
						pimage_row[2] = ((pmouse_row[2] >> 2) + (pimage_row[2] >> 2) + (pimage_row[2] >> 1));
					} else if (pmouse_row[3] >= 64) {
						pimage_row[0] = ((pmouse_row[0] >> 2) + (pmouse_row[0] >> 1) + (pimage_row[0] >> 2));
						pimage_row[1] = ((pmouse_row[1] >> 2) + (pmouse_row[1] >> 1) + (pimage_row[1] >> 2));
						pimage_row[2] = ((pmouse_row[2] >> 2) + (pmouse_row[2] >> 1) + (pimage_row[2] >> 2));
					}
					pmouse_row += 4;
					pimage_row += 3;
				}

				pimage += img_stride;
				pmouse += USB_HAL_CURSOR_WIDTH * 4;
				pold += (end_col - start_col) * 3;
			}
			
			usb_dev->old_cursor_buf.x1 = start_col;
			usb_dev->old_cursor_buf.y1 = start_row;
			usb_dev->old_cursor_buf.x2 = end_col;
			usb_dev->old_cursor_buf.y2 = end_row;
			usb_dev->old_cursor_buf.valid = true;
		}
	}
	else
	{
		usb_dev->old_cursor_buf.valid = false;
	}

	return 0;
}

//#define CHAR_RANGE(a) (a<0?0:(a>255?255:a))
static void usb_hal_rgb_to_yuv(struct usb_hal_dev* usb_dev)
{
    int i, image_size;
	int b1, g1, r1, b2, g2, r2;
	int y0, y1, u0, v0;
    struct usb_hal_buffer* usb_buf = &usb_dev->usb_buf;
	u8* psrc = usb_buf->buf;
	u8* pdst = usb_buf->buf;

    image_size = usb_dev->mode.width * usb_dev->mode.height;
    for (i = 0; i < image_size; i += 2) {
        b1 = *psrc++;
        g1 = *psrc++;
        r1 = *psrc++;

        b2 = *psrc++;
        g2 = *psrc++;
        r2 = *psrc++;

        y0 = ((263 * r1 + 516 * g1 + 97 * b1) >> 10) + 16;
        u0 = ((-152 * r1 - 298 * g1 + 450 * b1) >> 10) + 128;
        v0 = ((450 * r1 - 377 * g1 - 73 * b1) >> 10) + 128;

        y1 = ((263 * r2 + 516 * g2 + 97 * b2) >> 10) + 16;
        u0 += ((-152 * r2 - 298 * g2 + 450 * b2) >> 10) + 128;
        v0 += ((450 * r2 - 377 * g2 - 73 * b2) >> 10) + 128;

        u0 >>= 1;
        v0 >>= 1;

		// u0 = CHAR_RANGE(u0);
		// v0 = CHAR_RANGE(v0);
		// y0 = CHAR_RANGE(y0);
		// y1 = CHAR_RANGE(y1);

        *pdst++ = (u8)u0;
        *pdst++ = (u8)y0;
        *pdst++ = (u8)v0;    
        *pdst++ = (u8)y1;
    }

    usb_dev->usb_buf.len = image_size * 2;
}

static int usb_hal_dev_send_frame(struct usb_hal_dev* usb_dev, struct urb* data_urb, unsigned char* zero_msg, int ep)
{
	int real_ret, ret, snd_len;
	s64 elapsed_ms;
	ktime_t start_time, end_time;
	struct usb_device* udev = usb_dev->udev;

	real_ret = 0;
	usb_dev->stat.send_total++;
	start_time = ktime_get();

	mutex_lock(&usb_dev->usb_buf.mutex);
	memcpy(usb_dev->usb_buf.buf, usb_dev->desktop_buf.buf, usb_dev->desktop_buf.len);
	mutex_unlock(&usb_dev->usb_buf.mutex);

	mutex_lock(&usb_dev->cursor_buf.mutex);
	usb_hal_dev_draw_cursor(usb_dev);
	mutex_unlock(&usb_dev->cursor_buf.mutex);

	usb_hal_rgb_to_yuv(usb_dev);

	start_time = ktime_get();
	usb_fill_bulk_urb(data_urb, udev, usb_sndbulkpipe(udev, ep), usb_dev->usb_buf.buf, usb_dev->usb_buf.len,
			usb_hal_api_blocking_completion, NULL);
		
	if ((USB_HAL_BUF_TYPE_USB == usb_dev->usb_buf.type ) || (USB_HAL_BUF_TYPE_DMA == usb_dev->usb_buf.type)) {
		data_urb->transfer_dma = usb_dev->usb_buf.dma_addr;
		data_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	} else if (USB_HAL_BUF_TYPE_VMALLOC == usb_dev->usb_buf.type) {
		data_urb->num_sgs = usb_dev->usb_buf.sgt->orig_nents;
		data_urb->sg = usb_dev->usb_buf.sgt->sgl; 
	}

	ret = usb_hal_start_wait_urb(data_urb, 2000, &snd_len);
	if (ret) {
		dev_err(&udev->dev, "wait urb failed!\n ret = %d\n", ret);
		real_ret = ret;
	} else {
		usb_dev->stat.send_success++;
	}

    ret = usb_bulk_msg(udev, usb_sndbulkpipe(udev, ep), zero_msg, 0, &snd_len, 2000);
    if (ret) {
        dev_err(&udev->dev, "send zero msg failed! ret=%d\n", ret);
    }
	end_time = ktime_get();

	elapsed_ms = ktime_to_ms(ktime_sub(end_time, start_time));
	if(elapsed_ms > 100) {
		dev_info(&udev->dev, "send frame Elapsed time: %lld ms\n", elapsed_ms);
	}

	usb_dev->frame_index = ((0 == usb_dev->frame_index) ? 1 : 0);
   	usb_dev->hal_dev->funcs->trigger_frame(usb_dev->udev, usb_dev->frame_index, 100);

	return real_ret;
}

static void usb_hal_dev_do_enable(struct usb_hal_dev* usb_dev, struct usb_hal_event* event)
{
    unsigned char frame_index = 0;
    int ret;
    const struct msdisp_hal_dev* hal_dev = usb_dev->hal_dev;
    struct usb_device* udev = usb_dev->udev;
	struct usb_hal* hal = usb_dev->hal;

    ret = hal_dev->funcs->event_proc(usb_dev->udev, event, hal->chip_id, hal->port_type, hal->sdram_type);
    if (ret) {
        dev_err(&udev->dev, "hal dev enable event proc failed! ret=%d\n", ret);
    }

    ret = hal_dev->funcs->current_frame_index(udev, &frame_index);
	if (!ret) {
		usb_dev->frame_index = frame_index;
		dev_info(&udev->dev, "current index:%d\n", frame_index);
	} else {
		dev_info(&udev->dev, "get frame index failed! rtn = %d\n", ret);
		usb_dev->frame_index = 0;
	}

	usb_dev->state = USB_HAL_DEV_STATE_ENABLED;
	usb_dev->first_buf_send = 0;
}

static void usb_hal_dev_do_disable(struct usb_hal_dev* usb_dev, struct usb_hal_event* event)
{
    int ret;
    const struct msdisp_hal_dev* hal_dev = usb_dev->hal_dev;
    struct usb_device* udev = usb_dev->udev;
	struct usb_hal* hal = usb_dev->hal;

    ret = hal_dev->funcs->event_proc(usb_dev->udev, event, hal->chip_id, hal->port_type, hal->sdram_type);
    if (ret) {
        dev_err(&udev->dev, "hal dev disable event proc failed! ret=%d\n", ret);
    }

    usb_dev->state = USB_HAL_DEV_STATE_DISABLED;
}

static void usb_hal_dev_do_update(struct usb_hal_dev* usb_dev, struct urb* data_urb, unsigned char* zero_msg, int ep)
{
    int ret;
	const struct msdisp_hal_dev* hal_dev = usb_dev->hal_dev;
	struct usb_device *udev = usb_dev->udev;

	usb_dev->stat.update_event++;
	usb_dev->wait_send_cnt = 0;

	ret = usb_hal_dev_send_frame(usb_dev, data_urb, zero_msg, ep);
	if (ret) {
		goto out;
	}

	if (0 == usb_dev->first_buf_send) {
		struct usb_hal* hal = usb_dev->hal;
		ret = hal_dev->funcs->set_video_enable(udev, 1);
    	if (ret) {
        	dev_err(&udev->dev, "start video failed! rtn = %d\n", ret);
		}

		ret = hal_dev->funcs->set_screen_enable(udev, 1, hal->chip_id, hal->port_type, hal->sdram_type);
    	if (ret) {
        	dev_err(&udev->dev, "stop screen failed! rtn = %d\n", ret);
    	}

		usb_dev->first_buf_send = 1;
		dev_info(&udev->dev, "start video success!\n");
	}
out:
	return;
}

void usb_hal_dev_state_unknown(struct usb_hal_dev* usb_dev, struct usb_hal_event* event)
{
	if (USB_HAL_EVENT_TYPE_ENABLE == event->base.type ) {
		dev_info(&usb_dev->udev->dev, "event:%x width:%d height:%d\n", event->base.type, event->para.enable.width, \
        event->para.enable.height);
	}
    
	/* in unknown state, only process enable event*/
	if (event->base.type != USB_HAL_EVENT_TYPE_ENABLE) {
		return;
	}

    usb_hal_dev_do_enable(usb_dev, event);
}

// void usb_hal_dev_state_enable(struct usb_hal_dev* usb_dev, struct urb* data_urb, unsigned char* zero_msg, int ep, struct usb_hal_event* event)
// {
//     if (USB_HAL_EVENT_TYPE_DISABLE == event->base.type) {
//         usb_hal_dev_do_disable(usb_dev, event);
//         return ;
//     }

//     if (USB_HAL_EVENT_TYPE_UPDATE == event->base.type) {
//         usb_hal_dev_do_update(usb_dev, data_urb, zero_msg, ep, event);
//     }
// }

void usb_hal_state_machine(struct usb_hal_dev* usb_dev, struct urb* data_urb, unsigned char* zero_msg, int ep, struct kfifo* fifo)
{
    int len, ret;
	struct usb_hal_event event;
	bool bupdate = false;
	ktime_t current_time;

    ret = down_timeout(&usb_dev->sema, 1);
    // if usb will be suspend, not process, until usb resume
	if ( MS9132_USB_BUS_STATUS_SUSPEND == usb_dev->bus_status) {
		return;
	}

	// event received, proc event
    if (!ret) {
        while ((len = kfifo_out(fifo, &event, sizeof(event)) != 0)) {
            switch (usb_dev->state) {
                case USB_HAL_DEV_STATE_UNKNOWN:
                case USB_HAL_DEV_STATE_DISABLED:
                    usb_hal_dev_state_unknown(usb_dev, &event);
                    break;

                case USB_HAL_DEV_STATE_ENABLED:
					if (USB_HAL_EVENT_TYPE_DISABLE == event.base.type) {
						usb_hal_dev_do_disable(usb_dev, &event);
					}

					if (USB_HAL_EVENT_TYPE_UPDATE == event.base.type) {
						bupdate = true;
					}
                    break;
	        }
        }
    }

	if (USB_HAL_DEV_STATE_ENABLED != usb_dev->state) return;
	if (!bupdate) {
		current_time = ktime_get();		
		if ((ktime_to_ms(ktime_sub(current_time, usb_dev->updatetime))) >= 10) {
			if (1 == atomic_cmpxchg(&usb_dev->mouse_movingflag, 1, 0)) {
				bupdate = true;
			}
		} 

		if (!bupdate) {
			if ((ktime_to_ms(ktime_sub(current_time, usb_dev->updatetime))) >= 2500) {
				bupdate = true;
			} 
		}
	}

	if (bupdate) {
		usb_dev->updatetime = ktime_get();
		usb_hal_dev_do_update(usb_dev, data_urb, zero_msg, ep);

		return;
	}
}

int usb_hal_state_machine_entry(void* data)
{
    struct usb_hal* usb_hal = (struct usb_hal *)data;
    struct usb_hal_dev* usb_dev = (struct usb_hal_dev*)usb_hal->private;
	struct urb* data_urb;
    int ep;
	int counter = 0;
	unsigned char* zero_msg;
    struct kfifo* fifo;

	dev_info(&usb_dev->udev->dev, "state machine proc enter!\n");

    fifo = usb_dev->fifo;

	zero_msg = kmalloc(8, GFP_KERNEL);
	if (!zero_msg){
		return -ENOMEM;
	}

	data_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!data_urb) {
		kfree(zero_msg);
		return -ENOMEM;
	}

    ep = usb_dev->hal_dev->funcs->get_transfer_bulk_ep();
	/* wait for drm enable */
    while(usb_dev->thread_run_flag) {
        usb_hal_state_machine(usb_dev, data_urb, zero_msg, ep, fifo);		
		counter++;
    }

	usb_free_urb(data_urb);
	kfree(zero_msg);

    return 0;
}

void usb_hal_stop_thread(struct usb_hal_dev *usb_dev)
{
    usb_dev->thread_run_flag = 0;
}