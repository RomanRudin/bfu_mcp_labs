#include <stdint.h>

void delay(uint32_t ticks) {
    for (volatile uint32_t i = 0; i < ticks; i++);
}

void SPI1_Init(void);
void SPI1_Write(uint8_t data);
uint8_t SPI1_Read(void);
void SSD1306_Init(void);
void SSD1306_DrawChessBoard(void);
void SSD1306_Update(void);

int main(void) {
    *(volatile uint32_t*)(0x40021000 + 0x18) |= (1 << 4);
    *(volatile uint32_t*)(0x40011000 + 0x04) |= (1 << 20);
    for(int i = 0; i < 3; i++) {
        *(volatile uint32_t*)(0x40011000 + 0x0C) |= (1 << 13);
        delay(100000);
        *(volatile uint32_t*)(0x40011000 + 0x0C) &= ~(1 << 13);
        delay(100000);
    }
    
    SPI1_Init();
    SSD1306_Init();
    SSD1306_DrawChessBoard();
    SSD1306_Update();
    
    while(1) {
        *(volatile uint32_t*)(0x40011000 + 0x0C) ^= (1 << 13);
        delay(500000);
    }
}