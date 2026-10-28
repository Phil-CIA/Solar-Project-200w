/* Rev 1 stage: USART1 boot log on PA9/PA10 (COM5 through CP2102). */

#include <stdint.h>

#define REG32(address) (*(volatile uint32_t *)(address))

#define RCC_BASE 0x40021000UL
#define RCC_AHB2ENR REG32(RCC_BASE + 0x4CUL)
#define RCC_APB2ENR REG32(RCC_BASE + 0x60UL)

#define GPIOA_BASE 0x48000000UL
#define GPIO_MODER(base) REG32((base) + 0x00UL)
#define GPIO_AFRH(base) REG32((base) + 0x24UL)

#define USART1_BASE 0x40013800UL
#define USART_CR1 REG32(USART1_BASE + 0x00UL)
#define USART_BRR REG32(USART1_BASE + 0x0CUL)
#define USART_ISR REG32(USART1_BASE + 0x1CUL)
#define USART_TDR REG32(USART1_BASE + 0x28UL)

#define GPIOA_EN_BIT (1UL << 0)
#define USART1_EN_BIT (1UL << 14)

static void delay_cycles(volatile uint32_t cycles) {
    while (cycles-- > 0U) {
        __asm volatile("nop");
    }
}

static void uart_gpio_init(void) {
    RCC_AHB2ENR |= GPIOA_EN_BIT;
    (void)RCC_AHB2ENR;

    GPIO_MODER(GPIOA_BASE) = (GPIO_MODER(GPIOA_BASE) & ~((3UL << (9U * 2U)) | (3UL << (10U * 2U)))) |
                             (2UL << (9U * 2U)) |
                             (2UL << (10U * 2U));

    GPIO_AFRH(GPIOA_BASE) = (GPIO_AFRH(GPIOA_BASE) & ~((0xFUL << 4U) | (0xFUL << 8U))) |
                            (7UL << 4U) |
                            (7UL << 8U);
}

static void uart_init(void) {
    RCC_APB2ENR |= USART1_EN_BIT;
    (void)RCC_APB2ENR;

    USART_CR1 = 0U;
    USART_BRR = 139U; /* 16 MHz / 115200 ~= 138.9 */
    USART_CR1 = (1UL << 0) | (1UL << 2) | (1UL << 3);
}

static void uart_putc(char value) {
    while ((USART_ISR & (1UL << 7)) == 0U) {
    }
    USART_TDR = (uint32_t)(uint8_t)value;
}

static void uart_write(const char *text) {
    while (*text != '\0') {
        uart_putc(*text++);
    }
}

static void uart_line(const char *text) {
    uart_write(text);
    uart_write("\r\n");
}

int main(void) {
    uart_gpio_init();
    uart_init();

    uart_line("");
    uart_line("Solar Rev1 bootlog stage");
    uart_line("USART1 115200 8N1 on PA9/PA10");

    uint32_t count = 0U;
    for (;;) {
        uart_write("bootlog tick ");
        uart_putc((char)('0' + (count / 10U) % 10U));
        uart_putc((char)('0' + (count % 10U)));
        uart_write("\r\n");
        count = (count + 1U) % 100U;
        delay_cycles(1600000U);
    }
}

