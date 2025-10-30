#include <stdint.h>
#define __IO volatile

typedef struct { __IO uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR; } GPIO_TypeDef;
typedef struct { __IO uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR, BDCR, CSR; } RCC_TypeDef;
typedef struct { __IO uint32_t CR1, CR2, SR, DR, CRCPR, RXCRCR, TXCRCR, I2SCFGR, I2SPR; } SPI_TypeDef;

#define PERIPH_BASE       0x40000000U
#define APB2PERIPH_BASE   (PERIPH_BASE + 0x10000U)
#define AHBPERIPH_BASE    (PERIPH_BASE + 0x20000U)

#define GPIOA_BASE        (APB2PERIPH_BASE + 0x0800U)
#define GPIOB_BASE        (APB2PERIPH_BASE + 0x0C00U)
#define GPIOC_BASE        (APB2PERIPH_BASE + 0x1000U)
#define RCC_BASE          (AHBPERIPH_BASE + 0x1000U)
#define SPI1_BASE         (APB2PERIPH_BASE + 0x3000U)

#define GPIOA             ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB             ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC             ((GPIO_TypeDef *)GPIOC_BASE)
#define RCC               ((RCC_TypeDef *)RCC_BASE)
#define SPI1              ((SPI_TypeDef *)SPI1_BASE)

#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 64

static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

#define SSD1306_CS_PIN   (1 << 0)
#define SSD1306_DC_PIN   (1 << 1)
#define SSD1306_RES_PIN  (1 << 10)

void SPI1_Init(void);
void SPI1_Write(uint8_t data);
uint8_t SPI1_Read(void);

static void delay(uint32_t ticks) {
    for (volatile uint32_t i = 0; i < ticks; i++);
}

static void SSD1306_WriteCommand(uint8_t cmd) {
    GPIOB->BRR = SSD1306_DC_PIN;
    GPIOB->BRR = SSD1306_CS_PIN;
    SPI1_Write(cmd);
    GPIOB->BSRR = SSD1306_CS_PIN;
}

static void SSD1306_WriteData(uint8_t data) {
    GPIOB->BSRR = SSD1306_DC_PIN;
    GPIOB->BRR = SSD1306_CS_PIN;
    SPI1_Write(data);
    GPIOB->BSRR = SSD1306_CS_PIN;
}

void SSD1306_Init(void) {
    RCC->APB2ENR |= (1 << 3); 
    GPIOB->CRL &= ~(0xF << 0);
    GPIOB->CRL |= (0x3 << 0); 
    GPIOB->CRL &= ~(0xF << 4);
    GPIOB->CRL |= (0x3 << 4);
    GPIOB->CRH &= ~(0xF << 8);
    GPIOB->CRH |= (0x3 << 8);
    
    SPI1_Init();
    GPIOB->BRR = SSD1306_RES_PIN;
    delay(10000);
    GPIOB->BSRR = SSD1306_RES_PIN;
    delay(10000);
    
    SSD1306_WriteCommand(0xAE);
    SSD1306_WriteCommand(0x20); 
    SSD1306_WriteCommand(0x00);
    SSD1306_WriteCommand(0x21); 
    SSD1306_WriteCommand(0x00); 
    SSD1306_WriteCommand(0x7F);
    SSD1306_WriteCommand(0x22);
    SSD1306_WriteCommand(0x00);
    SSD1306_WriteCommand(0x07);
    SSD1306_WriteCommand(0x40); 
    SSD1306_WriteCommand(0xA1);
    SSD1306_WriteCommand(0xC8);
    SSD1306_WriteCommand(0xDA);
    SSD1306_WriteCommand(0x12);
    SSD1306_WriteCommand(0x81);
    SSD1306_WriteCommand(0x7F);
    SSD1306_WriteCommand(0xA4);
    SSD1306_WriteCommand(0xA6);
    SSD1306_WriteCommand(0xD5);
    SSD1306_WriteCommand(0x80);
    SSD1306_WriteCommand(0x8D);
    SSD1306_WriteCommand(0x14);
    SSD1306_WriteCommand(0xAF);
    
    for (int i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = 0x00;
    }
    
    for (uint8_t page = 0; page < 8; page++) {
        SSD1306_WriteCommand(0xB0 + page);
        SSD1306_WriteCommand(0x00);
        SSD1306_WriteCommand(0x10);
        
        for (uint8_t col = 0; col < SSD1306_WIDTH; col++) {
            SSD1306_WriteData(SSD1306_Buffer[page * SSD1306_WIDTH + col]);
        }
    }
}

void SSD1306_Clear(void) {
    for (int i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = 0x00;
    }
}

void SSD1306_Update(void) {
    for (uint8_t page = 0; page < 8; page++) {
        SSD1306_WriteCommand(0xB0 + page);
        SSD1306_WriteCommand(0x00);
        SSD1306_WriteCommand(0x10);
        
        for (uint8_t col = 0; col < SSD1306_WIDTH; col++) {
            SSD1306_WriteData(SSD1306_Buffer[page * SSD1306_WIDTH + col]);
        }
    }
}

void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;
    
    uint16_t index = x + (y / 8) * SSD1306_WIDTH;
    
    if (color) {
        SSD1306_Buffer[index] |= (1 << (y % 8));
    } else {
        SSD1306_Buffer[index] &= ~(1 << (y % 8));
    }
}

void SSD1306_DrawChessBoard(void) {
    const uint8_t squareSize = 8;
    
    for (uint8_t y = 0; y < SSD1306_HEIGHT; y++) {
        for (uint8_t x = 0; x < SSD1306_WIDTH; x++) {
            uint8_t squareX = x / squareSize;
            uint8_t squareY = y / squareSize;
            
            if ((squareX + squareY) % 2 == 0) {
                SSD1306_DrawPixel(x, y, 1);
            } else {
                SSD1306_DrawPixel(x, y, 0);
            }
        }
    }
}