/* Rev 1 stage: PB6/PB7 software I2C + USART1 + PB2 D8/D11 railroad heartbeat. */

#include <stdbool.h>
#include <stdint.h>

#define REG32(address) (*(volatile uint32_t *)(address))

#define RCC_BASE 0x40021000UL
#define RCC_AHB2ENR REG32(RCC_BASE + 0x4CUL)
#define RCC_AHB2RSTR REG32(RCC_BASE + 0x2CUL)
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
#define USART_ICR REG32(USART1_BASE + 0x20UL)
#define USART_RDR REG32(USART1_BASE + 0x24UL)
#define USART_TDR REG32(USART1_BASE + 0x28UL)

/* ESP32-C3 UART0 (GPIO21 U0TXD) arrives on PB11 = USART3_RX (AF7). Receive-only relay:
 * PB10 (to ESP GPIO20 U0RXD) is left in its reset high-Z state so the ESP is never driven. */
#define RCC_APB1ENR1 REG32(RCC_BASE + 0x58UL)
#define USART3_EN_BIT (1UL << 18)
#define USART3_BASE 0x40004800UL
#define USART3_CR1 REG32(USART3_BASE + 0x00UL)
#define USART3_BRR REG32(USART3_BASE + 0x0CUL)
#define USART3_ISR REG32(USART3_BASE + 0x1CUL)
#define USART3_ICR REG32(USART3_BASE + 0x20UL)
#define USART3_RDR REG32(USART3_BASE + 0x24UL)
#define ESPLOG_RX_BUF_SIZE 8192U
#define ESPLOG_LINE_MAX 160U
#define ESPLOG_LINES_PER_PUMP 8U

/* PA4 (SENSE_PV_V) is ADC2_IN17 on STM32G473; it is not routed to ADC1. */
#define ADC2_BASE 0x50000100UL
#define ADC12_COMMON_BASE 0x50000300UL
#define ADC_ISR REG32(ADC2_BASE + 0x00UL)
#define ADC_CR REG32(ADC2_BASE + 0x08UL)
#define ADC_CFGR REG32(ADC2_BASE + 0x0CUL)
#define ADC_SMPR2 REG32(ADC2_BASE + 0x18UL)
#define ADC_SQR1 REG32(ADC2_BASE + 0x30UL)
#define ADC_DR REG32(ADC2_BASE + 0x40UL)
#define VIN_ADC_CHANNEL 17UL
#define ADC12_CCR REG32(ADC12_COMMON_BASE + 0x08UL)

#define GPIOA_EN_BIT (1UL << 0)
#define GPIOB_EN_BIT (1UL << 1)
#define GPIOC_EN_BIT (1UL << 2)
#define USART1_EN_BIT (1UL << 14)
#define ADC12_EN_BIT (1UL << 13)
#define ADC12_RST_BIT (1UL << 13)

#define PB6_BIT (1UL << 6) /* SDA */
#define PB7_BIT (1UL << 7) /* SCL */
#define I2C_MASK (PB6_BIT | PB7_BIT)
#define PB2_BIT (1UL << 2) /* WS2812 data for D8->D11 chain */
#define PB2_SET PB2_BIT
#define PB2_CLR (PB2_BIT << 16U)
#define PC13_BIT (1UL << 13) /* U6_FLT (open-drain, pulled up by R34) */
#define PB12_BIT (1UL << 12) /* CTRL_EN_CHG: active-high EN/UVLO clamp */

#define AHT20_ADDR 0x38U
#define OLED_ADDR 0x3CU
#define U6_ADDR 0x6AU
#define WS_RESET_CYCLES (300U * 16U) /* 300 us at 16 MHz */
#define PA5_BIT (1UL << 5) /* V_in_ON */
#define PA8_BIT (1UL << 8) /* V_out _on: U14 EN/UVLO */

#define VIN_ADC_SHUTDOWN_MV 50000U
#define VIN_ADC_SCALE_NUM 190U
#define VIN_ADC_SCALE_DEN 10U
#define BENCH_JUMPER_FITTED 0U /* TP1-to-CHG_OUT_POS jumper removed; U6 TEST controls U14 */
#define ARM_WINDOW_MS 3000U
#define QUIET_TEST_LOG 1U
#define CMD_IDLE_EXECUTE_MS 150U
#define ENABLE_IVP_BENCH 0U
#define ARM_WRITE_D6_MFR_SPECIFIC_BITS32 0U
#define D6_BITS32_MASK 0x0CU
#define D6_BITS32_TARGET 0x0CU
#define START_TIMEOUT_MS 10000U
#define U6_STATUS_LOG_INTERVAL_MS 100U
#define U6_STATUS_LOG_DURATION_MS 10000U
#define U6_TEST_MIN_MV 3300U
#define U6_TEST_MAX_MV 14000U
#define U6_TEST_MIN_HEADROOM_MV 3000U
#define U6_TEST_MAX_VIN_MV 30000U
#define U6_TEST_POLL_MS 50U
#define U6_TEST_UPDATE_D7 0U
#define U6_TEST_D7_TARGET 0x2BU
#define BUILD_ID "u6q5q6-10s-esplog"
#define I2C_DEFAULT_HALF_PERIOD_CYCLES 70U
#define I2C_SCL_HIGH_TIMEOUT_MS 2U
#define LOOP_PERIOD_MS 1U
#define TELEMETRY_PERIOD_MS 1000U
#define UART_RX_BUF_SIZE 128U
#define U6_CLEAR_FAULTS_CMD 0x03U
#define U6_HAS_STATUS_WORD 0U
#define U6_HAS_STATUS_IOUT 0U
#define U6_HAS_STATUS_INPUT 0U
#define U6_HAS_STATUS_MFR_SPECIFIC 0U
#define OLED_SEG_REMAP_CMD 0xA1U
#define OLED_COM_SCAN_CMD 0xC0U
#if QUIET_TEST_LOG
#define MAYBE_UNUSED __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif

static volatile uint8_t uart_rx_buf[UART_RX_BUF_SIZE];
static volatile uint16_t uart_rx_head = 0U;
static volatile uint16_t uart_rx_tail = 0U;
static uint32_t i2c_half_period_cycles = I2C_DEFAULT_HALF_PERIOD_CYCLES;
static bool g_oled_ok = false;
static bool g_vin_adc_ok = false;
static bool g_vin_adc_recovery_reported = false;
static bool u6_read_reg8(uint8_t reg, uint8_t *value);
static void print_reg_result(const char *name, uint8_t reg);

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

/* Real-time clock for the U6 test loop: DWT cycle counter at the 16 MHz core clock.
 * delay_ms() is an uncalibrated busy loop and runs several times slower than its name. */
#define DWT_CTRL_REG REG32(0xE0001000UL)
#define DWT_CYCCNT_REG REG32(0xE0001004UL)
#define DEMCR_REG REG32(0xE000EDFCUL)
#define CYCLES_PER_MS 16000U

static void dwt_start(void) {
    DEMCR_REG |= (1UL << 24U);
    DWT_CYCCNT_REG = 0U;
    DWT_CTRL_REG |= 1UL;
}

static uint32_t dwt_ms_since(uint32_t t0_cycles) {
    return (DWT_CYCCNT_REG - t0_cycles) / CYCLES_PER_MS;
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
    const uint32_t NVIC_ISER1 = 0xE000E104UL;
    const uint32_t USART1_IRQ_BIT_IN_ISER1 = (1UL << 5U);
    RCC_APB2ENR |= USART1_EN_BIT;
    (void)RCC_APB2ENR;

    USART_CR1 = 0U;
    USART_BRR = 139U; /* 16 MHz / 115200 ~= 138.9 */
    USART_CR1 = (1UL << 0) | (1UL << 2) | (1UL << 3) | (1UL << 5);
    REG32(NVIC_ISER1) = USART1_IRQ_BIT_IN_ISER1;
}

static void uart_putc(char value) {
    while ((USART_ISR & (1UL << 7)) == 0U) {
    }
    USART_TDR = (uint32_t)(uint8_t)value;
}

static bool uart_try_getc(char *out) {
    if (uart_rx_tail != uart_rx_head) {
        *out = (char)uart_rx_buf[uart_rx_tail];
        uart_rx_tail = (uint16_t)((uart_rx_tail + 1U) % UART_RX_BUF_SIZE);
        return true;
    }
    return false;
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

void USART1_IRQHandler(void) {
    while ((USART_ISR & (1UL << 5U)) != 0U) {
        const uint8_t byte = (uint8_t)(USART_RDR & 0xFFU);
        const uint16_t next = (uint16_t)((uart_rx_head + 1U) % UART_RX_BUF_SIZE);
        if (next != uart_rx_tail) {
            uart_rx_buf[uart_rx_head] = byte;
            uart_rx_head = next;
        }
    }
    if ((USART_ISR & (1UL << 3U)) != 0U) {
        USART_ICR = (1UL << 3U);
    }
}

static void uart_write_u32(uint32_t value);

static volatile uint8_t esplog_rx_buf[ESPLOG_RX_BUF_SIZE];
static volatile uint16_t esplog_rx_head = 0U;
static volatile uint16_t esplog_rx_tail = 0U;
static volatile uint32_t esplog_dropped = 0U;
static bool g_esplog_enabled = true;
static char esplog_line[ESPLOG_LINE_MAX + 1U];
static uint32_t esplog_line_len = 0U;
static uint8_t esplog_esc_state = 0U; /* 0 normal, 1 after ESC, 2 inside CSI */

static void esplog_init(void) {
    const uint32_t NVIC_ISER1 = 0xE000E104UL;
    const uint32_t USART3_IRQ_BIT_IN_ISER1 = (1UL << 7U); /* IRQ 39 */
    RCC_AHB2ENR |= GPIOB_EN_BIT;
    RCC_APB1ENR1 |= USART3_EN_BIT;
    (void)RCC_APB1ENR1;

    GPIO_PUPDR(GPIOB_BASE) = (GPIO_PUPDR(GPIOB_BASE) & ~(3UL << (11U * 2U))) | (1UL << (11U * 2U));
    GPIO_AFRH(GPIOB_BASE) = (GPIO_AFRH(GPIOB_BASE) & ~(0xFUL << 12U)) | (7UL << 12U);
    GPIO_MODER(GPIOB_BASE) = (GPIO_MODER(GPIOB_BASE) & ~(3UL << (11U * 2U))) | (2UL << (11U * 2U));

    USART3_CR1 = 0U;
    USART3_BRR = 139U; /* 16 MHz / 115200 */
    USART3_CR1 = (1UL << 29) | (1UL << 0) | (1UL << 2) | (1UL << 5); /* FIFOEN, UE, RE, RXFNEIE */
    REG32(NVIC_ISER1) = USART3_IRQ_BIT_IN_ISER1;
}

void USART3_IRQHandler(void) {
    while ((USART3_ISR & (1UL << 5U)) != 0U) {
        const uint8_t byte = (uint8_t)(USART3_RDR & 0xFFU);
        const uint16_t next = (uint16_t)((esplog_rx_head + 1U) % ESPLOG_RX_BUF_SIZE);
        if (next != esplog_rx_tail) {
            esplog_rx_buf[esplog_rx_head] = byte;
            esplog_rx_head = next;
        } else {
            esplog_dropped++;
        }
    }
    /* Clear ORE/NF/FE (ESP reset glitches) so reception keeps running. */
    if ((USART3_ISR & 0x0EUL) != 0U) {
        USART3_ICR = 0x0EUL;
    }
}

static void esplog_flush_line(void) {
    esplog_line[esplog_line_len] = '\0';
    uart_write("ESP> ");
    uart_line(esplog_line);
    esplog_line_len = 0U;
}

/* Emits one complete ESP line; returns false when no full line is buffered. */
static bool esplog_pump_line(void) {
    while (esplog_rx_tail != esplog_rx_head) {
        const uint8_t c = esplog_rx_buf[esplog_rx_tail];
        esplog_rx_tail = (uint16_t)((esplog_rx_tail + 1U) % ESPLOG_RX_BUF_SIZE);
        if (esplog_esc_state == 1U) {
            esplog_esc_state = (c == '[') ? 2U : 0U;
            continue;
        }
        if (esplog_esc_state == 2U) {
            if (c >= 0x40U && c <= 0x7EU) {
                esplog_esc_state = 0U;
            }
            continue;
        }
        if (c == 0x1BU) {
            esplog_esc_state = 1U;
            continue;
        }
        if (c == '\n') {
            esplog_flush_line();
            return true;
        }
        if (c == '\r') {
            continue;
        }
        esplog_line[esplog_line_len++] = (c >= 0x20U && c <= 0x7EU) ? (char)c : '.';
        if (esplog_line_len >= ESPLOG_LINE_MAX) {
            esplog_flush_line();
            return true;
        }
    }
    return false;
}

static void esplog_pump(void) {
    if (esplog_dropped != 0U) {
        const uint32_t dropped = esplog_dropped;
        esplog_dropped = 0U;
        uart_write("ESP> [relay dropped ");
        uart_write_u32(dropped);
        uart_line(" bytes]");
    }
    for (uint32_t i = 0U; i < ESPLOG_LINES_PER_PUMP; i++) {
        if (!esplog_pump_line()) {
            break;
        }
    }
}

static void esplog_discard_pending(void) {
    esplog_rx_tail = esplog_rx_head;
    esplog_line_len = 0U;
    esplog_esc_state = 0U;
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

    GPIO_MODER(GPIOA_BASE) = (GPIO_MODER(GPIOA_BASE) & ~((3UL << (4U * 2U)) | (3UL << (5U * 2U)) | (3UL << (8U * 2U)))) |
                             (3UL << (4U * 2U)) | /* PA4 analog (V_in_ADC) */
                             (1UL << (5U * 2U)) | /* PA5 output (V_in_ON) */
                             (1UL << (8U * 2U));  /* PA8 output (V_out _on) */
    GPIO_BSRR(GPIOA_BASE) = PA5_BIT; /* default ON: active-high control */
    GPIO_BSRR(GPIOA_BASE) = PA8_BIT << 16U; /* default OFF */
}

static void u6_ctrl_gpio_init(void) {
    RCC_AHB2ENR |= GPIOB_EN_BIT;
    (void)RCC_AHB2ENR;

    GPIO_MODER(GPIOB_BASE) = (GPIO_MODER(GPIOB_BASE) & ~(3UL << (12U * 2U))) |
                             (1UL << (12U * 2U));
    GPIO_OTYPER(GPIOB_BASE) &= ~PB12_BIT;
    GPIO_BSRR(GPIOB_BASE) = PB12_BIT; /* default inhibit asserted */
}

static void u6_set_inhibit(bool inhibit) {
    if (inhibit) {
        GPIO_BSRR(GPIOB_BASE) = PB12_BIT;
    } else {
        GPIO_BSRR(GPIOB_BASE) = PB12_BIT << 16U;
    }
}

static void vin_path_enable(bool enabled) {
    if (enabled) {
        GPIO_BSRR(GPIOA_BASE) = PA5_BIT;
    } else {
        GPIO_BSRR(GPIOA_BASE) = PA5_BIT << 16U;
    }
}

static void out_path_enable(bool enabled) {
#if BENCH_JUMPER_FITTED
    (void)enabled;
    GPIO_BSRR(GPIOA_BASE) = PA8_BIT << 16U;
#else
    if (enabled) {
        GPIO_BSRR(GPIOA_BASE) = PA8_BIT;
    } else {
        GPIO_BSRR(GPIOA_BASE) = PA8_BIT << 16U;
    }
#endif
}

static bool adc1_init_pa4(void) {
    RCC_AHB2ENR |= ADC12_EN_BIT;
    (void)RCC_AHB2ENR;
    RCC_AHB2RSTR |= ADC12_RST_BIT;
    RCC_AHB2RSTR &= ~ADC12_RST_BIT;

    /* CKMODE=01 (HCLK/1). CKMODE=00 uses the RCC async clock, which has no source by default. */
    ADC12_CCR = (ADC12_CCR & ~(0x3UL << 16U)) | (1UL << 16U);

    ADC_CR |= (1UL << 29U);  /* Enter deep-power-down to force clean state */
    delay_cycles(200U);
    ADC_CR &= ~(1UL << 29U); /* Exit deep-power-down */
    ADC_CR |= (1UL << 28U);  /* Enable ADC regulator */
    delay_cycles(2000U);

    if ((ADC_CR & 1UL) != 0U) {
        ADC_CR |= (1UL << 1U); /* ADDIS */
        if (!wait_reg32_clear(&ADC_CR, 1UL, 200000U)) {
            return false;
        }
    }

    ADC_ISR = (1UL << 0U) | (1UL << 1U) | (1UL << 2U) | (1UL << 3U) | (1UL << 4U);

    ADC_CR |= (1UL << 31U); /* ADCAL */
    if (!wait_reg32_clear(&ADC_CR, (1UL << 31U), 500000U)) {
        return false;
    }

    ADC_ISR = 1UL;       /* clear ADRDY */
    ADC_CFGR = 0U;       /* single conversion, software trigger */
    ADC_SMPR2 &= ~(7UL << 21U);
    ADC_SMPR2 |= (7UL << 21U);              /* SMP17 = 640.5 cycles (high-impedance divider) */
    ADC_SQR1 = (VIN_ADC_CHANNEL << 6U);     /* SQ1 = channel 17, sequence length = 1 */

    ADC_CR |= 1UL; /* ADEN */
    if (!wait_reg32_set(&ADC_ISR, 1UL, 500000U)) {
        return false;
    }
    return true;
}

static bool adc1_read_pa4_raw(uint16_t *raw_out) {
    ADC_ISR = (1UL << 2U) | (1UL << 3U); /* clear EOC/EOS */
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

static bool ensure_vin_adc_ready(void) {
    if (g_vin_adc_ok) {
        return true;
    }
    g_vin_adc_ok = adc1_init_pa4();
    if (g_vin_adc_ok && !g_vin_adc_recovery_reported) {
        uart_line("VIN ADC init OK (retry)");
        g_vin_adc_recovery_reported = true;
    }
    return g_vin_adc_ok;
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

static MAYBE_UNUSED void ws_send_two_pixels(uint8_t d8_red, uint8_t d8_green, uint8_t d8_blue,
                               uint8_t d11_red, uint8_t d11_green, uint8_t d11_blue) {
    ws_send_pixel(d8_red, d8_green, d8_blue);
    ws_send_pixel(d11_red, d11_green, d11_blue);
    ws_latch_reset();
}

static inline void i2c_half_period(void) {
    delay_cycles(i2c_half_period_cycles);
}

static uint32_t i2c_effective_rate_khz(void) {
    const uint32_t scl_period_cycles = i2c_half_period_cycles * 2U;
    if (scl_period_cycles == 0U) {
        return 0U;
    }
    return 16000U / scl_period_cycles;
}

static bool i2c_set_speed_khz(uint32_t khz) {
    switch (khz) {
    case 100U:
        i2c_half_period_cycles = 80U;
        return true;
    case 50U:
        i2c_half_period_cycles = 160U;
        return true;
    case 20U:
        i2c_half_period_cycles = 400U;
        return true;
    case 10U:
        i2c_half_period_cycles = 800U;
        return true;
    default:
        return false;
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

static bool i2c_scl_read(void) {
    return (GPIO_IDR(GPIOB_BASE) & PB7_BIT) != 0U;
}

static bool i2c_scl_release_wait_high(void) {
    i2c_scl_release();
    const uint32_t timeout_cycles = I2C_SCL_HIGH_TIMEOUT_MS * 16000U;
    for (uint32_t i = 0U; i < timeout_cycles; ++i) {
        if (i2c_scl_read()) {
            return true;
        }
    }
    uart_line("SCL stuck low");
    return false;
}

static bool i2c_start(void) {
    i2c_sda_release();
    if (!i2c_scl_release_wait_high()) {
        return false;
    }
    i2c_half_period();
    i2c_sda_low();
    i2c_half_period();
    i2c_scl_low();
    return true;
}

static bool i2c_stop(void) {
    i2c_sda_low();
    i2c_half_period();
    if (!i2c_scl_release_wait_high()) {
        return false;
    }
    i2c_half_period();
    i2c_sda_release();
    i2c_half_period();
    return true;
}

static bool i2c_write_byte(uint8_t value) {
    for (uint8_t bit = 0U; bit < 8U; ++bit) {
        if ((value & 0x80U) != 0U) {
            i2c_sda_release();
        } else {
            i2c_sda_low();
        }
        i2c_half_period();
        if (!i2c_scl_release_wait_high()) {
            return false;
        }
        i2c_half_period();
        i2c_scl_low();
        value <<= 1U;
    }

    i2c_sda_release();
    i2c_half_period();
    if (!i2c_scl_release_wait_high()) {
        return false;
    }
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
        if (!i2c_scl_release_wait_high()) {
            return 0U;
        }
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
    if (!i2c_scl_release_wait_high()) {
        return 0U;
    }
    i2c_half_period();
    i2c_scl_low();
    i2c_sda_release();
    i2c_half_period();
    return value;
}

static bool i2c_write_bytes(uint8_t address7, const uint8_t *bytes, uint32_t len) {
    if (!i2c_start()) {
        return false;
    }
    if (!i2c_write_byte((uint8_t)(address7 << 1U))) {
        (void)i2c_stop();
        return false;
    }
    for (uint32_t i = 0U; i < len; ++i) {
        if (!i2c_write_byte(bytes[i])) {
            (void)i2c_stop();
            return false;
        }
    }
    return i2c_stop();
}

static bool i2c_read_bytes(uint8_t address7, uint8_t *bytes, uint32_t len) {
    if (!i2c_start()) {
        return false;
    }
    if (!i2c_write_byte((uint8_t)((address7 << 1U) | 1U))) {
        (void)i2c_stop();
        return false;
    }
    for (uint32_t i = 0U; i < len; ++i) {
        bytes[i] = i2c_read_byte(i + 1U < len);
    }
    return i2c_stop();
}

static bool i2c_read_reg8(uint8_t address7, uint8_t reg, uint8_t *value) {
    if (!i2c_start()) {
        return false;
    }
    if (!i2c_write_byte((uint8_t)(address7 << 1U))) {
        (void)i2c_stop();
        return false;
    }
    if (!i2c_write_byte(reg)) {
        (void)i2c_stop();
        return false;
    }
    if (!i2c_start()) {
        return false;
    }
    if (!i2c_write_byte((uint8_t)((address7 << 1U) | 1U))) {
        (void)i2c_stop();
        return false;
    }
    *value = i2c_read_byte(false);
    return i2c_stop();
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
    if (!i2c_start()) {
        return false;
    }
    const bool ack = i2c_write_byte((uint8_t)(AHT20_ADDR << 1U));
    (void)i2c_stop();
    return ack;
}

static bool oled_probe(void) {
    if (!i2c_start()) {
        return false;
    }
    const bool ack = i2c_write_byte((uint8_t)(OLED_ADDR << 1U));
    (void)i2c_stop();
    return ack;
}

static bool u6_probe(void) {
    if (!i2c_start()) {
        return false;
    }
    const bool ack = i2c_write_byte((uint8_t)(U6_ADDR << 1U));
    (void)i2c_stop();
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

static bool u6_write_reg8(uint8_t reg, uint8_t value) {
    const uint8_t bytes[2] = {reg, value};
    return i2c_write_bytes(U6_ADDR, bytes, 2U);
}

static bool u6_read_reg8(uint8_t reg, uint8_t *value) {
    return i2c_read_reg8(U6_ADDR, reg, value);
}

static bool u6_clear_faults(void) {
    const uint8_t cmd = U6_CLEAR_FAULTS_CMD; /* CLEAR_FAULTS command */
    return i2c_write_bytes(U6_ADDR, &cmd, 1U);
}

static MAYBE_UNUSED bool aht20_read_measurement(int32_t *temp_f_tenths, int32_t *rh_tenths) {
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
    if (!i2c_start()) {
        return false;
    }
    if (!i2c_write_byte((uint8_t)(OLED_ADDR << 1U))) {
        (void)i2c_stop();
        return false;
    }
    if (!i2c_write_byte(0x40U)) {
        (void)i2c_stop();
        return false;
    }
    for (uint32_t i = 0U; i < len; ++i) {
        if (!i2c_write_byte(data[i])) {
            (void)i2c_stop();
            return false;
        }
    }
    return i2c_stop();
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
    static const uint8_t F_EQUAL[5] = {0x14, 0x14, 0x14, 0x14, 0x14};
    static const uint8_t F_PERCENT[5] = {0x62, 0x64, 0x08, 0x13, 0x23};
    static const uint8_t F_MINUS[5] = {0x08, 0x08, 0x08, 0x08, 0x08};
    static const uint8_t F_A[5] = {0x3F, 0x48, 0x48, 0x48, 0x3F};
    static const uint8_t F_C[5] = {0x3E, 0x41, 0x41, 0x41, 0x22};
    static const uint8_t F_E[5] = {0x7F, 0x49, 0x49, 0x49, 0x41};
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
    static const uint8_t F_G[5] = {0x3E, 0x41, 0x49, 0x4B, 0x2F};
    static const uint8_t F_H[5] = {0x7F, 0x08, 0x08, 0x08, 0x7F};
    static const uint8_t F_I[5] = {0x00, 0x41, 0x7F, 0x41, 0x00};
    static const uint8_t F_L[5] = {0x7F, 0x01, 0x01, 0x01, 0x01};
    static const uint8_t F_M[5] = {0x7F, 0x20, 0x10, 0x20, 0x7F};
    static const uint8_t F_N[5] = {0x7F, 0x20, 0x10, 0x08, 0x7F};
    static const uint8_t F_O[5] = {0x3E, 0x41, 0x41, 0x41, 0x3E};
    static const uint8_t F_P[5] = {0x7F, 0x48, 0x48, 0x48, 0x30};
    static const uint8_t F_R[5] = {0x7F, 0x48, 0x4C, 0x4A, 0x31};
    static const uint8_t F_S[5] = {0x32, 0x49, 0x49, 0x49, 0x26};
    static const uint8_t F_T[5] = {0x40, 0x40, 0x7F, 0x40, 0x40};
    static const uint8_t F_U[5] = {0x7E, 0x01, 0x01, 0x01, 0x7E};
    static const uint8_t F_V[5] = {0x7C, 0x02, 0x01, 0x02, 0x7C};
    static const uint8_t F_W[5] = {0x7F, 0x02, 0x0C, 0x02, 0x7F};

    if (c >= '0' && c <= '9') {
        static const uint8_t *digits[10] = {F_0, F_1, F_2, F_3, F_4, F_5, F_6, F_7, F_8, F_9};
        return digits[(uint8_t)(c - '0')];
    }
    switch (c) {
    case ':': return F_COLON;
    case '.': return F_DOT;
    case '=': return F_EQUAL;
    case '%': return F_PERCENT;
    case '-': return F_MINUS;
    case 'A': return F_A;
    case 'C': return F_C;
    case 'E': return F_E;
    case 'F': return F_F;
    case 'G': return F_G;
    case 'H': return F_H;
    case 'I': return F_I;
    case 'L': return F_L;
    case 'M': return F_M;
    case 'N': return F_N;
    case 'O': return F_O;
    case 'P': return F_P;
    case 'R': return F_R;
    case 'S': return F_S;
    case 'T': return F_T;
    case 'U': return F_U;
    case 'V': return F_V;
    case 'W': return F_W;
    default: return F_SPACE;
    }
}

static MAYBE_UNUSED bool oled_write_text_line(uint8_t page, const char *text) {
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
        0xAEU, 0x20U, 0x00U, 0x40U, OLED_SEG_REMAP_CMD, OLED_COM_SCAN_CMD, 0x81U, 0x7FU,
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

static MAYBE_UNUSED void format_tenths(int32_t value_tenths, char suffix, char *out) {
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

static void format_mv_decimal(uint32_t mv, char *out) {
    const uint32_t whole = mv / 1000U;
    const uint32_t frac = (mv % 1000U) / 10U;
    uint32_t pos = 0U;

    if (whole >= 100U) {
        out[pos++] = (char)('0' + ((whole / 100U) % 10U));
    }
    if (whole >= 10U) {
        out[pos++] = (char)('0' + ((whole / 10U) % 10U));
    } else {
        out[pos++] = '0';
    }
    out[pos++] = (char)('0' + (whole % 10U));
    out[pos++] = '.';
    out[pos++] = (char)('0' + ((frac / 10U) % 10U));
    out[pos++] = (char)('0' + (frac % 10U));
    out[pos] = '\0';
}

static char status_bit_char(uint8_t status, uint8_t bit) {
    return ((status >> bit) & 1U) != 0U ? '1' : '0';
}

static void format_fault_tags_compact(uint8_t status, char *out, uint32_t out_len) {
    static const struct {
        uint8_t mask;
        const char *tag;
    } tags[] = {
        {0x40U, "OFF"},
        {0x20U, "OV"},
        {0x10U, "OC"},
        {0x08U, "UV"},
        {0x04U, "TMP"},
        {0x02U, "CML"},
        {0x01U, "OTH"},
    };
    uint32_t pos = 0U;
    bool first = true;

    if (out_len == 0U) {
        return;
    }
    out[0] = '\0';

    for (uint32_t i = 0U; i < (sizeof(tags) / sizeof(tags[0])); ++i) {
        if ((status & tags[i].mask) == 0U) {
            continue;
        }
        if (!first) {
            if (pos + 1U >= out_len) {
                break;
            }
            out[pos++] = ' ';
        }
        for (uint32_t j = 0U; tags[i].tag[j] != '\0'; ++j) {
            if (pos + 1U >= out_len) {
                out[pos] = '\0';
                return;
            }
            out[pos++] = tags[i].tag[j];
        }
        first = false;
    }

    if (first) {
        const char none_text[] = "NONE";
        for (uint32_t i = 0U; none_text[i] != '\0' && pos + 1U < out_len; ++i) {
            out[pos++] = none_text[i];
        }
    }
    out[pos] = '\0';
}

static void format_status_tags_active(uint8_t status, char *out, uint32_t out_len) {
    static const struct {
        uint8_t mask;
        const char *tag;
    } tags[] = {
        {0x40U, "OFF"},
        {0x20U, "OV"},
        {0x10U, "OC"},
        {0x08U, "UV"},
        {0x04U, "TMP"},
        {0x02U, "CML"},
        {0x01U, "OTH"},
    };
    uint32_t pos = 0U;
    bool first = true;

    if (out_len == 0U) {
        return;
    }
    out[0] = '\0';
    for (uint32_t i = 0U; i < (sizeof(tags) / sizeof(tags[0])); ++i) {
        if ((status & tags[i].mask) == 0U) {
            continue;
        }
        if (!first) {
            if (pos + 1U >= out_len) {
                break;
            }
            out[pos++] = ' ';
        }
        for (uint32_t j = 0U; tags[i].tag[j] != '\0'; ++j) {
            if (pos + 1U >= out_len) {
                out[pos] = '\0';
                return;
            }
            out[pos++] = tags[i].tag[j];
        }
        first = false;
    }
    if (first) {
        const char none_text[] = "NONE";
        for (uint32_t i = 0U; i < sizeof(none_text); ++i) {
            if (pos + 1U >= out_len) {
                break;
            }
            out[pos++] = none_text[i];
        }
    }
    out[pos] = '\0';
}

static void oled_show_u6_test_status(uint32_t elapsed_ms, uint8_t status_78, bool reinhibited,
                                     bool vin_ok, uint32_t vin_mv, bool vout_ok, uint32_t vout_mv) {
    if (!g_oled_ok) {
        return;
    }
    char line0[22];
    char line1[22];
    char line2[22];
    char line3[22];
    char tags[18];
    char vout_str[8];
    char vin_str[8];
    uint32_t pos = 0U;

    format_fault_tags_compact(status_78, tags, sizeof(tags));
    format_mv_decimal(vout_ok ? vout_mv : 0U, vout_str);
    format_mv_decimal(vin_ok ? vin_mv : 0U, vin_str);

    line0[0] = 'T'; line0[1] = '=';
    if (elapsed_ms >= 1000U) {
        line0[2] = '1'; line0[3] = '0'; line0[4] = '0'; line0[5] = '0';
    } else {
        line0[2] = (char)('0' + ((elapsed_ms / 100U) % 10U));
        line0[3] = (char)('0' + ((elapsed_ms / 10U) % 10U));
        line0[4] = (char)('0' + (elapsed_ms % 10U));
        line0[5] = ' ';
    }
    line0[6] = ' '; line0[7] = 'P'; line0[8] = 'H'; line0[9] = '=';
    line0[10] = reinhibited ? '1' : '0';
    line0[11] = '\0';

    line1[0] = 'S'; line1[1] = '=';
    line1[2] = hex_nibble((uint8_t)(status_78 >> 4U));
    line1[3] = hex_nibble(status_78);
    line1[4] = ' '; line1[5] = 'F'; line1[6] = 'L'; line1[7] = 'T'; line1[8] = ':';
    line1[9] = '\0';
    pos = 9U;
    for (uint32_t i = 0U; tags[i] != '\0' && pos + 1U < sizeof(line1); ++i) {
        line1[pos++] = tags[i];
    }
    line1[pos] = '\0';

    line2[0] = 'V'; line2[1] = 'O'; line2[2] = '=';
    line2[3] = vout_str[0]; line2[4] = vout_str[1]; line2[5] = vout_str[2]; line2[6] = vout_str[3]; line2[7] = vout_str[4];
    line2[8] = ' '; line2[9] = 'V'; line2[10] = 'I'; line2[11] = '=';
    line2[12] = vin_str[0]; line2[13] = vin_str[1]; line2[14] = vin_str[2]; line2[15] = vin_str[3]; line2[16] = vin_str[4];
    line2[17] = '\0';

    line3[0] = 'F'; line3[1] = 'L'; line3[2] = 'T'; line3[3] = '=';
    line3[4] = u6_flt_asserted() ? 'L' : 'H';
    line3[5] = u6_flt_asserted() ? 'O' : 'I';
    line3[6] = u6_flt_asserted() ? 'W' : 'G';
    line3[7] = u6_flt_asserted() ? '\0' : 'H';
    if (!u6_flt_asserted()) {
        line3[8] = '\0';
    }

    (void)oled_clear();
    (void)oled_write_text_line(0U, line0);
    (void)oled_write_text_line(1U, line1);
    (void)oled_write_text_line(2U, line2);
    (void)oled_write_text_line(3U, line3);
}

static bool u6_read_vout_target_mv(uint32_t *mv_out) {
    uint8_t d8 = 0U;
    uint8_t msb = 0U;
    uint8_t lsb = 0U;
    if (!u6_read_reg8(0xD8U, &d8) || !u6_read_reg8(0x0DU, &msb) || !u6_read_reg8(0x0CU, &lsb)) {
        return false;
    }
    const bool div20 = (d8 & 0x80U) != 0U;
    const uint16_t code = (uint16_t)(((uint16_t)(msb & 0x0FU) << 8U) | lsb);
    *mv_out = (uint32_t)code * (div20 ? 20U : 10U);
    return true;
}

static void uart_write_u32(uint32_t value) {
    char buf[10];
    uint32_t len = 0U;
    if (value == 0U) {
        uart_putc('0');
        return;
    }
    while (value > 0U && len < sizeof(buf)) {
        buf[len++] = (char)('0' + (value % 10U));
        value /= 10U;
    }
    while (len > 0U) {
        uart_putc(buf[--len]);
    }
}

static void print_i2c_speed_line(const char *prefix) {
    uart_write(prefix);
    uart_write(" I2C_SPEED_EFF_KHZ=");
    uart_write_u32(i2c_effective_rate_khz());
    uart_write(" HALF_CYC=");
    uart_write_u32(i2c_half_period_cycles);
    uart_line("");
}

static bool streq(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        if (*a != *b) {
            return false;
        }
        ++a;
        ++b;
    }
    return (*a == '\0') && (*b == '\0');
}

static void uppercase_ascii(char *text) {
    while (*text != '\0') {
        if (*text >= 'a' && *text <= 'z') {
            *text = (char)(*text - ('a' - 'A'));
        }
        ++text;
    }
}

static bool parse_u32_dec(const char *text, uint32_t *value_out) {
    uint32_t value = 0U;
    if (*text == '\0') {
        return false;
    }
    while (*text != '\0') {
        if (*text < '0' || *text > '9') {
            return false;
        }
        value = (value * 10U) + (uint32_t)(*text - '0');
        ++text;
    }
    *value_out = value;
    return true;
}

static bool hex_value(char c, uint8_t *out) {
    if (c >= '0' && c <= '9') {
        *out = (uint8_t)(c - '0');
        return true;
    }
    if (c >= 'A' && c <= 'F') {
        *out = (uint8_t)(10 + (c - 'A'));
        return true;
    }
    if (c >= 'a' && c <= 'f') {
        *out = (uint8_t)(10 + (c - 'a'));
        return true;
    }
    return false;
}

static bool parse_u8_hex(const char *text, uint8_t *value_out) {
    uint8_t high = 0U;
    uint8_t low = 0U;
    if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
        text += 2;
    }
    if (text[0] == '\0' || text[1] == '\0' || text[2] != '\0') {
        return false;
    }
    if (!hex_value(text[0], &high) || !hex_value(text[1], &low)) {
        return false;
    }
    *value_out = (uint8_t)((high << 4U) | low);
    return true;
}

static void print_u6_status_once(void) {
    uint8_t status = 0U;
    uint8_t pd0 = 0U;
    char status_hex[5];
    char pd_hex[5];
    char tags[32];
    if (!u6_read_status(&status, &pd0)) {
        uart_line("U6 status read FAIL");
        return;
    }
    format_hex8(status, status_hex);
    format_hex8(pd0, pd_hex);
    format_status_tags_active(status, tags, sizeof(tags));
    uart_write("U6 STATUS=");
    uart_write(status_hex);
    uart_write(" PD0=");
    uart_write(pd_hex);
    uart_write(" TAGS=");
    uart_write(tags);
    uart_write(" FLT_PIN=");
    uart_line(u6_flt_asserted() ? "LOW" : "HIGH");
}

static void print_u6_fault_registers_snapshot(void) {
    print_reg_result("78", 0x78U);
#if U6_HAS_STATUS_WORD
    print_reg_result("79", 0x79U);
#else
    uart_line("U6 79 N/A (STATUS_WORD not in Table 8-1)");
#endif
#if U6_HAS_STATUS_IOUT
    print_reg_result("7B", 0x7BU);
#else
    uart_line("U6 7B N/A (STATUS_IOUT not in Table 8-1)");
#endif
#if U6_HAS_STATUS_INPUT
    print_reg_result("7C", 0x7CU);
#else
    uart_line("U6 7C N/A (STATUS_INPUT not in Table 8-1)");
#endif
#if U6_HAS_STATUS_MFR_SPECIFIC
    print_reg_result("80", 0x80U);
#else
    uart_line("U6 80 N/A (STATUS_MFR_SPECIFIC not in Table 8-1)");
#endif
}

static void print_help(void) {
    uart_line("Commands:");
    uart_line("  HELP");
    uart_line("  STAT");
    uart_line("  ARM");
    uart_line("  U6 FAULTS");
    uart_line("  INH 1");
    uart_line("  DIV10 | DIV20");
    uart_line("  SETV <mV>");
    uart_line("  SET5");
    uart_line("  CLR");
    uart_line("  READ <regHex>");
    uart_line("  WRITE <regHex> <valHex>");
    uart_line("  U6 TEST <mV> <VinV>");
    uart_line("  U6 TEST <mV> <VinV> BLIND [kHz]");
    uart_line("  U6 BOOST ON|OFF  (bypass Vin>=target+3V guard)");
    uart_line("  U6 PCM OFF|DEFAULT  (D9 PCM_WINDOW_LOW 0 / 0xC)");
    uart_line("  I2C SPEED <kHz>");
    uart_line("  START <ms> (deprecated)");
    uart_line("  STOP");
    uart_line("  ESPLOG ON|OFF  (relay ESP UART0 via PB11; paused while armed/running)");
}

static void print_reg_result(const char *name, uint8_t reg) {
    uint8_t value = 0U;
    char hex[5];
    if (u6_read_reg8(reg, &value)) {
        format_hex8(value, hex);
        uart_write("U6 ");
        uart_write(name);
        uart_write("=");
        uart_line(hex);
    } else {
        uart_write("U6 ");
        uart_write(name);
        uart_line(" read FAIL");
    }
}

static void print_u6_set_snapshot(const char *label) {
    uart_line(label);
    print_reg_result("0A", 0x0AU);
    print_reg_result("D7", 0xD7U);
    print_reg_result("D3", 0xD3U);
    print_reg_result("DA", 0xDAU);
    print_reg_result("D8", 0xD8U);
    print_reg_result("0D", 0x0DU);
    print_reg_result("0C", 0x0CU);
}

static void print_u6_meas_snapshot(const char *label) {
    uart_line(label);
    print_reg_result("78", 0x78U);
    print_reg_result("21", 0x21U);
    uart_write("U6 FLT_PIN=");
    uart_line(u6_flt_asserted() ? "LOW" : "HIGH");
}

static void print_u6_test_post_fail_dump(void) {
    uart_line("U6_TEST POST_FAIL_DUMP");
    print_reg_result("78", 0x78U);
    print_reg_result("21", 0x21U);
    print_reg_result("D3", 0xD3U);
    print_reg_result("D7", 0xD7U);
    print_reg_result("D8", 0xD8U);
    print_reg_result("DA", 0xDAU);
    print_reg_result("0D", 0x0DU);
    print_reg_result("0C", 0x0CU);
    uart_write("U6 FLT_PIN=");
    uart_line(u6_flt_asserted() ? "LOW" : "HIGH");
}

static void print_u6_test_poll_read_fail(uint32_t elapsed_ms, uint8_t reg) {
    char reg_hex[5];
    format_hex8(reg, reg_hex);
    uart_write("U6_TEST POLL_READ_FAIL t=");
    uart_write_u32(elapsed_ms);
    uart_write("ms reg=");
    uart_write(reg_hex);
    uart_line(" i2c=bitbang");
}

typedef struct {
    bool inhibit_asserted;
    bool fault_latched;
    bool armed;
    bool baseline_valid;
    bool flt_low_baseline;
    uint8_t status_fault_mask_baseline;
    uint32_t run_ms_remaining;
    uint32_t arm_window_ms;
} control_state_t;

static void stop_and_latch_fault(control_state_t *ctl, const char *reason) {
    u6_set_inhibit(true);
    ctl->inhibit_asserted = true;
    out_path_enable(false);
    ctl->run_ms_remaining = 0U;
    ctl->arm_window_ms = 0U;
    ctl->armed = false;
    if (!ctl->fault_latched) {
        ctl->fault_latched = true;
        uart_write("FAULT_LATCHED: ");
        uart_line(reason);
    }
}

static void safety_fast_path(control_state_t *ctl, bool has_u6) {
    if (ctl->fault_latched) {
        return;
    }
    if (ctl->inhibit_asserted) {
        return;
    }
    if (u6_flt_asserted() && (!ctl->baseline_valid || !ctl->flt_low_baseline)) {
        stop_and_latch_fault(ctl, "FLT asserted");
        return;
    }
    if (!has_u6) {
        stop_and_latch_fault(ctl, "U6 missing");
        return;
    }
    uint8_t status_byte = 0U;
    uint8_t pd_status0 = 0U;
    (void)pd_status0;
    if (!u6_read_status(&status_byte, &pd_status0)) {
        stop_and_latch_fault(ctl, "U6 comms loss");
        return;
    }
    {
        const uint8_t current_mask = (uint8_t)(status_byte & 0x3FU);
        if (ctl->baseline_valid) {
            const uint8_t new_faults = (uint8_t)(current_mask & (uint8_t)~ctl->status_fault_mask_baseline);
            if (new_faults != 0U) {
                stop_and_latch_fault(ctl, "U6 STATUS new fault");
            }
        } else if (current_mask != 0U) {
            stop_and_latch_fault(ctl, "U6 STATUS fault");
        }
    }
}

static void arm_start_sequence(control_state_t *ctl, bool has_u6) {
    if (ctl->fault_latched) {
        if (!ctl->inhibit_asserted || ctl->run_ms_remaining > 0U) {
            uart_line("ERR fault latched");
            return;
        }
        ctl->fault_latched = false;
        uart_line("FAULT_LATCH CLEARED (software)");
    }
    if (!has_u6) {
        uart_line("ERR U6 unavailable");
        return;
    }
    if (!u6_clear_faults()) {
        uart_line("ERR CLR");
        return;
    }
    delay_ms(2U);
    uart_write("BUILD ");
    uart_line(BUILD_ID);
    print_i2c_speed_line("ARM");
    uart_line("ARM: leaving D3/DA/D7/D8/VOUT unchanged");
    {
        uint8_t status_byte = 0U;
        uint8_t pd_status0 = 0U;
        if (!u6_read_status(&status_byte, &pd_status0)) {
            uart_line("ERR status baseline");
            return;
        }
        ctl->status_fault_mask_baseline = (uint8_t)(status_byte & 0x3FU);
    }
    ctl->flt_low_baseline = u6_flt_asserted();
    ctl->baseline_valid = true;
    ctl->armed = true;
    ctl->arm_window_ms = ARM_WINDOW_MS;
    uart_line("ARMED");
    print_u6_set_snapshot("SET_SNAPSHOT");
    print_u6_meas_snapshot("MEAS_SNAPSHOT");
}

static bool g_u6_allow_boost = false;

static void u6_set_and_start(control_state_t *ctl, bool has_u6, uint32_t mv, uint32_t vin_v, bool blind, uint32_t speed_khz, bool speed_override) {
    if (!has_u6) {
        uart_line("ERR U6 unavailable");
        return;
    }
    if (!ctl->inhibit_asserted || ctl->run_ms_remaining > 0U) {
        uart_line("ERR converter not inhibited");
        return;
    }
    if (ctl->fault_latched) {
        uart_line("ERR fault latched");
        return;
    }
    if (mv < U6_TEST_MIN_MV || mv > U6_TEST_MAX_MV || (mv % 20U) != 0U) {
        uart_line("ERR mV out of range/step");
        return;
    }
    const uint32_t vin_mv = vin_v * 1000U;
    if (vin_mv > U6_TEST_MAX_VIN_MV) {
        uart_line("ERR Vin > 30V");
        return;
    }
    if (vin_mv < (mv + U6_TEST_MIN_HEADROOM_MV) && !g_u6_allow_boost) {
        uart_line("ERR Vin < target+3V (use U6 BOOST ON to allow)");
        return;
    }
    if (g_u6_allow_boost) {
        uart_line("U6_TEST BOOST_OVERRIDE=1 headroom guard bypassed");
    }
    {
        uart_write("U6_TEST VIN_V=");
        if (vin_v >= 10U) {
            uart_putc((char)('0' + ((vin_v / 10U) % 10U)));
        }
        uart_putc((char)('0' + (vin_v % 10U)));
        uart_line("");
    }
    uint8_t status_78 = 0U;
    if (!u6_read_reg8(0x78U, &status_78)) {
        uart_line("ERR read 78 precheck");
        return;
    }

    uint8_t d8 = 0U;
    if (!u6_read_reg8(0xD8U, &d8)) {
        uart_line("ERR read D8");
        return;
    }
    if ((d8 & 0x80U) == 0U) {
        uart_line("ERR D8 DIV20 required");
        return;
    }

    const uint16_t code = (uint16_t)(mv / 20U);
    const uint8_t target_0c = (uint8_t)(code & 0xFFU);
    const uint8_t target_0d = (uint8_t)((code >> 8U) & 0x0FU);
    if (!u6_write_reg8(0x0CU, target_0c) || !u6_write_reg8(0x0DU, target_0d)) {
        uart_line("ERR write VOUT_TARGET1");
        return;
    }

    uint8_t read_0c = 0U;
    uint8_t read_0d = 0U;
    if (!u6_read_reg8(0x0CU, &read_0c) || !u6_read_reg8(0x0DU, &read_0d)) {
        uart_line("ERR readback VOUT_TARGET1");
        return;
    }
    if (read_0c != target_0c || read_0d != target_0d) {
        uart_line("ERR VOUT_TARGET1 mismatch");
        return;
    }

    {
        char hx[5];
        uart_write("U6_TEST CODE_0D=");
        format_hex8(target_0d, hx);
        uart_write(hx);
        uart_write(" CODE_0C=");
        format_hex8(target_0c, hx);
        uart_line(hx);
        uart_line("U6_TEST NOTE 12000mV->0x0258, 14000mV->0x02BC");
        uart_write("U6_TEST READBACK 0D=");
        format_hex8(read_0d, hx);
        uart_write(hx);
        uart_write(" 0C=");
        format_hex8(read_0c, hx);
        uart_line(hx);
    }
    if (mv > 5500U) {
        uart_line("WARN DMM abort limit = target+1.0V; PSU abort = 0.3A");
    }

    {
        uint8_t d7 = 0U;
        char hx[5];
        if (!u6_read_reg8(0xD7U, &d7)) {
            uart_line("ERR read D7");
            return;
        }
#if U6_TEST_UPDATE_D7
        if (d7 != U6_TEST_D7_TARGET) {
            if (!u6_write_reg8(0xD7U, U6_TEST_D7_TARGET)) {
                uart_line("ERR write D7");
                return;
            }
            if (!u6_read_reg8(0xD7U, &d7)) {
                uart_line("ERR verify D7");
                return;
            }
        }
        uart_write("U6_TEST D7=");
        format_hex8(d7, hx);
        uart_line(hx);
#else
        uart_write("U6_TEST D7 unchanged=");
        format_hex8(d7, hx);
        uart_line(hx);
#endif
    }
    uart_line("U6_TEST I2C_IMPL=bitbang (no HAL ErrorCode)");

    {
        /* Write-1-to-clear OC/UV in STATUS_BYTE while PB12 still holds EN/UVLO low (works at idle;
         * the address-only CLEAR_FAULTS write did not clear a latched OC, and clearing during
         * the run wedged I2C). UV stays set at idle because of the clamp. */
        uint8_t pre_78 = 0U;
        char hx[5];
        const bool wr_ok = u6_write_reg8(0x78U, 0x18U);
        const bool rd_ok = u6_read_reg8(0x78U, &pre_78);
        uart_write("U6_TEST PRECLEAR W1C 78<=0x18 ");
        uart_write(wr_ok ? "OK" : "FAIL");
        uart_write(" 78=");
        if (rd_ok) {
            format_hex8(pre_78, hx);
            uart_line(hx);
        } else {
            uart_line("NA");
        }
    }

    out_path_enable(true);
    delay_ms(10U);
    dwt_start();
    u6_set_inhibit(false);
    const uint32_t t_release = DWT_CYCCNT_REG;
    ctl->inhibit_asserted = false;
    delay_ms(5U);

    if (blind) {
        uint32_t saved_half_cycles = i2c_half_period_cycles;
        if (speed_override) {
            if (!i2c_set_speed_khz(speed_khz)) {
                stop_and_latch_fault(ctl, "I2C speed");
                uart_line("ERR I2C speed; outputs inhibited");
                return;
            }
        }
        uart_write("U6_TEST MODE=BLIND I2C_SPEED_KHZ=");
        uart_write_u32(i2c_effective_rate_khz());
        uart_line("");
        while (dwt_ms_since(t_release) < START_TIMEOUT_MS) {
        }
        i2c_half_period_cycles = saved_half_cycles;
        u6_set_inhibit(true);
        out_path_enable(false);
        ctl->inhibit_asserted = true;
        ctl->run_ms_remaining = 0U;
        ctl->armed = false;
        ctl->arm_window_ms = 0U;
        uart_line("U6_TEST BLIND timeout -> INH=1");
        uart_line("U6_TEST DMM_CHG_OUT_POS=<manual>");
        return;
    }

    bool reinhibited = false;
    uint32_t next_ms = U6_STATUS_LOG_INTERVAL_MS;
    for (;;) {
        uint32_t elapsed_ms = dwt_ms_since(t_release);
        while (elapsed_ms < next_ms && (reinhibited || elapsed_ms < START_TIMEOUT_MS)) {
            elapsed_ms = dwt_ms_since(t_release);
        }
        if (!reinhibited && elapsed_ms >= START_TIMEOUT_MS) {
            u6_set_inhibit(true);
            out_path_enable(false);
            ctl->inhibit_asserted = true;
            ctl->run_ms_remaining = 0U;
            ctl->armed = false;
            ctl->arm_window_ms = 0U;
            reinhibited = true;
            uart_write("U6_TEST REINHIBIT t=");
            uart_write_u32(elapsed_ms);
            uart_line("ms");
        }
        uint8_t poll_78 = 0U;
        uint8_t poll_21 = 0U;
        uint16_t vin_adc_raw = 0U;
        uint32_t vin_mv = 0U;
        uint32_t vout_mv = 0U;
        bool vin_ok = false;
        bool vout_ok = false;
        char tags[32];
        char hx78[5];
        char hx21[5];
        if (!u6_read_reg8(0x78U, &poll_78)) {
            print_u6_test_poll_read_fail(elapsed_ms, 0x78U);
            print_u6_test_post_fail_dump();
            stop_and_latch_fault(ctl, "U6 test poll read fail");
            return;
        }
        if (!u6_read_reg8(0x21U, &poll_21)) {
            print_u6_test_poll_read_fail(elapsed_ms, 0x21U);
            print_u6_test_post_fail_dump();
            stop_and_latch_fault(ctl, "U6 test poll read fail");
            return;
        }
        if (!g_vin_adc_ok && ((elapsed_ms % 500U) == 0U)) {
            (void)ensure_vin_adc_ready();
        }
        if (g_vin_adc_ok && adc1_read_pa4_raw(&vin_adc_raw)) {
            vin_mv = vin_input_mv_from_adc_raw(vin_adc_raw);
            vin_ok = true;
        }
        if (u6_read_vout_target_mv(&vout_mv)) {
            vout_ok = true;
        }
        format_status_tags_active(poll_78, tags, sizeof(tags));
        uart_write("U6_STATUS_LOG t=");
        uart_write_u32(elapsed_ms);
        uart_write("ms 78=");
        format_hex8(poll_78, hx78);
        uart_write(hx78);
        uart_write(" 21=");
        format_hex8(poll_21, hx21);
        uart_write(hx21);
        uart_write(" TAGS=");
        uart_write(tags);
        uart_write(" VIN_mV=");
        if (vin_ok) {
            uart_write_u32(vin_mv);
        } else {
            uart_write("NA");
        }
        uart_write(" VOUT_mV=");
        if (vout_ok) {
            uart_write_u32(vout_mv);
        } else {
            uart_write("NA");
        }
        uart_write(" PHASE=");
        uart_line(reinhibited ? "POST_START" : "START");
        if (reinhibited) {
            oled_show_u6_test_status(elapsed_ms, poll_78, reinhibited, vin_ok, vin_mv, vout_ok, vout_mv);
        }
        if (elapsed_ms >= U6_STATUS_LOG_DURATION_MS) {
            break;
        }
        next_ms = (elapsed_ms / U6_STATUS_LOG_INTERVAL_MS + 1U) * U6_STATUS_LOG_INTERVAL_MS;
    }
    if (!reinhibited) {
        u6_set_inhibit(true);
        out_path_enable(false);
        ctl->inhibit_asserted = true;
        ctl->run_ms_remaining = 0U;
        ctl->armed = false;
        ctl->arm_window_ms = 0U;
        uart_line("U6_TEST REINHIBIT t=<forced>");
    }
    uart_write("U6_TEST status log complete (");
    uart_write_u32(U6_STATUS_LOG_DURATION_MS / 1000U);
    uart_line("s)");
    uart_line("U6_TEST DMM_CHG_OUT_POS=<manual>");
}

static void run_command(const char *line, control_state_t *ctl, bool has_u6) {
    char cmd[16] = {0};
    char arg1[16] = {0};
    char arg2[16] = {0};
    char arg3[16] = {0};
    char arg4[16] = {0};
    char arg5[16] = {0};
    uint32_t ci = 0U;
    uint32_t ai = 0U;
    uint32_t bi = 0U;
    uint32_t di = 0U;
    uint32_t ei = 0U;
    uint32_t fi = 0U;
    uint32_t idx = 0U;

    while (line[idx] != '\0' &&
           (((unsigned char)line[idx] <= (unsigned char)' ') || ((unsigned char)line[idx] > 0x7EU))) {
        ++idx;
    }
    while (line[idx] != '\0' &&
           ((unsigned char)line[idx] > (unsigned char)' ') &&
           ((unsigned char)line[idx] <= 0x7EU) &&
           ci + 1U < sizeof(cmd)) {
        cmd[ci++] = line[idx++];
    }
    cmd[ci] = '\0';
    while (line[idx] != '\0' &&
           (((unsigned char)line[idx] <= (unsigned char)' ') || ((unsigned char)line[idx] > 0x7EU))) {
        ++idx;
    }
    while (line[idx] != '\0' &&
           ((unsigned char)line[idx] > (unsigned char)' ') &&
           ((unsigned char)line[idx] <= 0x7EU) &&
           ai + 1U < sizeof(arg1)) {
        arg1[ai++] = line[idx++];
    }
    arg1[ai] = '\0';
    while (line[idx] != '\0' &&
           (((unsigned char)line[idx] <= (unsigned char)' ') || ((unsigned char)line[idx] > 0x7EU))) {
        ++idx;
    }
    while (line[idx] != '\0' &&
           ((unsigned char)line[idx] > (unsigned char)' ') &&
           ((unsigned char)line[idx] <= 0x7EU) &&
           bi + 1U < sizeof(arg2)) {
        arg2[bi++] = line[idx++];
    }
    arg2[bi] = '\0';
    while (line[idx] != '\0' &&
           (((unsigned char)line[idx] <= (unsigned char)' ') || ((unsigned char)line[idx] > 0x7EU))) {
        ++idx;
    }
    while (line[idx] != '\0' &&
           ((unsigned char)line[idx] > (unsigned char)' ') &&
           ((unsigned char)line[idx] <= 0x7EU) &&
           di + 1U < sizeof(arg3)) {
        arg3[di++] = line[idx++];
    }
    arg3[di] = '\0';
    while (line[idx] != '\0' &&
           (((unsigned char)line[idx] <= (unsigned char)' ') || ((unsigned char)line[idx] > 0x7EU))) {
        ++idx;
    }
    while (line[idx] != '\0' &&
           ((unsigned char)line[idx] > (unsigned char)' ') &&
           ((unsigned char)line[idx] <= 0x7EU) &&
           ei + 1U < sizeof(arg4)) {
        arg4[ei++] = line[idx++];
    }
    arg4[ei] = '\0';
    while (line[idx] != '\0' &&
           (((unsigned char)line[idx] <= (unsigned char)' ') || ((unsigned char)line[idx] > 0x7EU))) {
        ++idx;
    }
    while (line[idx] != '\0' &&
           ((unsigned char)line[idx] > (unsigned char)' ') &&
           ((unsigned char)line[idx] <= 0x7EU) &&
           fi + 1U < sizeof(arg5)) {
        arg5[fi++] = line[idx++];
    }
    arg5[fi] = '\0';
    uppercase_ascii(cmd);
    uppercase_ascii(arg1);
    uppercase_ascii(arg2);
    uppercase_ascii(arg3);
    uppercase_ascii(arg4);
    uppercase_ascii(arg5);
    uart_write("CMD<");
    uart_write(line);
    uart_line(">");

    if (streq(cmd, "HELP")) {
        print_help();
        return;
    }
    if (streq(cmd, "ESPLOG")) {
        if (streq(arg1, "ON")) {
            esplog_discard_pending();
            g_esplog_enabled = true;
            uart_line("ESPLOG ON");
        } else if (streq(arg1, "OFF")) {
            g_esplog_enabled = false;
            uart_line("ESPLOG OFF");
        } else {
            uart_line("ERR ESPLOG ON|OFF");
        }
        return;
    }
    if (streq(cmd, "ARM")) {
        arm_start_sequence(ctl, has_u6);
        return;
    }
    if (streq(cmd, "U6") && streq(arg1, "TEST")) {
        uint32_t mv = 0U;
        uint32_t vin_v = 0U;
        bool blind = false;
        bool speed_override = false;
        uint32_t speed_khz = 0U;
        if (!parse_u32_dec(arg2, &mv) || !parse_u32_dec(arg3, &vin_v)) {
            uart_line("ERR usage: U6 TEST <mV> <VinV>");
            return;
        }
        if (arg4[0] != '\0') {
            if (!streq(arg4, "BLIND")) {
                uart_line("ERR usage: U6 TEST <mV> <VinV> BLIND [kHz]");
                return;
            }
            blind = true;
        }
        if (arg5[0] != '\0') {
            if (!blind || !parse_u32_dec(arg5, &speed_khz)) {
                uart_line("ERR usage: U6 TEST <mV> <VinV> BLIND [kHz]");
                return;
            }
            speed_override = true;
        }
        u6_set_and_start(ctl, has_u6, mv, vin_v, blind, speed_khz, speed_override);
        return;
    }
    if (streq(cmd, "U6") && streq(arg1, "BOOST")) {
        if (streq(arg2, "ON")) {
            g_u6_allow_boost = true;
        } else if (streq(arg2, "OFF")) {
            g_u6_allow_boost = false;
        } else {
            uart_line("ERR usage: U6 BOOST ON|OFF");
            return;
        }
        uart_line(g_u6_allow_boost ? "U6_BOOST ON (Vin headroom guard bypassed)" : "U6_BOOST OFF");
        return;
    }
    if (streq(cmd, "U6") && streq(arg1, "PCM")) {
        if (!has_u6) {
            uart_line("ERR U6 unavailable");
            return;
        }
        if (!ctl->inhibit_asserted || ctl->run_ms_remaining > 0U) {
            uart_line("ERR converter not inhibited");
            return;
        }
        const bool pcm_off = streq(arg2, "OFF");
        if (!pcm_off && !streq(arg2, "DEFAULT")) {
            uart_line("ERR usage: U6 PCM OFF|DEFAULT");
            return;
        }
        uint8_t d9 = 0U;
        if (!u6_read_reg8(0xD9U, &d9)) {
            uart_line("ERR read D9");
            return;
        }
        d9 = (uint8_t)((d9 & 0xE0U) | (pcm_off ? 0x00U : 0x0CU));
        if (!u6_write_reg8(0xD9U, d9)) {
            uart_line("ERR write D9");
            return;
        }
        uint8_t d9_rb = 0U;
        if (!u6_read_reg8(0xD9U, &d9_rb) || d9_rb != d9) {
            uart_line("ERR D9 readback mismatch");
            return;
        }
        uart_write(pcm_off ? "OK PCM_WINDOW_LOW=0 D9=" : "OK PCM_WINDOW_LOW=default D9=");
        {
            char hx[5];
            format_hex8(d9_rb, hx);
            uart_line(hx);
        }
        return;
    }
    if (streq(cmd, "U6") && streq(arg1, "FAULTS")) {
        char cmd_hex[5];
        if (!has_u6) {
            uart_line("ERR U6 unavailable");
            return;
        }
        uart_line("U6_FAULTS PRE");
        print_u6_fault_registers_snapshot();
        format_hex8(U6_CLEAR_FAULTS_CMD, cmd_hex);
        uart_write("U6 CLEAR_FAULTS cmd=");
        uart_line(cmd_hex);
        uart_line(u6_clear_faults() ? "U6 CLEAR OK" : "U6 CLEAR FAIL");
        uart_line("U6_FAULTS POST");
        print_u6_fault_registers_snapshot();
        return;
    }
    if (streq(cmd, "I2C") && streq(arg1, "SPEED")) {
        uint32_t speed_khz = 0U;
        if (!parse_u32_dec(arg2, &speed_khz) || arg3[0] != '\0') {
            uart_line("ERR usage: I2C SPEED <kHz>");
            return;
        }
        if (!i2c_set_speed_khz(speed_khz)) {
            uart_line("ERR I2C speed options: 100|50|20|10");
            return;
        }
        uart_write("I2C SPEED SET EFF_KHZ=");
        uart_write_u32(i2c_effective_rate_khz());
        uart_write(" HALF_CYC=");
        uart_write_u32(i2c_half_period_cycles);
        uart_line("");
        return;
    }
    if (streq(cmd, "STAT")) {
        if (!has_u6) {
            uart_line("U6 not present");
            return;
        }
        print_u6_set_snapshot("SET_VALUES");
        print_u6_meas_snapshot("MEAS_VALUES");
        print_u6_status_once();
        return;
    }
    if (streq(cmd, "INH")) {
        if (streq(arg1, "1")) {
            stop_and_latch_fault(ctl, "INH 1 command");
            uart_line("INH=1");
            return;
        }
        if (streq(arg1, "0")) {
            uart_line("ERR INH 0 disabled; use ARM then START");
            return;
        }
        uart_line("ERR usage: INH 1");
        return;
    }
    if (!has_u6) {
        uart_line("ERR U6 unavailable");
        return;
    }
    if (streq(cmd, "DIV10") || streq(cmd, "DIV20")) {
        uint8_t d8 = 0U;
        if (!u6_read_reg8(0xD8U, &d8)) {
            uart_line("ERR read D8");
            return;
        }
        if (streq(cmd, "DIV10")) {
            d8 &= (uint8_t)~0x80U;
        } else {
            d8 |= 0x80U;
        }
        if (!u6_write_reg8(0xD8U, d8)) {
            uart_line("ERR write D8");
            return;
        }
        uart_write("OK D8=");
        {
            char hx[5];
            format_hex8(d8, hx);
            uart_line(hx);
        }
        return;
    }
    if (streq(cmd, "SETV")) {
        uint32_t mv = 0U;
        uint8_t d8 = 0U;
        if (!parse_u32_dec(arg1, &mv)) {
            uart_line("ERR usage: SETV <mV>");
            return;
        }
        if (!u6_read_reg8(0xD8U, &d8)) {
            uart_line("ERR read D8");
            return;
        }
        const bool div20 = (d8 & 0x80U) != 0U;
        const uint32_t step_mv = div20 ? 20U : 10U;
        const uint32_t min_mv = div20 ? 3300U : 1000U;
        const uint32_t max_mv = div20 ? 48000U : 24000U;
        if (mv < min_mv || mv > max_mv || (mv % step_mv) != 0U) {
            uart_line("ERR out of range/step");
            return;
        }
        const uint16_t code = (uint16_t)(mv / step_mv);
        if (!u6_write_reg8(0x0DU, (uint8_t)((code >> 8U) & 0x0FU)) ||
            !u6_write_reg8(0x0CU, (uint8_t)(code & 0xFFU))) {
            uart_line("ERR write VOUT");
            return;
        }
        uart_line("OK VOUT set");
        return;
    }
    if (streq(cmd, "SET5")) {
        uint8_t d8 = 0U;
        if (!u6_read_reg8(0xD8U, &d8)) {
            uart_line("ERR read D8");
            return;
        }
        d8 &= (uint8_t)~0x80U;
        if (!u6_write_reg8(0xD8U, d8) || !u6_write_reg8(0x0DU, 0x01U) || !u6_write_reg8(0x0CU, 0xF4U)) {
            uart_line("ERR set5");
            return;
        }
        uart_line("OK SET5");
        return;
    }
    if (streq(cmd, "CLR")) {
        uart_line(u6_clear_faults() ? "OK CLR" : "ERR CLR");
        return;
    }
    if (streq(cmd, "READ")) {
        uint8_t reg = 0U;
        uint8_t val = 0U;
        char hx[5];
        if (!parse_u8_hex(arg1, &reg)) {
            uart_line("ERR usage: READ <regHex>");
            return;
        }
        if (!u6_read_reg8(reg, &val)) {
            uart_line("ERR READ");
            return;
        }
        format_hex8(val, hx);
        uart_write("READ ");
        uart_write(arg1);
        uart_write(" = ");
        uart_line(hx);
        return;
    }
    if (streq(cmd, "WRITE")) {
        uint8_t reg = 0U;
        uint8_t val = 0U;
        if (!parse_u8_hex(arg1, &reg) || !parse_u8_hex(arg2, &val)) {
            uart_line("ERR usage: WRITE <regHex> <valHex>");
            return;
        }
        uart_line(u6_write_reg8(reg, val) ? "OK WRITE" : "ERR WRITE");
        return;
    }
    if (streq(cmd, "START")) {
        uart_line("ERR START replaced; use U6 TEST <mV> <VinV>");
        return;
    }
    if (streq(cmd, "STOP")) {
        stop_and_latch_fault(ctl, "STOP command");
        uart_line("STOP");
        return;
    }
    uart_line("ERR unknown command");
}

int main(void) {
    uart_gpio_init();
    uart_init();
    vin_path_gpio_init();
    u6_ctrl_gpio_init();
    g_vin_adc_ok = adc1_init_pa4();
    i2c_gpio_init();
    ws_gpio_init();
    u6_flt_gpio_init();
    ws_latch_reset();

    uart_line("");
    uart_line("Solar Rev1 AHT20/OLED stage");
    uart_line("USART1 115200 8N1 on PA9/PA10");
    uart_write("BUILD ");
    uart_line(BUILD_ID);
    esplog_init();
    uart_line("ESPLOG ON (USART3 RX PB11 <- ESP GPIO21, 115200)");
    uart_line(g_vin_adc_ok ? "VIN ADC init OK" : "VIN ADC init FAIL");

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
    g_oled_ok = oled_ok;

    bool heartbeat_phase = false;
    bool vin_path_on = true;
    bool vin_cutoff_reported = false;
    control_state_t ctl = {
        .inhibit_asserted = true,
        .fault_latched = false,
        .armed = false,
        .baseline_valid = false,
        .flt_low_baseline = false,
        .status_fault_mask_baseline = 0U,
        .run_ms_remaining = 0U,
        .arm_window_ms = 0U,
    };
    char cmd_buf[64];
    uint32_t cmd_len = 0U;
    uint32_t cmd_idle_ms = 0U;
    uint32_t telemetry_tick = 0U;
    uint32_t vin_adc_retry_ms = 0U;
    print_help();
    for (;;) {
        char rx = 0;
        bool saw_rx = false;
        while (uart_try_getc(&rx)) {
            saw_rx = true;
            if (rx == '\r' || rx == '\n') {
                if (cmd_len > 0U) {
                    cmd_buf[cmd_len] = '\0';
                    run_command(cmd_buf, &ctl, has_u6);
                    cmd_len = 0U;
                    cmd_idle_ms = 0U;
                }
            } else if (cmd_len + 1U < sizeof(cmd_buf)) {
                cmd_buf[cmd_len++] = rx;
                cmd_idle_ms = 0U;
            }
        }
        if (!saw_rx && cmd_len > 0U) {
            cmd_idle_ms += LOOP_PERIOD_MS;
            if (cmd_idle_ms >= CMD_IDLE_EXECUTE_MS) {
                cmd_buf[cmd_len] = '\0';
                run_command(cmd_buf, &ctl, has_u6);
                cmd_len = 0U;
                cmd_idle_ms = 0U;
            }
        }

        if (ctl.run_ms_remaining > 0U) {
            if (ctl.run_ms_remaining <= LOOP_PERIOD_MS) {
                stop_and_latch_fault(&ctl, "START timeout");
                uart_line("START timeout -> STOP");
            } else {
                ctl.run_ms_remaining -= LOOP_PERIOD_MS;
            }
        }
        if (ctl.arm_window_ms > 0U) {
            if (ctl.arm_window_ms <= LOOP_PERIOD_MS) {
                ctl.arm_window_ms = 0U;
                ctl.armed = false;
                uart_line("ARM window expired");
            } else {
                ctl.arm_window_ms -= LOOP_PERIOD_MS;
            }
        }

        safety_fast_path(&ctl, has_u6);

        if (!g_vin_adc_ok) {
            vin_adc_retry_ms += LOOP_PERIOD_MS;
            if (vin_adc_retry_ms >= 500U) {
                vin_adc_retry_ms = 0U;
                (void)ensure_vin_adc_ready();
            }
        } else {
            vin_adc_retry_ms = 0U;
        }

        if (g_vin_adc_ok && vin_path_on) {
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

        if (telemetry_tick == 0U) {
            uint8_t u6_status = 0U;
            uint8_t u6_pd_status0 = 0U;
            char u6_status_hex[5];
            char u6_pd_hex[5];
            char u6_tags[32];
            const bool u6_status_ok = has_u6 && u6_read_status(&u6_status, &u6_pd_status0);
            format_hex8(u6_status, u6_status_hex);
            format_hex8(u6_pd_status0, u6_pd_hex);
            format_status_tags_active(u6_status, u6_tags, sizeof(u6_tags));

            uint16_t vin_adc_raw = 0U;
            uint32_t vin_mv = 0U;
            const bool vin_ok = g_vin_adc_ok && adc1_read_pa4_raw(&vin_adc_raw);
            if (vin_ok) {
                vin_mv = vin_input_mv_from_adc_raw(vin_adc_raw);
            }

            uint32_t vout_mv = 0U;
            const bool vout_ok = has_u6 && u6_read_vout_target_mv(&vout_mv);

#if !QUIET_TEST_LOG
            int32_t tf_tenths = 0;
            int32_t rh_tenths = 0;
            const bool sample_ok = has_aht20 && aht20_read_measurement(&tf_tenths, &rh_tenths);
            if (sample_ok) {
                char temp_str[12];
                char rh_str[12];
                format_tenths(tf_tenths, 'F', temp_str);
                format_tenths(rh_tenths, '%', rh_str);
                uart_write("T:");
                uart_write(temp_str);
                uart_write("  H:");
                uart_line(rh_str);
            } else {
                uart_line("AHT20 sample FAIL");
            }
#else
            (void)has_aht20;
#endif

            if (u6_status_ok) {
#if !QUIET_TEST_LOG
                uart_write("MEAS_U6_STATUS=");
                uart_write(u6_status_hex);
                uart_write(" MEAS_U6_PD0=");
                uart_write(u6_pd_hex);
                uart_write(" TAGS=");
                uart_write(u6_tags);
                uart_write(" VIN_mV=");
                if (vin_ok) {
                    uart_write_u32(vin_mv);
                } else {
                    uart_write("NA");
                }
                uart_write(" VOUT_mV=");
                if (vout_ok) {
                    uart_write_u32(vout_mv);
                } else {
                    uart_write("NA");
                }
                uart_write(" MEAS_FLT_PIN=");
                uart_line(u6_flt_asserted() ? "LOW" : "HIGH");
#endif
            } else {
#if !QUIET_TEST_LOG
                uart_line("U6 status read FAIL");
#endif
                if (!ctl.inhibit_asserted && !ctl.fault_latched) {
                    stop_and_latch_fault(&ctl, "telemetry comms loss");
                }
            }

            if (oled_ok) {
                char line0[22];
                char line2[22];
                char line4[22];
                char line6[22];
                char fault_tags_compact[18];
                char vout_str[8];
                char vin_str[8];
                uint32_t pos = 0U;

                format_mv_decimal(vout_ok ? vout_mv : 0U, vout_str);
                format_mv_decimal(vin_ok ? vin_mv : 0U, vin_str);
                format_fault_tags_compact(u6_status, fault_tags_compact, sizeof(fault_tags_compact));

                line0[0] = 'S'; line0[1] = '='; line0[2] = u6_status_hex[2]; line0[3] = u6_status_hex[3];
                line0[4] = ' '; line0[5] = ' ';
                line0[7] = 'F'; line0[8] = 'L'; line0[9] = 'T'; line0[10] = '=';
                line0[11] = u6_flt_asserted() ? 'L' : 'H';
                line0[12] = u6_flt_asserted() ? 'O' : 'I';
                line0[13] = u6_flt_asserted() ? 'W' : 'G';
                line0[14] = u6_flt_asserted() ? '\0' : 'H';
                if (!u6_flt_asserted()) {
                    line0[15] = '\0';
                }

                line2[0] = 'F'; line2[1] = 'L'; line2[2] = 'T'; line2[3] = ':'; line2[4] = ' ';
                pos = 5U;
                for (uint32_t i = 0U; fault_tags_compact[i] != '\0' && pos + 1U < sizeof(line2); ++i) {
                    line2[pos++] = fault_tags_compact[i];
                }
                line2[pos] = '\0';

                if (!heartbeat_phase) {
                    line4[0] = 'O'; line4[1] = 'F'; line4[2] = 'F'; line4[3] = status_bit_char(u6_status, 6U);
                    line4[4] = ' '; line4[5] = 'O'; line4[6] = 'V'; line4[7] = status_bit_char(u6_status, 5U);
                    line4[8] = ' '; line4[9] = 'O'; line4[10] = 'C'; line4[11] = status_bit_char(u6_status, 4U);
                    line4[12] = ' '; line4[13] = 'U'; line4[14] = 'V'; line4[15] = status_bit_char(u6_status, 3U);
                    line4[16] = '\0';
                } else {
                    line4[0] = 'T'; line4[1] = 'M'; line4[2] = 'P'; line4[3] = status_bit_char(u6_status, 2U);
                    line4[4] = ' '; line4[5] = 'C'; line4[6] = 'M'; line4[7] = 'L'; line4[8] = status_bit_char(u6_status, 1U);
                    line4[9] = ' '; line4[10] = 'O'; line4[11] = 'T'; line4[12] = 'H'; line4[13] = status_bit_char(u6_status, 0U);
                    line4[14] = '\0';
                }

                line6[0] = 'V'; line6[1] = 'O'; line6[2] = '=';
                line6[3] = vout_str[0]; line6[4] = vout_str[1]; line6[5] = vout_str[2]; line6[6] = vout_str[3]; line6[7] = vout_str[4];
                line6[8] = ' '; line6[9] = 'V'; line6[10] = 'I'; line6[11] = '=';
                line6[12] = vin_str[0]; line6[13] = vin_str[1]; line6[14] = vin_str[2]; line6[15] = vin_str[3]; line6[16] = vin_str[4];
                line6[17] = '\0';

                (void)oled_clear();
                (void)oled_write_text_line(0U, line0);
                (void)oled_write_text_line(1U, line2);
                (void)oled_write_text_line(2U, line4);
                (void)oled_write_text_line(3U, line6);
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
        }

        if (!ctl.armed && ctl.run_ms_remaining == 0U) {
            if (g_esplog_enabled) {
                esplog_pump();
            } else {
                esplog_discard_pending();
                esplog_dropped = 0U;
            }
        }

        telemetry_tick += LOOP_PERIOD_MS;
        if (telemetry_tick >= TELEMETRY_PERIOD_MS) {
            telemetry_tick = 0U;
        }
        delay_ms(LOOP_PERIOD_MS);
    }
}
