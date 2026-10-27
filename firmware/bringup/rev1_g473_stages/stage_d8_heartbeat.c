/* Rev 1 stage: D8/D11 chain heartbeat on PB2 only. */

#include <stdint.h>

#define REG32(address) (*(volatile uint32_t *)(address))

#define RCC_BASE 0x40021000UL
#define RCC_AHB2ENR REG32(RCC_BASE + 0x4CUL)
#define GPIOB_BASE 0x48000400UL

#define GPIO_MODER(base) REG32((base) + 0x00UL)
#define GPIO_OTYPER(base) REG32((base) + 0x04UL)
#define GPIO_BSRR(base) REG32((base) + 0x18UL)

#define GPIOB_EN_BIT (1UL << 1)
#define PB2_BIT (1UL << 2)

static void delay_cycles(volatile uint32_t cycles) {
    while (cycles-- > 0U) {
        __asm volatile("nop");
    }
}

static void d8_send_byte(uint8_t value) {
    for (uint8_t mask = 0x80U; mask != 0U; mask >>= 1U) {
        GPIO_BSRR(GPIOB_BASE) = PB2_BIT;
        if ((value & mask) != 0U) {
            delay_cycles(130U);
            GPIO_BSRR(GPIOB_BASE) = PB2_BIT << 16U;
            delay_cycles(40U);
        } else {
            delay_cycles(40U);
            GPIO_BSRR(GPIOB_BASE) = PB2_BIT << 16U;
            delay_cycles(130U);
        }
    }
}

static void d8_set(uint8_t red, uint8_t green, uint8_t blue) {
    d8_send_byte(green);
    d8_send_byte(red);
    d8_send_byte(blue);
    GPIO_BSRR(GPIOB_BASE) = PB2_BIT << 16U;
    delay_cycles(8000U);
}

static void pb2_init(void) {
    RCC_AHB2ENR |= GPIOB_EN_BIT;
    (void)RCC_AHB2ENR;

    GPIO_OTYPER(GPIOB_BASE) &= ~PB2_BIT;
    GPIO_MODER(GPIOB_BASE) = (GPIO_MODER(GPIOB_BASE) & ~(3UL << (2U * 2U))) | (1UL << (2U * 2U));
    GPIO_BSRR(GPIOB_BASE) = PB2_BIT << 16U;
}

int main(void) {
    pb2_init();

    for (;;) {
        d8_set(0x01U, 0x00U, 0x00U);
        delay_cycles(800000U);
        d8_set(0x00U, 0x00U, 0x00U);
        delay_cycles(3000000U);
    }
}

