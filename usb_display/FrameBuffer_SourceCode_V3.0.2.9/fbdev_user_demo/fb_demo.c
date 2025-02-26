#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>


static int fd_fb;
static struct fb_fix_screeninfo fix;    /* Current fix */
static struct fb_var_screeninfo var;    /* Current var */
static int screen_size;
static unsigned char *fb_base;
static unsigned int line_width;
static unsigned int pixel_width;

void print_usage(char* app)
{
    printf("%s usage:\n", app);
    printf("%s [-d dev_id] [-l loop count] [-s widthxheight]\n", app);
    printf("dev_id: fbdev's minor code. for example: 0 stands /dev/fb0\n");
    printf("loop: loop count\n");
    printf("widthxheight: for example: 1920x1080\n");
}

void lcd_draw_screen(char* buf, unsigned int colors)
{
    int i;
    int width = var.xres;
    int height = var.yres;
    int cpp = var.bits_per_pixel / 8;

    for (i = 0; i < width * height; i++) {
        buf[i * 3] = ((colors & 0xff0000) >> 16);
        buf[i * 3 + 1] = ((colors & 0xff00) >> 8);
        buf[i * 3 + 2] = (colors & 0xff); 
    }
}

int main(int argc, char **argv)
{
    int i;
    int ret;
    int buffer_num;
    int buf_idx = 1;
    char *buf_next;
    char path[256];
    int dev_id = 0, loop = 3, width = 0, height = 0;
    int ch, usage_flag = 0;
    unsigned int colors[] = {0x00FF0000, 0x0000FF00, 0x000000FF, 0, 0x00FFFFFF};  /* 0x00BBGGRR */

    memset(path, 0, 256);

    while ((ch = getopt(argc, argv, "hd:l:s:")) != -1) {
        switch (ch) {
            case 'h':
                usage_flag = 1;
                break;
            case 'd':
                dev_id = strtod(optarg, NULL);
                break;
            case 'l':
                loop = strtod(optarg, NULL);
                break;
            case 's':
                sscanf(optarg,"%dx%d", &width, &height);
                break;
            default:
                usage_flag = 1;
                break;
        }
    }

    if (usage_flag) {
        print_usage(argv[0]);
        return 0;
    }

    snprintf(path, 256, "/dev/fb%d", dev_id);
    printf("dev path:%s\n", path);

    fd_fb = open(path, O_RDWR);
    //fd_fb = open("/dev/fb1", O_RDWR);
    printf("fd:%d\n", fd_fb);
    if (fd_fb < 0) {
        printf("open %s failed!\n", path);
        return -1;
    }
    ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix);
    ioctl(fd_fb, FBIOGET_VSCREENINFO, &var);

    if ((width != 0) && (height != 0)) {
        if (var.xres == width && var.yres == height) {
            printf("The frame size is same, need not set varscreeninfo\n");
        } else {
            var.xres = var.xres_virtual = width;
            var.yres = height;
            var.yres_virtual = height * 2;

            ret = ioctl(fd_fb, FBIOPUT_VSCREENINFO,&var);
            if (ret) {
                printf("put vscreen info failed!ret=%d\n", ret);
                close(fd_fb);
                return ret;
            }
        }
    }

    line_width  = var.xres * var.bits_per_pixel / 8;
    pixel_width = var.bits_per_pixel / 8;
    screen_size = var.xres * var.yres * var.bits_per_pixel / 8;

    // 1. 获得 buffer 个数
    buffer_num = fix.smem_len / screen_size;
    printf("buffer_num = %d, smem len:%d\n", buffer_num, fix.smem_len);
    
    fb_base = (unsigned char *)mmap(NULL , fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
    if (fb_base == (unsigned char *)-1) {
        printf("can't mmap\n");
        return -1;
    }

    printf("double buffer:\n");

    // 2. 使能多 buffer
    var.yres_virtual = buffer_num * var.yres;
    ret = ioctl(fd_fb, FBIOPUT_VSCREENINFO, &var);
    if (ret) {
        printf("put vscreen info failed! ret=%d\n", ret);
    }

    printf("put vscreeninfo finished!\n");
    while (loop) {
        for (i = 0; i < sizeof(colors)/sizeof(colors[0]); i++) {

            // 3. 更新 buffer 里的数据
            buf_next =  fb_base + buf_idx * screen_size;
            lcd_draw_screen(buf_next, colors[i]);

            // 4. 通知驱动切换 buffer
            var.yoffset = buf_idx * var.yres;
            ret = ioctl(fd_fb, FBIOPAN_DISPLAY, &var);
            if (ret < 0) {
                perror("ioctl() / FBIOPAN_DISPLAY");
            }

            buf_idx = !buf_idx;
            sleep(1);
        }
        loop--;
    }
    
    munmap(fb_base , screen_size);
    close(fd_fb);
    
    return 0;   
}

