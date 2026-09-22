#include "lcd_i2c.h"

/*
 * Handle del periférico I2C.
 */
static I2C_HandleTypeDef *lcd_i2c;


/* ============================================================
 * Enviar un byte al PCF8574
 * ============================================================ */

static void PCF8574_Write(uint8_t data)
{
    HAL_I2C_Master_Transmit(
        lcd_i2c,
        PCF8574_ADDRESS,
        &data,
        1,
        HAL_MAX_DELAY
    );
}


/* ============================================================
 * Generar pulso ENABLE
 * ============================================================ */

static void LCD_EnablePulse(uint8_t data)
{
    PCF8574_Write(data | LCD_EN);

    HAL_Delay(1);

    PCF8574_Write(data & ~LCD_EN);

    HAL_Delay(1);
}


/* ============================================================
 * Enviar 4 bits al LCD
 * ============================================================ */

static void LCD_Write4Bits(uint8_t nibble, uint8_t mode)
{
    /*
     * nibble contiene solamente los 4 bits menos significativos.
     *
     * Se desplazan hacia P4-P7.
     */

    uint8_t data = (nibble << 4);

    /*
     * RS:
     * 0 = comando
     * 1 = dato
     */

    if(mode)
    {
        data |= LCD_RS;
    }

    /*
     * RW siempre queda en 0:
     * escritura.
     */

    data &= ~LCD_RW;

    PCF8574_Write(data);

    LCD_EnablePulse(data);
}


/* ============================================================
 * Enviar byte completo
 * ============================================================ */

static void LCD_SendByte(uint8_t byte, uint8_t mode)
{
    uint8_t highNibble;
    uint8_t lowNibble;

    highNibble = (byte >> 4) & 0x0F;
    lowNibble  = byte & 0x0F;

    LCD_Write4Bits(highNibble, mode);

    LCD_Write4Bits(lowNibble, mode);
}


/* ============================================================
 * Enviar comando
 * ============================================================ */

void LCD_Command(uint8_t cmd)
{
    LCD_SendByte(cmd, 0);

    /*
     * CLEAR y HOME necesitan más tiempo.
     */

    if(cmd == LCD_CLEAR || cmd == LCD_HOME)
    {
        HAL_Delay(2);
    }
}


/* ============================================================
 * Inicialización
 * ============================================================ */

void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;

    /*
     * Tiempo necesario después de energizar el LCD.
     */
    HAL_Delay(50);

    /*
     * Secuencia estándar de inicialización HD44780.
     *
     * Inicialmente el LCD se comporta como interfaz de 8 bits.
     * Se envía 0x03 tres veces.
     */

    LCD_Write4Bits(0x03, 0);

    HAL_Delay(5);

    LCD_Write4Bits(0x03, 0);

    HAL_Delay(1);

    LCD_Write4Bits(0x03, 0);

    HAL_Delay(1);

    /*
     * Cambiar definitivamente a modo de 4 bits.
     */

    LCD_Write4Bits(0x02, 0);

    HAL_Delay(1);


    /*
     * 4 bits
     * 2 líneas internas
     * caracteres 5x8
     */

    LCD_Command(LCD_FUNCTION_SET);


    /*
     * Display ON
     * Cursor OFF
     * Blink OFF
     */

    LCD_Command(LCD_DISPLAY_ON);


    /*
     * Incrementar dirección automáticamente.
     */

    LCD_Command(LCD_ENTRY_MODE);


    /*
     * Limpiar pantalla.
     */

    LCD_Clear();

    HAL_Delay(2);
}


/* ============================================================
 * Limpiar LCD
 * ============================================================ */

void LCD_Clear(void)
{
    LCD_Command(LCD_CLEAR);

    HAL_Delay(2);
}


/* ============================================================
 * Cursor a HOME
 * ============================================================ */

void LCD_Home(void)
{
    LCD_Command(LCD_HOME);

    HAL_Delay(2);
}


/* ============================================================
 * Posicionar cursor
 *
 * Para LCD 20x4
 *
 * Línea 1 = 0x00
 * Línea 2 = 0x40
 * Línea 3 = 0x14
 * Línea 4 = 0x54
 * ============================================================ */

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t row_offsets[] =
    {
        0x00,
        0x40,
        0x14,
        0x54
    };

    if(row > 3)
    {
        row = 3;
    }

    if(col > 19)
    {
        col = 19;
    }

    LCD_Command(
        LCD_SET_DDRAM |
        (row_offsets[row] + col)
    );
}


/* ============================================================
 * Escribir carácter normal
 * ============================================================ */

void LCD_PrintChar(char character)
{
    LCD_SendByte((uint8_t)character, 1);
}


/* ============================================================
 * Escribir texto
 * ============================================================ */

void LCD_Print(char *text)
{
    while(*text)
    {
        LCD_PrintChar(*text);

        text++;
    }
}


/* ============================================================
 * Crear carácter personalizado
 *
 * location = 0 - 7
 * ============================================================ */

void LCD_CreateChar(uint8_t location, uint8_t charmap[])
{
    location &= 0x07;

    LCD_Command(
        LCD_SET_CGRAM |
        (location << 3)
    );

    for(uint8_t i = 0; i < 8; i++)
    {
        LCD_SendByte(charmap[i], 1);
    }
}


/* ============================================================
 * Mostrar carácter personalizado
 * ============================================================ */

void LCD_WriteCustomChar(uint8_t location)
{
    LCD_SendByte(location & 0x07, 1);
}