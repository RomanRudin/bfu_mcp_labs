#include <stdint.h>
#define __IO volatile

typedef struct { __IO uint32_t CR1, CR2, SR, DR; } SPI_TypeDef;
typedef struct { __IO uint32_t APB2ENR; } RCC_TypeDef;
typedef struct { __IO uint32_t CRL, CRH, IDR, ODR, BSRR; } GPIO_TypeDef;

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

void SPI1_Init(void) {
    RCC->APB2ENR |= (1 << 12) | (1 << 2);
    GPIOA->CRL &= ~((0xF << 20) | (0xF << 24) | (0xF << 28));
    GPIOA->CRL |= (0xB << 20) | (0x4 << 24) | (0xB << 28);
    SPI1->CR1 = (1 << 2) | (1 << 3) | (1 << 8) | (1 << 9) | (1 << 6);
    SPI1->CR2 = (1 << 2);
}

void SPI1_Write(uint8_t data) {
    while (!(SPI1->SR & (1 << 1)));
    SPI1->DR = data;
    while (SPI1->SR & (1 << 7)); 
}

uint8_t SPI1_Read(void) {
    SPI1_Write(0xFF);
    while (!(SPI1->SR & (1 << 0)));
    return SPI1->DR;
}