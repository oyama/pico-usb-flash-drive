/*
 * This sample formats Pico's onboard flash memory with FAT12 to enable the
 * following operations:
 *
 * - Read/write files from/to host PC with USB Mass Storage class
 * - Read/write files from inside Pico with FatFs library
 *
 * Pressing the BOOTSEL button on Pico reads a file and releasing it updates the
 * file. Also, holding the button down for about 10 seconds initializes the file
 * system.
 */
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

static bool task_read_file_content(int *count) {
    printf("read README.TXT\n");

    FRESULT res = f_mount(&filesystem, "/", 1);
    if (res != FR_OK) {
        printf("f_mount fail rc=%d\n", res);
        return false;
    }

    bool result = false;
    FIL fp;
    res = f_open(&fp, "README.TXT", FA_READ);
    if (res == FR_OK) {
        uint8_t buffer[512];
        UINT length;
        //memset(buffer, 0, sizeof(buffer));
        f_read(&fp, buffer, sizeof(buffer), &length);
        printf("%s", buffer);
        if (sscanf(buffer, "count=%d", count) != 1) {
            // invalid file format, reset.
            *count = 0;
        }
        f_close(&fp);
        result = true;
    } else {
        printf("can't open README.txt: %d\n", res);
        count = 0;
        result = false;
    }
    f_unmount("/");
    return result;
}

static bool task_write_file_content(int count) {
    printf("update README.txt\n");

    FRESULT res = f_mount(&filesystem, "/", 1);
    if (res != FR_OK) {
        printf("f_mount fail rc=%d\n", res);
        return false;
    }

    bool result = false;
    FIL fp;
    res = f_open(&fp, "README.TXT", FA_WRITE|FA_CREATE_ALWAYS);
    if (res == FR_OK) {
        count++;
        uint8_t buffer[512];
        int size = snprintf(buffer, sizeof(buffer), "count=%d\n", count);
        UINT writed = 0;
        res = f_write(&fp, buffer, size, &writed);
        f_close(&fp);
        result = true;
    } else {
        printf("can't update README.txt: %d\n", res);
        result = false;
    }
    f_unmount("/");
    return result;
}

static void read_write_task(void) {
    static bool last_status = false;
    static int count = 0;
    static uint64_t long_push = 0;

    bool button = bb_get_bootsel_button();
    if (last_status != button && button) {  // Push BOOTSEL button
        task_read_file_content(&count);
    } else if (last_status != button && !button) { // Release BOOTSEL button
        task_write_file_content(count);
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
