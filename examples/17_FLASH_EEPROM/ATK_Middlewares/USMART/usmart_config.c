#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* user configuration area
 * This should include the header file declared by the function used (user added)
 */

#include "../../BSP/LCD/lcd.h"
#include "../../BSP/STMFLASH/stmflash.h"

extern void led_set(uint8_t sta);
extern void test_fun(void(*ledset)(uint8_t), uint8_t sta);

/* Function name list initialization (user added)
 * Here the user enters directly the name of the function to be executed and its lookup string
 */
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* If read and write operations are enabled */
    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void *)write_addr, "void write_addr(uint32_t addr,uint32_t val)",
#endif
        
    (void *)lcd_clear, "void lcd_clear(uint16_t color)",
    (void *)lcd_fill, "void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)",
    (void *)lcd_draw_line, "void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)",
    (void *)lcd_draw_circle, "lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)",
    (void *)lcd_draw_rectangle, "void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)",
        
    (void *)lcd_show_num, "void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)",
    (void *)lcd_show_xnum, "void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)",
    (void *)lcd_show_string, "void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)",
    (void *)lcd_draw_point, "void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)",

	(void *)stmflash_read_halfword, "uint16_t stmflash_read_halfword(uint32_t faddr)",
	(void *)test_write, "void test_write(uint32_t waddr, uint16_t wdata)",
};

/******************************************************************************************/

/* The function controls the manager initialization
 * Get the names of each controlled function
 * Get the total number of functions
 */
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), /* Function quantity */
    0,      /* Arity */
    0,      /* Function ID */
    1,      /* Parameter display type,0, decimal; 1, hexadecimal */
    0,      /* Parameter type.bitx:0, digit; 1, string */
    0,      /* The length temporary table for each parameter requires MAX PARM initialization*/
    0,      /* The parameter of the function requires PARM LEN initialization */
};



















