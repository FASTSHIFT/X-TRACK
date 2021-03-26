#include "../Display.h"

#if LV_USE_FILESYSTEM
#include "SdFat.h"
#include "HAL/HAL.h"

extern SdFat SD;

/*********************
 *      DEFINES
 *********************/
#define SD_LETTER 'S'

/**********************
 *      TYPEDEFS
 **********************/

/* Create a type to store the required data about your file.*/
typedef SdFile file_t;

/*Similarly to `file_t` create a type for directory reading too */
typedef FatFile rddir_t;

#define SD_FILE(file_p) ((file_t*)file_p)
#define SD_DIR(dir_p)   ((rddir_t*)dir_p)

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);
static bool fs_ready(lv_fs_drv_t * drv);
static lv_fs_res_t fs_open (lv_fs_drv_t * drv, void * file_p, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close (lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read (lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek (lv_fs_drv_t * drv, void * file_p, uint32_t pos);
static lv_fs_res_t fs_size (lv_fs_drv_t * drv, void * file_p, uint32_t * size_p);
static lv_fs_res_t fs_tell (lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static lv_fs_res_t fs_remove (lv_fs_drv_t * drv, const char *path);
static lv_fs_res_t fs_trunc (lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_rename (lv_fs_drv_t * drv, const char * oldname, const char * newname);
static lv_fs_res_t fs_free (lv_fs_drv_t * drv, uint32_t * total_p, uint32_t * free_p);
static lv_fs_res_t fs_dir_open (lv_fs_drv_t * drv, void * dir_p, const char *path);
static lv_fs_res_t fs_dir_read (lv_fs_drv_t * drv, void * dir_p, char *fn);
static lv_fs_res_t fs_dir_close (lv_fs_drv_t * drv, void * dir_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_fs_drv_t fs_drv;                         /*A driver descriptor*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_if_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface  in LittlevGL
     *--------------------------------------------------*/

    /* Add a simple drive to open images */
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.file_size = sizeof(file_t);
    fs_drv.letter = SD_LETTER;
    fs_drv.ready_cb = fs_ready;
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;
    fs_drv.free_space_cb = fs_free;
    fs_drv.size_cb = fs_size;
    fs_drv.remove_cb = fs_remove;
    fs_drv.rename_cb = fs_rename;
    fs_drv.trunc_cb = fs_trunc;

    fs_drv.rddir_size = sizeof(rddir_t);
    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool fs_ready(lv_fs_drv_t * drv)
{
    return HAL::SD_GetReady();
}

/* Initialize your Storage device and File system. */
static void fs_init(void)
{
}

/**
 * Open a file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_open (lv_fs_drv_t * drv, void * file_p, const char * path, lv_fs_mode_t mode)
{
    oflag_t oflag = 0;

    if(mode == LV_FS_MODE_WR) oflag = O_WRONLY;
    else if(mode == LV_FS_MODE_RD) oflag = O_RDONLY;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) oflag = O_ACCMODE;

    /*构造文件对象*/
    file_t file;

    if(file.open(path, oflag))
    {
        file.seekSet(0);
        
        /*拷贝数据到lvgl申请的内存区域*/
        *SD_FILE(file_p) = file;

        return LV_FS_RES_OK;
    }
    else
    {
        return LV_FS_RES_UNKNOWN;
    }
}


/**
 * Close an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close (lv_fs_drv_t * drv, void * file_p)
{
    return SD_FILE(file_p)->close() ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

/**
 * Read data from an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */

static lv_fs_res_t fs_read (lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    int res = SD_FILE(file_p)->read(buf, btr);

    if(res < 0)
    {
        return LV_FS_RES_UNKNOWN;
    }

    *br = res;

    return LV_FS_RES_OK;
}

/**
 * Write into a file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btr Bytes To Write
 * @param br the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    int res = SD_FILE(file_p)->write((const uint8_t*)buf, btw);

    if(res < 0)
    {
        return LV_FS_RES_UNKNOWN;
    }

    *bw = res;

    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek (lv_fs_drv_t * drv, void * file_p, uint32_t pos)
{
    SD_FILE(file_p)->seekSet(pos);
    return LV_FS_RES_OK;
}

/**
 * Give the size of a file bytes
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param size pointer to a variable to store the size
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_size (lv_fs_drv_t * drv, void * file_p, uint32_t * size_p)
{
    *size_p = SD_FILE(file_p)->fileSize();
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell (lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    *pos_p = SD_FILE(file_p)->curPosition();
    return LV_FS_RES_OK;
}

/**
 * Delete a file
 * @param drv pointer to a driver where this function belongs
 * @param path path of the file to delete
 * @return  LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_remove (lv_fs_drv_t * drv, const char *path)
{
    return SD.remove(path) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

/**
 * Truncate the file size to the current position of the read write pointer
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_fs_open )
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_trunc (lv_fs_drv_t * drv, void * file_p)
{
    //SD_FILE(file_p).sync();
    //SD_FILE(file_p).truncate(); /*If not syncronized fclose can write the truncated part*/
    return LV_FS_RES_NOT_IMP;
}

/**
 * Rename a file
 * @param drv pointer to a driver where this function belongs
 * @param oldname path to the file
 * @param newname path with the new name
 * @return LV_FS_RES_OK or any error from 'fs_res_t'
 */
static lv_fs_res_t fs_rename (lv_fs_drv_t * drv, const char * oldname, const char * newname)
{
    return SD.rename(oldname, newname) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

/**
 * Get the free and total size of a driver in kB
 * @param drv pointer to a driver where this function belongs
 * @param letter the driver letter
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free size [kB]
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_free (lv_fs_drv_t * drv, uint32_t * total_p, uint32_t * free_p)
{
    return LV_FS_RES_NOT_IMP;
}

/**
 * Initialize a 'fs_read_dir_t' variable for directory reading
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to a 'fs_read_dir_t' variable
 * @param path path to a directory
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_open (lv_fs_drv_t * drv, void * dir_p, const char * path)
{
    rddir_t dir;
    *SD_DIR(dir_p) = dir;

    return SD_DIR(dir_p)->open(path) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'fs_read_dir_t' variable
 * @param fn pointer to a buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read (lv_fs_drv_t * drv, void * dir_p, char *fn)
{
    /*file_t file;
    char nameBuf[128];

    while (file.openNext(SD_DIR(dir_p), O_RDONLY))
    {
        file.getName(nameBuf, sizeof(nameBuf));

        if(file.isDir())
        {
            fn[0] = '/';
            strcpy(&fn[1], nameBuf);
        }
        else strcpy(fn, nameBuf);

        file.close();
    }

    if (SD_DIR(dir_p)->getError())
    {
        return LV_FS_RES_UNKNOWN;
    }*/

    return LV_FS_RES_NOT_IMP;
}

/**
 * Close the directory reading
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'fs_read_dir_t' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close (lv_fs_drv_t * drv, void * dir_p)
{
    return SD_DIR(dir_p)->close() ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

#endif  /*LV_USE_FILESYSTEM*/
