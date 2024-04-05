#include <stdlib.h>
#include <string.h>

#include <bsp/board.h>
#include <pico/stdlib.h>
#include <tusb.h>
#include <ff.h>

#include "bootsel_button.h"
#include "flash.h"


static FATFS filesystem;


/*
 * `fatfs_flash_driver.c:disk_initialize()` is called to test the file
 * system and initialize it if necessary.
 */
static void test_and_init_filesystem(void) {
    f_mount(&filesystem, "/", 1);
    f_unmount("/");
}

static void read_write_task(void) {
    static bool last_status = false;
    static int count = 0;
    static uint64_t long_push = 0;

    bool button = bb_get_bootsel_button();
    FIL fp;
    FRESULT res;

    if (last_status != button && button) {  // Push BOOTSEL button
        printf("read README.TXT\n");
        res = f_mount(&filesystem, "/", 1);
        if (res != FR_OK) {
            printf("f_mount fail rc=%d\n", res);
            last_status = button;
            return;
        }
        res = f_open(&fp, "README.TXT", FA_READ);
        if (res == FR_OK) {
            uint8_t buffer[512];
            UINT bytes;
            memset(buffer, 0, sizeof(buffer));
            f_read(&fp, buffer, sizeof(buffer), &bytes);
            printf("%s", buffer, bytes);
            if (sscanf(buffer, "count=%d", &count) != 1) {
                count = 0;
            }
            f_close(&fp);
        } else {
            printf("can't read README.txt: %d\n", res);
            count = 0;
        }
        f_unmount("/");

    } else if (last_status != button && !button) { // Release BOOTSEL button
        printf("update README.txt\n");
        res = f_mount(&filesystem, "/", 1);
        if (res != FR_OK) {
            printf("f_mount fail rc=%d\n", res);
            last_status = button;
            return;
        }
        res = f_open(&fp, "README.TXT", FA_WRITE|FA_CREATE_ALWAYS);
        if (res == FR_OK) {
            count++;
            uint8_t buffer[512];
            int size = snprintf(buffer, sizeof(buffer), "count=%d\n", count);
            UINT writed = 0;
            res = f_write(&fp, buffer, size, &writed);
            f_close(&fp);
        } else {
            printf("can't update README.txt: %d\n", res);
        }
        f_unmount("/");
    }
    last_status = button;

    if (button) {
        long_push++;
    } else {
        long_push = 0;
    }
    if (long_push > 125000) { // Long-push BOOTSEL button
        printf("Formate FAT12 on flash memory\n");
        flash_fat_initialize();
        count = 0;
        long_push = 0;
    }
}

int main() {
    board_init();
    tud_init(BOARD_TUD_RHPORT);
    stdio_init_all();

    test_and_init_filesystem();
    while (true) {
         read_write_task();
         tud_task();
    }
}
