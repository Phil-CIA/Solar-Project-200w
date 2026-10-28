/* Rev 1 stage: bounded software I2C address probe on PB6/PB7 only. */

#include <stdbool.h>
#include <stdint.h>

#define REG32(address) (*(volatile uint32_t *)(address))

#define RCC_BASE 0x40021000UL
#define RCC_AHB2ENR REG32(RCC_BASE + 0x4CUL)

#define GPIOB_BASE 0x48000400UL
#define GPIO_MODER(base) REG32((base) + 0x00UL)
#define GPIO_OTYPER(base) REG32((base) + 0x04UL)
#define GPIO_PUPDR(base) REG32((base) + 0x0CUL)
#define GPIO_IDR(base) REG32((base) + 0x10UL)
#define GPIO_BSRR(base) REG32((base) + 0x18UL)

#define GPIOB_EN_BIT (1UL << 1)
#define PB6_BIT (1UL << 6)
#define PB7_BIT (1UL << 7)
#define I2C_MASK (PB6_BIT | PB7_BIT)

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t sweep_count;
    uint8_t first_address;
    uint8_t last_address;
    uint8_t found_count;
    uint8_t finished;
    uint8_t ack_bitmap[14];
} rev1_scan_result_t;

volatile rev1_scan_result_t g_rev1_scan_result = {
    .magic = 0x52563153UL, /* RV1S */
    .version = 1U,
    .sweep_count = 0U,
    .first_address = 0x08U,
    .last_address = 0x77U,
    .found_count = 0U,
    .finished = 0U,
    .ack_bitmap = {0}
};

static void delay_cycles(volatile uint32_t cycles) {
    while (cycles-- > 0U) {
        __asm volatile("nop");
    }
}

static void i2c_sda_release(void) {
    GPIO_BSRR(GPIOB_BASE) = PB6_BIT;
}

static void i2c_sda_low(void) {
    GPIO_BSRR(GPIOB_BASE) = PB6_BIT << 16U;
}

static void i2c_scl_release(void) {
    GPIO_BSRR(GPIOB_BASE) = PB7_BIT;
}

static void i2c_scl_low(void) {
    GPIO_BSRR(GPIOB_BASE) = PB7_BIT << 16U;
}

static bool i2c_sda_read(void) {
    return (GPIO_IDR(GPIOB_BASE) & PB6_BIT) != 0U;
}

static void i2c_half_period(void) {
    delay_cycles(70U);
}

static void i2c_start(void) {
    i2c_sda_release();
    i2c_scl_release();
    i2c_half_period();
    i2c_sda_low();
    i2c_half_period();
    i2c_scl_low();
}

static void i2c_stop(void) {
    i2c_sda_low();
    i2c_half_period();
    i2c_scl_release();
    i2c_half_period();
    i2c_sda_release();
    i2c_half_period();
}

static bool i2c_write_byte(uint8_t value) {
    for (uint8_t bit = 0U; bit < 8U; ++bit) {
        if ((value & 0x80U) != 0U) {
            i2c_sda_release();
        } else {
            i2c_sda_low();
        }
        i2c_half_period();
        i2c_scl_release();
        i2c_half_period();
        i2c_scl_low();
        value <<= 1U;
    }

    i2c_sda_release();
    i2c_half_period();
    i2c_scl_release();
    i2c_half_period();
    const bool ack = !i2c_sda_read();
    i2c_scl_low();
    i2c_half_period();
    return ack;
}

static void mark_acked(uint8_t address) {
    const uint8_t offset = (uint8_t)(address - g_rev1_scan_result.first_address);
    g_rev1_scan_result.ack_bitmap[offset >> 3U] |= (uint8_t)(1U << (offset & 7U));
}

static void run_bounded_scan(void) {
    g_rev1_scan_result.sweep_count++;
    g_rev1_scan_result.found_count = 0U;
    g_rev1_scan_result.finished = 0U;
    for (uint8_t i = 0U; i < sizeof(g_rev1_scan_result.ack_bitmap); ++i) {
        g_rev1_scan_result.ack_bitmap[i] = 0U;
    }

    for (uint8_t address = g_rev1_scan_result.first_address; address <= g_rev1_scan_result.last_address; ++address) {
        i2c_start();
        const bool ack = i2c_write_byte((uint8_t)(address << 1U));
        i2c_stop();
        if (ack) {
            mark_acked(address);
            g_rev1_scan_result.found_count++;
        }
        delay_cycles(350U);
    }

    g_rev1_scan_result.finished = 1U;
}

static void pb6_pb7_i2c_gpio_init(void) {
    RCC_AHB2ENR |= GPIOB_EN_BIT;
    (void)RCC_AHB2ENR;

    GPIO_OTYPER(GPIOB_BASE) |= I2C_MASK;
    GPIO_PUPDR(GPIOB_BASE) &= ~((3UL << (6U * 2U)) | (3UL << (7U * 2U)));
    GPIO_MODER(GPIOB_BASE) = (GPIO_MODER(GPIOB_BASE) & ~((3UL << (6U * 2U)) | (3UL << (7U * 2U)))) |
                             (1UL << (6U * 2U)) |
                             (1UL << (7U * 2U));
    GPIO_BSRR(GPIOB_BASE) = I2C_MASK;
}

int main(void) {
    pb6_pb7_i2c_gpio_init();
    run_bounded_scan();

    for (;;) {
        __asm volatile("wfi");
    }
}

