/**
  * @file GC9A01.c
  *
 **/


/*********************
 *      INCLUDES
 *********************/
#include "GC9A01.h"
#if USE_GC9A01

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include LV_DRV_DISP_INCLUDE
#include LV_DRV_DELAY_INCLUDE

/*********************
 *      DEFINES
 *********************/
#define GC9A01_BAUD      		2000000    /*< 2,5 MHz (400 ns)*/

#define GC9A01_CMD_MODE     0
#define GC9A01_DATA_MODE    1

#define GC9A01_HOR_RES  		240
#define GC9A01_VER_RES  		240

/* GC9A01 Commands that we know of.  Limited documentation */
#define GC9A01_INVOFF	0x20
#define GC9A01_INVON	0x21
#define GC9A01_DISPON	0x29
#define GC9A01_CASET	0x2A
#define GC9A01_RASET	0x2B
#define GC9A01_RAMWR	0x2C
#define GC9A01_COLMOD	0x3A
#define GC9A01_MADCTL	0x36
#define GC9A01_MADCTL_MY  0x80
#define GC9A01_MADCTL_MX  0x40
#define GC9A01_MADCTL_MV  0x20
#define GC9A01_MADCTL_RGB 0x00
#define GC9A01_DISFNCTRL	0xB6

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void GC9A01_command(uint8_t cmd);
static void GC9A01_data(uint8_t data);
static void GC9A01_set_addr_win(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/**********************
 *  STATIC VARIABLES
 **********************/
// Documentation on op codes for GC9A01 are very hard to find.
// Will document should they be found.
static struct GC9A01_function GC9A01_cfg_script[] = {
	{ GC9A01_START, GC9A01_START},
	{ GC9A01_CMD, 0xEF},

	{ GC9A01_CMD, 0xEB},
	{ GC9A01_DATA, 0x14},

	{ GC9A01_CMD, 0xFE}, // Inter Register Enable1
	{ GC9A01_CMD, 0xEF}, // Inter Register Enable2

	{ GC9A01_CMD, 0xEB},
	{ GC9A01_DATA, 0x14},

	{ GC9A01_CMD, 0x84},
	{ GC9A01_DATA, 0x40},

	{ GC9A01_CMD, 0x85},
	{ GC9A01_DATA, 0xFF},

	{ GC9A01_CMD, 0x86},
	{ GC9A01_DATA, 0xFF},

	{ GC9A01_CMD, 0x87},
	{ GC9A01_DATA, 0xFF},

	{ GC9A01_CMD, 0x88},
	{ GC9A01_DATA, 0x0A},

	{ GC9A01_CMD, 0x89},
	{ GC9A01_DATA, 0x21},

	{ GC9A01_CMD, 0x8A},
	{ GC9A01_DATA, 0x00},

	{ GC9A01_CMD, 0x8B},
	{ GC9A01_DATA, 0x80},

	{ GC9A01_CMD, 0x8C},
	{ GC9A01_DATA, 0x01},

	{ GC9A01_CMD, 0x8D},
	{ GC9A01_DATA, 0x01},

	{ GC9A01_CMD, 0x8E},
	{ GC9A01_DATA, 0xFF},

	{ GC9A01_CMD, 0x8F},
	{ GC9A01_DATA, 0xFF},

	{ GC9A01_CMD, GC9A01_DISFNCTRL}, // Display Function Control
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x00},

	{ GC9A01_CMD, GC9A01_MADCTL}, // Memory Access Control
	{ GC9A01_DATA, 0x48}, // Set the display direction 0,1,2,3	four directions

	{ GC9A01_CMD, GC9A01_COLMOD}, // COLMOD: Pixel Format Set
	{ GC9A01_DATA, 0x05}, // 16 Bits per pixel

	{ GC9A01_CMD, 0x90},
	{ GC9A01_DATA, 0x08},
	{ GC9A01_DATA, 0x08},
	{ GC9A01_DATA, 0x08},
	{ GC9A01_DATA, 0x08},

	{ GC9A01_CMD, 0xBD},
	{ GC9A01_DATA, 0x06},

	{ GC9A01_CMD, 0xBC},
	{ GC9A01_DATA, 0x00},

	{ GC9A01_CMD, 0xFF},
	{ GC9A01_DATA, 0x60},
	{ GC9A01_DATA, 0x01},
	{ GC9A01_DATA, 0x04},

	{ GC9A01_CMD, 0xC3}, // Power Control 2
	{ GC9A01_DATA, 0x13},
	{ GC9A01_CMD, 0xC4}, // Power Control 3
	{ GC9A01_DATA, 0x13},

	{ GC9A01_CMD, 0xC9}, // Power Control 4
	{ GC9A01_DATA, 0x22},

	{ GC9A01_CMD, 0xBE}, 
	{ GC9A01_DATA, 0x11},

	{ GC9A01_CMD, 0xE1}, 
	{ GC9A01_DATA, 0x10},
	{ GC9A01_DATA, 0x0E},

	{ GC9A01_CMD, 0xDF},
	{ GC9A01_DATA, 0x21},
	{ GC9A01_DATA, 0x0C},
	{ GC9A01_DATA, 0x02},

	{ GC9A01_CMD, 0xF0}, // SET_GAMMA1
	{ GC9A01_DATA, 0x45},
	{ GC9A01_DATA, 0x09},
	{ GC9A01_DATA, 0x08},
	{ GC9A01_DATA, 0x08},
	{ GC9A01_DATA, 0x26},
	{ GC9A01_DATA, 0x2A},

	{ GC9A01_CMD, 0xF1}, // SET_GAMMA2
	{ GC9A01_DATA, 0x43},
	{ GC9A01_DATA, 0x70},
	{ GC9A01_DATA, 0x72},
	{ GC9A01_DATA, 0x36},
	{ GC9A01_DATA, 0x37},
	{ GC9A01_DATA, 0x6F},

	{ GC9A01_CMD, 0xF2}, // SET_GAMMA3
	{ GC9A01_DATA, 0x45},
	{ GC9A01_DATA, 0x09},
	{ GC9A01_DATA, 0x08},
	{ GC9A01_DATA, 0x08},
	{ GC9A01_DATA, 0x26},
	{ GC9A01_DATA, 0x2A},

	{ GC9A01_CMD, 0xF3}, // SET_GAMMA4
	{ GC9A01_DATA, 0x43},
	{ GC9A01_DATA, 0x70},
	{ GC9A01_DATA, 0x72},
	{ GC9A01_DATA, 0x36},
	{ GC9A01_DATA, 0x37},
	{ GC9A01_DATA, 0x6F},

	{ GC9A01_CMD, 0xED},
	{ GC9A01_DATA, 0x1B},
	{ GC9A01_DATA, 0x0B},

	{ GC9A01_CMD, 0xAE},
	{ GC9A01_DATA, 0x77},

	{ GC9A01_CMD, 0xCD},
	{ GC9A01_DATA, 0x63},

	{ GC9A01_CMD, 0x70},
	{ GC9A01_DATA, 0x07},
	{ GC9A01_DATA, 0x07},
	{ GC9A01_DATA, 0x04},
	{ GC9A01_DATA, 0x0E},
	{ GC9A01_DATA, 0x0F},
	{ GC9A01_DATA, 0x09},
	{ GC9A01_DATA, 0x07},
	{ GC9A01_DATA, 0x08},
	{ GC9A01_DATA, 0x03},

	{ GC9A01_CMD, 0xE8},
	{ GC9A01_DATA, 0x34},

	{ GC9A01_CMD, 0x62},
	{ GC9A01_DATA, 0x18},
	{ GC9A01_DATA, 0x0D},
	{ GC9A01_DATA, 0x71},
	{ GC9A01_DATA, 0xED},
	{ GC9A01_DATA, 0x70},
	{ GC9A01_DATA, 0x70},
	{ GC9A01_DATA, 0x18},
	{ GC9A01_DATA, 0x0F},
	{ GC9A01_DATA, 0x71},
	{ GC9A01_DATA, 0xEF},
	{ GC9A01_DATA, 0x70},
	{ GC9A01_DATA, 0x70},

	{ GC9A01_CMD, 0x63},
	{ GC9A01_DATA, 0x18},
	{ GC9A01_DATA, 0x11},
	{ GC9A01_DATA, 0x71},
	{ GC9A01_DATA, 0xF1},
	{ GC9A01_DATA, 0x70},
	{ GC9A01_DATA, 0x70},
	{ GC9A01_DATA, 0x18},
	{ GC9A01_DATA, 0x13},
	{ GC9A01_DATA, 0x71},
	{ GC9A01_DATA, 0xF3},
	{ GC9A01_DATA, 0x70},
	{ GC9A01_DATA, 0x70},

	{ GC9A01_CMD, 0x64},
	{ GC9A01_DATA, 0x28},
	{ GC9A01_DATA, 0x29},
	{ GC9A01_DATA, 0xF1},
	{ GC9A01_DATA, 0x01},
	{ GC9A01_DATA, 0xF1},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x07},

	{ GC9A01_CMD, 0x66},
	{ GC9A01_DATA, 0x3C},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0xCD},
	{ GC9A01_DATA, 0x67},
	{ GC9A01_DATA, 0x45},
	{ GC9A01_DATA, 0x45},
	{ GC9A01_DATA, 0x10},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x00},

	{ GC9A01_CMD, 0x67},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x3C},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x01},
	{ GC9A01_DATA, 0x54},
	{ GC9A01_DATA, 0x10},
	{ GC9A01_DATA, 0x32},
	{ GC9A01_DATA, 0x98},

	{ GC9A01_CMD, 0x74},
	{ GC9A01_DATA, 0x10},
	{ GC9A01_DATA, 0x85},
	{ GC9A01_DATA, 0x80},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x00},
	{ GC9A01_DATA, 0x4E},
	{ GC9A01_DATA, 0x00},

	{ GC9A01_CMD, 0x98},
	{ GC9A01_DATA, 0x3E},
	{ GC9A01_DATA, 0x07},

	{ GC9A01_CMD, 0x35}, // Tearing Effect Line ON
	{ GC9A01_CMD, 0x21}, // Display Inversion ON

	{ GC9A01_CMD, 0x11}, // Sleep Out Mode
	{ GC9A01_DELAY, 120},
	{ GC9A01_CMD, GC9A01_DISPON}, // Display ON
	{ GC9A01_DELAY, 255},
	{ GC9A01_END, GC9A01_END},
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Write a command to the GC9A01
 * @param cmd the command
 */
static void GC9A01_command(uint8_t cmd)
{
	LV_DRV_DISP_CMD_DATA(GC9A01_CMD_MODE);
	LV_DRV_DISP_SPI_WR_BYTE(cmd);
}

/**
 * Write data to the GC9A01
 * @param data the data
 */
static void GC9A01_data(uint8_t data)
{
	LV_DRV_DISP_CMD_DATA(GC9A01_DATA_MODE);
	LV_DRV_DISP_SPI_WR_BYTE(data);
}

static int GC9A01_data_array(uint8_t *buf, uint32_t len)
{
	uint8_t *pt = buf;

	for (uint32_t lp = 0; lp < len; lp++, pt++)
	{
		LV_DRV_DISP_SPI_WR_BYTE(*pt);
	} 
	return 0;
}

static int GC9A01_databuf(uint32_t len, uint8_t *buf)
{
	uint32_t byte_left = len;
	uint8_t *pt = buf;

	while (byte_left)
	{
		if (byte_left > 64)
		{
			LV_DRV_DISP_SPI_WR_ARRAY((char*)pt, 64);
			byte_left = byte_left - 64;
			pt = pt + 64;
		}
		else
		{
			LV_DRV_DISP_SPI_WR_ARRAY((char*)pt, byte_left);
			byte_left=0;
		}
	}

	return 0;
}

// hard reset of the tft controller
// ----------------------------------------------------------
static void GC9A01_hard_reset( void )
{
    LV_DRV_DISP_SPI_CS(0); // Low to listen to us

    LV_DRV_DISP_RST(1);
    LV_DRV_DELAY_MS(50);
    LV_DRV_DISP_RST(0);
    LV_DRV_DELAY_MS(50);
    LV_DRV_DISP_RST(1);
    LV_DRV_DELAY_MS(50);
}

// Configuration of the tft controller
// ----------------------------------------------------------
static void GC9A01_run_cfg_script(void)
{
	int i = 0;
	int end_script = 0;

	do {
		switch (GC9A01_cfg_script[i].cmd)
		{
			case GC9A01_START:
				break;
			case GC9A01_CMD:
				GC9A01_command( GC9A01_cfg_script[i].data & 0xFF );
				break;
			case GC9A01_DATA:
				GC9A01_data( GC9A01_cfg_script[i].data & 0xFF );
				break;
			case GC9A01_DELAY:
        LV_DRV_DELAY_MS(GC9A01_cfg_script[i].data);
				break;
			case GC9A01_END:
				end_script = 1;
		}
		i++;
	} while (!end_script);
}

void GC9A01_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  // Rudimentary clipping
  if((x >= GC9A01_HOR_RES) || (y >= GC9A01_VER_RES)) return;
  if((y+h-1) >= GC9A01_VER_RES) h = GC9A01_VER_RES - y;

	LV_DRV_DISP_SPI_CS(0); // Listen to us

  GC9A01_set_addr_win(x, y, x, y + h - 1);

  uint8_t hi = color >> 8, lo = color;
    
  while (h--) {
    GC9A01_data(hi);
    GC9A01_data(lo);
  }

	LV_DRV_DISP_SPI_CS(1);
}

void GC9A01_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Rudimentary clipping
  if((x >= GC9A01_HOR_RES) || (y >= GC9A01_VER_RES)) return;
  if((x+w-1) >= GC9A01_HOR_RES)  w = GC9A01_HOR_RES - x;

	LV_DRV_DISP_SPI_CS(0); // Listen to us

  GC9A01_set_addr_win(x, y, x + w - 1, y);

  uint8_t hi = color >> 8, lo = color;

  while (w--) {
    GC9A01_data(hi);
    GC9A01_data(lo);
  }

	LV_DRV_DISP_SPI_CS(1);
}


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t GC9A01_Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void GC9A01_invertDisplay(bool i)
{
  GC9A01_command(i ? GC9A01_INVON : GC9A01_INVOFF);
}

void GC9A01_drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if((x < 0) ||(x >= GC9A01_HOR_RES) || (y < 0) || (y >= GC9A01_VER_RES)) return;

	LV_DRV_DISP_SPI_CS(0); // Listen to us
  GC9A01_set_addr_win(x, y, x, y);

  uint8_t hi = color >> 8, lo = color;

	GC9A01_data(hi);
	GC9A01_data(lo);

	LV_DRV_DISP_SPI_CS(1);
}

void GC9A01_fillScreen(uint16_t color) {
	GC9A01_fillRect(0, 0,  GC9A01_HOR_RES, GC9A01_VER_RES, color);
}

// fill a rectangle
void GC9A01_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= GC9A01_HOR_RES) || (y >= GC9A01_VER_RES)) return;
  if((x + w - 1) >= GC9A01_HOR_RES)  w = GC9A01_HOR_RES  - x;
  if((y + h - 1) >= GC9A01_VER_RES) h = GC9A01_VER_RES - y;

  LV_DRV_DISP_SPI_CS(0);  // Listen to us

  GC9A01_set_addr_win(x, y, x + w - 1, y + h - 1);

	uint8_t hi = color >> 8, lo = color;

  for (y = h; y > 0; y--)
  {
    for (x = w; x > 0; x--)
    {
      GC9A01_data(hi);
      GC9A01_data(lo);
    }
  }
  LV_DRV_DISP_SPI_CS(1);
}

void GC9A01_setRotation(uint8_t m) {

  GC9A01_command(GC9A01_MADCTL);
  m %= 4; // can't be higher than 3
  switch (m) {
   case 0:
     GC9A01_data(GC9A01_MADCTL_MX | GC9A01_MADCTL_MY | GC9A01_MADCTL_RGB);

    //  _xstart = _colstart;
    //  _ystart = _rowstart;
     break;
   case 1:
     GC9A01_data(GC9A01_MADCTL_MY | GC9A01_MADCTL_MV | GC9A01_MADCTL_RGB);

    //  _ystart = _colstart;
    //  _xstart = _rowstart;
     break;
  case 2:
     GC9A01_data(GC9A01_MADCTL_RGB);
 
    //  _xstart = _colstart;
    //  _ystart = _rowstart;
     break;

   case 3:
     GC9A01_data(GC9A01_MADCTL_MX | GC9A01_MADCTL_MV | GC9A01_MADCTL_RGB);

    //  _ystart = _colstart;
    //  _xstart = _rowstart;
     break;
  }
}

/**
 * Initialize the GC9A01
 */
int GC9A01_init(void)
{
  LV_DRV_DISP_SPI_FREQ(GC9A01_SPI_BAUD);
  LV_DRV_DISP_SPI_MODE(GC9A01_SPI_BITS, GC9A01_SPI_MODE);

	GC9A01_hard_reset();
	GC9A01_run_cfg_script();

	// GC9A01_fillScreen(0x0000); // Black
	// GC9A01_fillScreen(0xFFFF); // White
	GC9A01_fillScreen(0xAAAA); // ?

	return 0;
}

static void GC9A01_set_addr_win(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  uint16_t x_start = x0 + GC9A01_XSTART, x_end = x1 + GC9A01_XSTART;
  uint16_t y_start = y0 + GC9A01_YSTART, y_end = y1 + GC9A01_YSTART;

  GC9A01_command(GC9A01_CASET); // Column addr set
  GC9A01_data(x_start >> 8);
  GC9A01_data(x_start & 0xFF);     // XSTART 
  GC9A01_data(x_end >> 8);
  GC9A01_data(x_end & 0xFF);     // XEND

  GC9A01_command(GC9A01_RASET); // Row addr set
  GC9A01_data(y_start >> 8);
  GC9A01_data(y_start & 0xFF);     // YSTART
  GC9A01_data(y_end >> 8);
  GC9A01_data(y_end & 0xFF);     // YEND

  GC9A01_command(GC9A01_RAMWR);
}

void GC9A01_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t *color_p)
{
  LV_DRV_DISP_SPI_CS(0); // Listen to us

  GC9A01_set_addr_win(area->x1, area->y1, area->x2, area->y2);
  int32_t len = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;

	LV_DRV_DISP_CMD_DATA(GC9A01_DATA_MODE);
  LV_DRV_DISP_SPI_WR_ARRAY((char*)color_p, len);

  LV_DRV_DISP_SPI_CS(1);
  lv_disp_flush_ready(disp_drv);         /* Indicate you are ready with the flushing*/
}

#endif