/* Rev 1 stage: PB6/PB7 software I2C + USART1 + PB2 D8/D11 railroad heartbeat. */

#include <stdbool.h>
#include <stdint.h>

#define REG32(address) (*(volatile uint32_t *)(address))

#define RCC_BASE 0x40021000UL
#define RCC_AHB2ENR REG32(RCC_BASE + 0x4CUL)
#define RCC_APB2ENR REG32(RCC_BASE + 0x60UL)

#define GPIOA_BASE 0x48000000UL
#define GPIOB_BASE 0x48000400UL
#define GPIOC_BASE 0x48000800UL
#define GPIO_MODER(base) REG32((base) + 0x00UL)
#define GPIO_OTYPER(base) REG32((base) + 0x04UL)
#define GPIO_PUPDR(base) REG32((base) + 0x0CUL)
#define GPIO_IDR(base) REG32((base) + 0x10UL)
#define GPIO_AFRH(base) REG32((base) + 0x24UL)
#define GPIO_BSRR(base) REG32((base) + 0x18UL)

#define USART1_BASE 0x40013800UL
#define USART_CR1 REG32(USART1_BASE + 0x00UL)
#define USART_BRR REG32(USART1_BASE + 0x0CUL)
#define USART_ISR REG32(USART1_BASE + 0x1CUL)
#define USART_TDR REG32(USART1_BASE + 0x28UL)

#define ADC1_BASE 0x50000000UL
#define ADC12_COMMON_BASE 0x50000300UL
#define ADC_ISR REG32(ADC1_BASE + 0x00UL)
#define ADC_CR REG32(ADC1_BASE + 0x08UL)
#define ADC_CFGR REG32(ADC1_BASE + 0x0CUL)
#define ADC_SMPR1 REG32(ADC1_BASE + 0x14UL)
#define ADC_SQR1 REG32(ADC1_BASE + 0x30UL)
#define ADC_DR REG32(ADC1_BASE + 0x40UL)
#define ADC12_CCR REG32(ADC12_COMMON_BASE + 0x08UL)

#define GPIOA_EN_BIT (1UL << 0)
#define GPIOB_EN_BIT (1UL << 1)
#define GPIOC_EN_BIT (1UL << 2)
#define USART1_EN_BIT (1UL << 14)
#define ADC12_EN_BIT (1UL << 13)

#define PB6_BIT (1UL << 6) /* SDA */
#define PB7_BIT (1UL << 7) /* SCL */
#define I2C_MASK (PB6_BIT | PB7_BIT)
#define PB2_BIT (1UL << 2) /* WS2812 data for D8->D11 chain */
#define PB2_SET PB2_BIT
#define PB2_CLR (PB2_BIT << 16U)
#define PC13_BIT (1UL << 13) /* U6_FLT (open-drain, pulled up by R34) */

#define AHT20_ADDR 0x38U
#define OLED_ADDR 0x3CU
#define U6_ADDR 0x6AU
#define WS_RESET_CYCLES (300U * 16U) /* 300 us at 16 MHz */
#define PA5_BIT (1UL << 5) /* V_in_ON */

#define VIN_ADC_SHUTDOWN_MV 50000U
#define VIN_ADC_SCALE_NUM 190U
#define VIN_ADC_SCALE_DEN 10U

static void delay_cycles(volatile uint32_t cycles) {
    while (cycles-- > 0U) {
        __asm volatile("nop");
    }
}

static void delay_ms(uint32_t ms) {
    while (ms-- > 0U) {
        delay_cycles(16000U);
    }
}

static bool wait_reg32_set(volatile uint32_t *reg, uint32_t mask, uint32_t timeout_cycles) {
    while (((*reg) & mask) == 0U) {
        if (timeout_cycles-- == 0U) {
            return false;
        }
    }
    return true;
}

static bool wait_reg32_clear(volatile uint32_t *reg, uint32_t mask, uint32_t timeout_cycles) {
    while (((*reg) & mask) != 0U) {
        if (timeout_cycles-- == 0U) {
            return false;
        }
    }
    return true;
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

static void i2c_gpio_init(void) {
    RCC_AHB2ENR |= GPIOB_EN_BIT;
    (void)RCC_AHB2ENR;

    GPIO_OTYPER(GPIOB_BASE) |= I2C_MASK;
    GPIO_PUPDR(GPIOB_BASE) &= ~((3UL << (6U * 2U)) | (3UL << (7U * 2U)));
    GPIO_MODER(GPIOB_BASE) = (GPIO_MODER(GPIOB_BASE) & ~((3UL << (6U * 2U)) | (3UL << (7U * 2U)))) |
                             (1UL << (6U * 2U)) |
                             (1UL << (7U * 2U));
    GPIO_BSRR(GPIOB_BASE) = I2C_MASK;
}

static void ws_gpio_init(void) {
    RCC_AHB2ENR |= GPIOB_EN_BIT;
    (void)RCC_AHB2ENR;

    GPIO_OTYPER(GPIOB_BASE) &= ~PB2_BIT;
    GPIO_MODER(GPIOB_BASE) = (GPIO_MODER(GPIOB_BASE) & ~(3UL << (2U * 2U))) | (1UL << (2U * 2U));
    GPIO_BSRR(GPIOB_BASE) = PB2_CLR;
}

static void u6_flt_gpio_init(void) {
    RCC_AHB2ENR |= GPIOC_EN_BIT;
    (void)RCC_AHB2ENR;

    GPIO_PUPDR(GPIOC_BASE) &= ~(3UL << (13U * 2U));
    GPIO_MODER(GPIOC_BASE) &= ~(3UL << (13U * 2U)); /* input mode */
}

static bool u6_flt_asserted(void) {
    return (GPIO_IDR(GPIOC_BASE) & PC13_BIT) == 0U;
}

static void vin_path_gpio_init(void) {
    RCC_AHB2ENR |= GPIOA_EN_BIT;
    (void)RCC_AHB2ENR;

    GPIO_MODER(GPIOA_BASE) = (GPIO_MODER(GPIOA_BASE) & ~((3UL << (4U * 2U)) | (3UL << (5U * 2U)))) |
                             (3UL << (4U * 2U)) | /* PA4 analog (V_in_ADC) */
                             (1UL << (5U * 2U));  /* PA5 output (V_in_ON) */
    GPIO_BSRR(GPIOA_BASE) = PA5_BIT; /* default ON: active-high control */
}

static void vin_path_enable(bool enabled) {
    if (enabled) {
        GPIO_BSRR(GPIOA_BASE) = PA5_BIT;
    } else {
        GPIO_BSRR(GPIOA_BASE) = PA5_BIT << 16U;
    }
}

static bool adc1_init_pa4(void) {
    RCC_AHB2ENR |= ADC12_EN_BIT;
    (void)RCC_AHB2ENR;

    ADC12_CCR &= ~(0x3UL << 16U); /* CKMODE = HCLK/1 (synchronous clock) */

    ADC_CR &= ~(1UL << 29U); /* Exit deep-power-down */
    ADC_CR |= (1UL << 28U);  /* Enable ADC regulator */
    delay_cycles(1000U);

    if ((ADC_CR & 1UL) != 0U) {
        ADC_CR |= (1UL << 1U); /* ADDIS */
        if (!wait_reg32_clear(&ADC_CR, 1UL, 200000U)) {
            return false;
        }
    }

    ADC_CR |= (1UL << 31U); /* ADCAL */
    if (!wait_reg32_clear(&ADC_CR, (1UL << 31U), 500000U)) {
        return false;
    }

    ADC_ISR |= 1UL;      /* clear ADRDY */
    ADC_CFGR = 0U;       /* single conversion, software trigger */
    ADC_SMPR1 &= ~(7UL << 12U);
    ADC_SMPR1 |= (7UL << 12U); /* channel 4 sampling time */
    ADC_SQR1 = (4UL << 6U);    /* SQ1 = channel 4, sequence length = 1 */

    ADC_CR |= 1UL; /* ADEN */
    if (!wait_reg32_set(&ADC_ISR, 1UL, 500000U)) {
        return false;
    }
    return true;
}

static bool adc1_read_pa4_raw(uint16_t *raw_out) {
    ADC_ISR |= (1UL << 2U) | (1UL << 3U); /* clear EOC/EOS */
    ADC_CR |= (1UL << 2U);                /* ADSTART */
    if (!wait_reg32_set(&ADC_ISR, (1UL << 2U), 500000U)) {
        return false;
    }
    *raw_out = (uint16_t)(ADC_DR & 0x0FFFU);
    return true;
}

static uint32_t vin_input_mv_from_adc_raw(uint16_t raw) {
    const uint32_t adc_mv = ((uint32_t)raw * 3300UL) / 4095UL;
    return (adc_mv * VIN_ADC_SCALE_NUM) / VIN_ADC_SCALE_DEN;
}

static inline void ws_delay_short(void) {
    __asm volatile(
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        :
        :
        : "memory");
}

static inline void ws_delay_long(void) {
    __asm volatile(
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        :
        :
        : "memory");
}

static inline void ws_send_bit0(void) {
    GPIO_BSRR(GPIOB_BASE) = PB2_SET;
    ws_delay_short();
    GPIO_BSRR(GPIOB_BASE) = PB2_CLR;
    ws_delay_long();
}

static inline void ws_send_bit1(void) {
    GPIO_BSRR(GPIOB_BASE) = PB2_SET;
    ws_delay_long();
    GPIO_BSRR(GPIOB_BASE) = PB2_CLR;
    ws_delay_short();
}

static void ws_send_byte(uint8_t value) {
    for (uint8_t mask = 0x80U; mask != 0U; mask >>= 1U) {
        if ((value & mask) != 0U) {
            ws_send_bit1();
        } else {
            ws_send_bit0();
        }
    }
}

static void ws_send_pixel(uint8_t red, uint8_t green, uint8_t blue) {
    ws_send_byte(green);
    ws_send_byte(red);
    ws_send_byte(blue);
}

static void ws_latch_reset(void) {
    GPIO_BSRR(GPIOB_BASE) = PB2_CLR;
    delay_cycles(WS_RESET_CYCLES);
}

static void ws_send_two_pixels(uint8_t d8_red, uint8_t d8_green, uint8_t d8_blue,
                               uint8_t d11_red, uint8_t d11_green, uint8_t d11_blue) {
    ws_send_pixel(d8_red, d8_green, d8_blue);
    ws_send_pixel(d11_red, d11_green, d11_blue);
    ws_latch_reset();
}

static inline void i2c_half_period(void) {
    delay_cycles(70U);
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

static uint8_t i2c_read_byte(bool ack) {
    uint8_t value = 0U;
    i2c_sda_release();
    for (uint8_t bit = 0U; bit < 8U; ++bit) {
        value <<= 1U;
        i2c_half_period();
        i2c_scl_release();
        i2c_half_period();
        if (i2c_sda_read()) {
            value |= 1U;
        }
        i2c_scl_low();
    }

    if (ack) {
        i2c_sda_low();
    } else {
        i2c_sda_release();
    }
    i2c_half_period();
    i2c_scl_release();
    i2c_half_period();
    i2c_scl_low();
    i2c_sda_release();
    i2c_half_period();
    return value;
}

static bool i2c_write_bytes(uint8_t address7, const uint8_t *bytes, uint32_t len) {
    i2c_start();
    if (!i2c_write_byte((uint8_t)(address7 << 1U))) {
        i2c_stop();
        return false;
    }
    for (uint32_t i = 0U; i < len; ++i) {
        if (!i2c_write_byte(bytes[i])) {
            i2c_stop();
            return false;
        }
    }
    i2c_stop();
    return true;
}

static bool i2c_read_bytes(uint8_t address7, uint8_t *bytes, uint32_t len) {
    i2c_start();
    if (!i2c_write_byte((uint8_t)((address7 << 1U) | 1U))) {
        i2c_stop();
        return false;
    }
    for (uint32_t i = 0U; i < len; ++i) {
        bytes[i] = i2c_read_byte(i + 1U < len);
    }
    i2c_stop();
    return true;
}

static bool i2c_read_reg8(uint8_t address7, uint8_t reg, uint8_t *value) {
    i2c_start();
    if (!i2c_write_byte((uint8_t)(address7 << 1U))) {
        i2c_stop();
        return false;
    }
    if (!i2c_write_byte(reg)) {
        i2c_stop();
        return false;
    }
    i2c_start();
    if (!i2c_write_byte((uint8_t)((address7 << 1U) | 1U))) {
        i2c_stop();
        return false;
    }
    *value = i2c_read_byte(false);
    i2c_stop();
    return true;
}

static uint8_t crc8_aht20(const uint8_t *data, uint32_t len) {
    uint8_t crc = 0xFFU;
    for (uint32_t i = 0U; i < len; ++i) {
        crc ^= data[i];
        for (uint8_t b = 0U; b < 8U; ++b) {
            crc = (crc & 0x80U) ? (uint8_t)((crc << 1U) ^ 0x31U) : (uint8_t)(crc << 1U);
        }
    }
    return crc;
}

static bool aht20_probe(void) {
    i2c_start();
    const bool ack = i2c_write_byte((uint8_t)(AHT20_ADDR << 1U));
    i2c_stop();
    return ack;
}

static bool oled_probe(void) {
    i2c_start();
    const bool ack = i2c_write_byte((uint8_t)(OLED_ADDR << 1U));
    i2c_stop();
    return ack;
}

static bool u6_probe(void) {
    i2c_start();
    const bool ack = i2c_write_byte((uint8_t)(U6_ADDR << 1U));
    i2c_stop();
    return ack;
}

static bool u6_read_status(uint8_t *status_byte, uint8_t *pd_status0) {
    if (!i2c_read_reg8(U6_ADDR, 0x78U, status_byte)) {
        return false;
    }
    if (!i2c_read_reg8(U6_ADDR, 0x21U, pd_status0)) {
        return false;
    }
    return true;
}

static bool aht20_read_measurement(int32_t *temp_f_tenths, int32_t *rh_tenths) {
    const uint8_t init_cmd[3] = {0xBEU, 0x08U, 0x00U};
    (void)i2c_write_bytes(AHT20_ADDR, init_cmd, 3U);
    delay_ms(10U);

    const uint8_t trig_cmd[3] = {0xACU, 0x33U, 0x00U};
    if (!i2c_write_bytes(AHT20_ADDR, trig_cmd, 3U)) {
        return false;
    }
    delay_ms(90U);

    uint8_t rx[7] = {0U};
    if (!i2c_read_bytes(AHT20_ADDR, rx, 7U)) {
        return false;
    }
    if (crc8_aht20(rx, 6U) != rx[6]) {
        return false;
    }
    if ((rx[0] & 0x80U) != 0U) {
        return false;
    }

    const uint32_t raw_h = ((uint32_t)rx[1] << 12U) | ((uint32_t)rx[2] << 4U) | ((uint32_t)rx[3] >> 4U);
    const uint32_t raw_t = ((uint32_t)(rx[3] & 0x0FU) << 16U) | ((uint32_t)rx[4] << 8U) | (uint32_t)rx[5];

    *rh_tenths = (int32_t)((raw_h * 1000UL) / 1048576UL);
    const int32_t tc_tenths = (int32_t)((raw_t * 2000UL) / 1048576UL) - 500;
    *temp_f_tenths = ((tc_tenths * 9) / 5) + 320;
    return true;
}

static bool oled_write_cmd(uint8_t cmd) {
    const uint8_t bytes[2] = {0x00U, cmd};
    return i2c_write_bytes(OLED_ADDR, bytes, 2U);
}

static bool oled_write_data(const uint8_t *data, uint32_t len) {
    i2c_start();
    if (!i2c_write_byte((uint8_t)(OLED_ADDR << 1U))) {
        i2c_stop();
        return false;
    }
    if (!i2c_write_byte(0x40U)) {
        i2c_stop();
        return false;
    }
    for (uint32_t i = 0U; i < len; ++i) {
        if (!i2c_write_byte(data[i])) {
            i2c_stop();
            return false;
        }
    }
    i2c_stop();
    return true;
}

static bool oled_set_cursor(uint8_t page, uint8_t col) {
    return oled_write_cmd((uint8_t)(0xB0U | (page & 0x07U))) &&
           oled_write_cmd((uint8_t)(0x00U | (col & 0x0FU))) &&
           oled_write_cmd((uint8_t)(0x10U | ((col >> 4U) & 0x0FU)));
}

static bool oled_clear(void) {
    uint8_t zeros[16];
    for (uint8_t i = 0U; i < sizeof(zeros); ++i) {
        zeros[i] = 0U;
    }
    for (uint8_t page = 0U; page < 8U; ++page) {
        if (!oled_set_cursor(page, 0U)) {
            return false;
        }
        for (uint8_t block = 0U; block < 8U; ++block) {
            if (!oled_write_data(zeros, sizeof(zeros))) {
                return false;
            }
        }
    }
    return true;
}

static const uint8_t *font5x7(char c) {
    static const uint8_t F_SPACE[5] = {0, 0, 0, 0, 0};
    static const uint8_t F_COLON[5] = {0x00, 0x36, 0x36, 0x00, 0x00};
    static const uint8_t F_DOT[5] = {0x00, 0x60, 0x60, 0x00, 0x00};
    static const uint8_t F_PERCENT[5] = {0x62, 0x64, 0x08, 0x13, 0x23};
    static const uint8_t F_MINUS[5] = {0x08, 0x08, 0x08, 0x08, 0x08};
    static const uint8_t F_0[5] = {0x3E, 0x45, 0x49, 0x51, 0x3E};
    static const uint8_t F_1[5] = {0x00, 0x21, 0x7F, 0x01, 0x00};
    static const uint8_t F_2[5] = {0x23, 0x45, 0x49, 0x51, 0x21};
    static const uint8_t F_3[5] = {0x42, 0x41, 0x51, 0x69, 0x46};
    static const uint8_t F_4[5] = {0x0C, 0x14, 0x24, 0x7F, 0x04};
    static const uint8_t F_5[5] = {0x72, 0x51, 0x51, 0x51, 0x4E};
    static const uint8_t F_6[5] = {0x1E, 0x29, 0x49, 0x49, 0x06};
    static const uint8_t F_7[5] = {0x40, 0x47, 0x48, 0x50, 0x60};
    static const uint8_t F_8[5] = {0x36, 0x49, 0x49, 0x49, 0x36};
    static const uint8_t F_9[5] = {0x30, 0x49, 0x49, 0x4A, 0x3C};
    static const uint8_t F_F[5] = {0x7F, 0x48, 0x48, 0x48, 0x40};
    static const uint8_t F_H[5] = {0x7F, 0x08, 0x08, 0x08, 0x7F};
    static const uint8_t F_T[5] = {0x40, 0x40, 0x7F, 0x40, 0x40};

    if (c >= '0' && c <= '9') {
        static const uint8_t *digits[10] = {F_0, F_1, F_2, F_3, F_4, F_5, F_6, F_7, F_8, F_9};
        return digits[(uint8_t)(c - '0')];
    }
    switch (c) {
    case ':': return F_COLON;
    case '.': return F_DOT;
    case '%': return F_PERCENT;
    case '-': return F_MINUS;
    case 'F': return F_F;
    case 'H': return F_H;
    case 'T': return F_T;
    default: return F_SPACE;
    }
}

static bool oled_write_text_line(uint8_t page, const char *text) {
    if (!oled_set_cursor(page, 0U)) {
        return false;
    }
    while (*text != '\0') {
        const uint8_t *glyph = font5x7(*text++);
        if (!oled_write_data(glyph, 5U)) {
            return false;
        }
        static const uint8_t spacer = 0x00U;
        if (!oled_write_data(&spacer, 1U)) {
            return false;
        }
    }
    return true;
}

static bool oled_init(void) {
    static const uint8_t init_seq[] = {
        0xAEU, 0x20U, 0x00U, 0x40U, 0xA1U, 0xC0U, 0x81U, 0x7FU,
        0xA6U, 0xA8U, 0x3FU, 0xD3U, 0x00U, 0xD5U, 0x80U, 0xD9U,
        0xF1U, 0xDAU, 0x12U, 0xDBU, 0x40U, 0x8DU, 0x14U, 0xAFU
    };
    for (uint32_t i = 0U; i < sizeof(init_seq); ++i) {
        if (!oled_write_cmd(init_seq[i])) {
            return false;
        }
    }
    return oled_clear();
}

static void format_tenths(int32_t value_tenths, char suffix, char *out) {
    uint32_t pos = 0U;
    if (value_tenths < 0) {
        out[pos++] = '-';
        value_tenths = -value_tenths;
    }
    const uint32_t whole = (uint32_t)(value_tenths / 10);
    const uint32_t frac = (uint32_t)(value_tenths % 10);

    if (whole >= 100U) {
        out[pos++] = (char)('0' + ((whole / 100U) % 10U));
    }
    out[pos++] = (char)('0' + ((whole / 10U) % 10U));
    out[pos++] = (char)('0' + (whole % 10U));
    out[pos++] = '.';
    out[pos++] = (char)('0' + frac);
    out[pos++] = suffix;
    out[pos] = '\0';
}

static char hex_nibble(uint8_t x) {
    x &= 0x0FU;
    return (x < 10U) ? (char)('0' + x) : (char)('A' + (x - 10U));
}

static void format_hex8(uint8_t value, char *out) {
    out[0] = '0';
    out[1] = 'x';
    out[2] = hex_nibble((uint8_t)(value >> 4U));
    out[3] = hex_nibble(value);
    out[4] = '\0';
}

int main(void) {
    uart_gpio_init();
    uart_init();
    vin_path_gpio_init();
    const bool vin_adc_ok = adc1_init_pa4();
    i2c_gpio_init();
    ws_gpio_init();
    u6_flt_gpio_init();
    ws_latch_reset();

    uart_line("");
    uart_line("Solar Rev1 AHT20/OLED stage");
    uart_line("USART1 115200 8N1 on PA9/PA10");
    uart_line(vin_adc_ok ? "VIN ADC init OK" : "VIN ADC init FAIL");

    const bool has_aht20 = aht20_probe();
    const bool has_oled = oled_probe();
    const bool has_u6 = u6_probe();

    uart_write("I2C probe 0x38(AHT20): ");
    uart_line(has_aht20 ? "ACK" : "NACK");
    uart_write("I2C probe 0x3C(OLED): ");
    uart_line(has_oled ? "ACK" : "NACK");
    uart_write("I2C probe 0x6A(U6): ");
    uart_line(has_u6 ? "ACK" : "NACK");

    bool oled_ok = false;
    if (has_oled) {
        oled_ok = oled_init();
        uart_line(oled_ok ? "OLED init OK" : "OLED init FAIL");
    }

    bool heartbeat_phase = false;
    bool vin_path_on = true;
    bool vin_cutoff_reported = false;
    for (;;) {
        if (vin_adc_ok && vin_path_on) {
            uint16_t vin_adc_raw = 0U;
            if (adc1_read_pa4_raw(&vin_adc_raw)) {
                const uint32_t vin_input_mv = vin_input_mv_from_adc_raw(vin_adc_raw);
                if (vin_input_mv >= VIN_ADC_SHUTDOWN_MV) {
                    vin_path_on = false;
                    vin_path_enable(false);
                    if (!vin_cutoff_reported) {
                        uart_line("VIN cutoff: PA5 OFF (>=50V input)");
                        vin_cutoff_reported = true;
                    }
                }
            }
        }

        uint8_t u6_status = 0U;
        uint8_t u6_pd_status0 = 0U;
        char u6_status_hex[5];
        char u6_pd_hex[5];
        const bool u6_status_ok = has_u6 && u6_read_status(&u6_status, &u6_pd_status0);
        format_hex8(u6_status, u6_status_hex);
        format_hex8(u6_pd_status0, u6_pd_hex);

        int32_t tf_tenths = 0;
        int32_t rh_tenths = 0;
        const bool sample_ok = has_aht20 && aht20_read_measurement(&tf_tenths, &rh_tenths);

        if (sample_ok) {
            char temp_str[12];
            char rh_str[12];
            char line1[18];
            char line2[18];
            format_tenths(tf_tenths, 'F', temp_str);
            format_tenths(rh_tenths, '%', rh_str);

            uint32_t i = 0U;
            line1[i++] = 'T';
            line1[i++] = ':';
            for (uint32_t j = 0U; temp_str[j] != '\0'; ++j) {
                line1[i++] = temp_str[j];
            }
            line1[i] = '\0';

            i = 0U;
            line2[i++] = 'H';
            line2[i++] = ':';
            for (uint32_t j = 0U; rh_str[j] != '\0'; ++j) {
                line2[i++] = rh_str[j];
            }
            line2[i] = '\0';

            uart_write(line1);
            uart_write("  ");
            uart_line(line2);

            if (oled_ok) {
                (void)oled_clear();
                (void)oled_write_text_line(0U, line1);
                (void)oled_write_text_line(2U, line2);
            }
        } else {
            uart_line("AHT20 sample FAIL");
            if (oled_ok) {
                (void)oled_clear();
                (void)oled_write_text_line(0U, "AHT20 FAIL");
            }
        }

        if (u6_status_ok) {
            uart_write("U6 STATUS=");
            uart_write(u6_status_hex);
            uart_write(" PD0=");
            uart_write(u6_pd_hex);
            uart_write(" FLT_PIN=");
            uart_line(u6_flt_asserted() ? "LOW" : "HIGH");
        } else {
            uart_line("U6 status read FAIL");
        }

        if (heartbeat_phase) {
            ws_send_two_pixels(
                0x08U, 0x00U, 0x00U, /* D8 red */
                0x00U, 0x00U, 0x00U  /* D11 off */
            );
        } else {
            ws_send_two_pixels(
                0x00U, 0x00U, 0x00U, /* D8 off */
                0x08U, 0x00U, 0x00U  /* D11 red */
            );
        }
        heartbeat_phase = !heartbeat_phase;

        delay_ms(1000U);
    }
}
