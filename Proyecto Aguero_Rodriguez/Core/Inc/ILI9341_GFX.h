#ifndef ILI9341_GFX_H
#define ILI9341_GFX_H

#include "stm32f1xx_hal.h"
#include "fonts.h"

#define HORIZONTAL_IMAGE	0
#define VERTICAL_IMAGE		1

#define ORIGIN_X     160
#define ORIGIN_Y     240


void ILI9341_DrawHollowCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color);
void ILI9341_DrawFilledCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color);
void ILI9341_DrawHollowRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color);
void ILI9341_DrawFilledRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color);
void ILI9341_DrawChar(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor);
void ILI9341_DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor);
void ILI9341_DrawImage(const uint8_t* image, uint8_t orientation);
void ILI9341_Recta(int x1, int y1 , uint16_t color);
void ILI9341_DibujarBarridoAngulo(
//	int x0, int y0,
    int radius,              // Longitud de las rectas
    float startAngleDeg,     // Ángulo inicial (°)
    float endAngleDeg,       // Ángulo final (°)
    int steps,               // Cantidad de rectas
    uint16_t color);

#endif
