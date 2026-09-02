/* Solar Project 200W Rev 0 stage-1 bring-up firmware. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define REG32(address) (*(volatile uint32_t *)(address))

#define RCC_BASE 0x40021000UL
#define RCC_AHB2ENR REG32(RCC_BASE + 0x4CUL)
#define RCC_APB2ENR REG32(RCC_BASE + 0x60UL)

#define CORE_DEBUG_DEMCR REG32(0xE000EDFCUL)
#define DWT_CTRL REG32(0xE0001000UL)
#define DWT_CYCCNT REG32(0xE0001004UL)

#define CORE_DEBUG_TRCENA (1UL << 24)
#define DWT_CYCCNTENA (1UL << 0)

#define GPIOA_BASE 0x48000000UL
#define GPIOB_BASE 0x48000400UL

#define GPIO_MODER(base) REG32((base) + 0x00UL)
#define GPIO_OTYPER(base) REG32((base) + 0x04UL)
#define GPIO_PUPDR(base) REG32((base) + 0x0CUL)
#define GPIO_IDR(base) REG32((base) + 0x10UL)
#define GPIO_ODR(base) REG32((base) + 0x14UL)
#define GPIO_BSRR(base) REG32((base) + 0x18UL)
#define GPIO_AFRH(base) REG32((base) + 0x24UL)

#define USART1_BASE 0x40013800UL
#define USART_CR1 REG32(USART1_BASE + 0x00UL)
#define USART_BRR REG32(USART1_BASE + 0x0CUL)
#define USART_ISR REG32(USART1_BASE + 0x1CUL)
#define USART_RDR REG32(USART1_BASE + 0x24UL)
#define USART_TDR REG32(USART1_BASE + 0x28UL)

#define PIN_0 (1UL << 0)
#define PIN_1 (1UL << 1)
#define PIN_2 (1UL << 2)
#define PIN_5 (1UL << 5)
#define PIN_6 (1UL << 6)
#define PIN_7 (1UL << 7)
#define PIN_8 (1UL << 8)

#define RX_BUFFER_LEN 64U

#define PROBE_MAGIC 0x53523030UL

#define LM51772_ADDRESS 0x6AU
#define LM51772_STATUS_BYTE 0x78U

struct bringup_probe {
    uint32_t magic;
    uint32_t version;
    uint32_t heartbeat;
    uint32_t gpioa_idr;
    uint32_t gpiob_idr;
    uint32_t gpioa_odr;
    uint32_t gpiob_odr;
    uint32_t last_command;
};

__attribute__((section(".bringup_probe")))
volatile struct bringup_probe g_probe;

static char rx_buffer[RX_BUFFER_LEN];
static size_t rx_len;
static bool rx_ready;
static uint8_t oled_buffer[1024];
static uint32_t uptime_seconds;
static uint32_t wall_seconds;
static uint32_t sample_count;
static uint32_t stability_errors;
static int32_t last_temperature_tenths;
static uint32_t last_humidity_tenths;
static uint8_t last_aht20_status;
static bool automatic_i2c_enabled = true;

static void delay_cycles(volatile uint32_t cycles)
{
    while (cycles-- > 0U) {
        __asm volatile ("nop");
    }
}

static void gpio_set_mode(uint32_t base, uint32_t pin_index, uint32_t mode)
{
    GPIO_MODER(base) = (GPIO_MODER(base) & ~(3UL << (pin_index * 2U))) | (mode << (pin_index * 2U));
}

static void gpio_set_pull(uint32_t base, uint32_t pin_index, uint32_t pull)
{
    GPIO_PUPDR(base) = (GPIO_PUPDR(base) & ~(3UL << (pin_index * 2U))) | (pull << (pin_index * 2U));
}

static void gpio_high(uint32_t base, uint32_t pin_mask)
{
    GPIO_BSRR(base) = pin_mask;
}

static void gpio_low(uint32_t base, uint32_t pin_mask)
{
    GPIO_BSRR(base) = pin_mask << 16U;
}

static bool gpio_read(uint32_t base, uint32_t pin_mask)
{
    return (GPIO_IDR(base) & pin_mask) != 0U;
}

static void safe_outputs(void)
{
    gpio_low(GPIOA_BASE, PIN_5 | PIN_8);
    gpio_low(GPIOB_BASE, PIN_2 | PIN_5);
}

static void gpio_init(void)
{
    RCC_AHB2ENR |= PIN_0 | PIN_1;
    (void)RCC_AHB2ENR;

    safe_outputs();

    gpio_set_mode(GPIOA_BASE, 5U, 1U);
    gpio_set_mode(GPIOA_BASE, 8U, 1U);
    gpio_set_mode(GPIOB_BASE, 2U, 1U);
    gpio_set_mode(GPIOB_BASE, 5U, 1U);

    gpio_set_mode(GPIOB_BASE, 0U, 0U);
    gpio_set_mode(GPIOB_BASE, 1U, 0U);

    gpio_high(GPIOB_BASE, PIN_6 | PIN_7);
    gpio_set_mode(GPIOB_BASE, 6U, 1U);
    gpio_set_mode(GPIOB_BASE, 7U, 1U);
    GPIO_OTYPER(GPIOB_BASE) |= PIN_6 | PIN_7;
    gpio_set_pull(GPIOB_BASE, 6U, 1U);
    gpio_set_pull(GPIOB_BASE, 7U, 1U);

    gpio_set_mode(GPIOA_BASE, 9U, 2U);
    gpio_set_mode(GPIOA_BASE, 10U, 2U);
    GPIO_AFRH(GPIOA_BASE) = (GPIO_AFRH(GPIOA_BASE) & ~((0xFUL << 4U) | (0xFUL << 8U))) |
                           (7UL << 4U) | (7UL << 8U);
}

static void uart_init(void)
{
    RCC_APB2ENR |= 1UL << 14;
    (void)RCC_APB2ENR;

    USART_CR1 = 0U;
    USART_BRR = 139U;
    USART_CR1 = (1UL << 0) | (1UL << 2) | (1UL << 3);
}

static void cycle_counter_init(void)
{
    CORE_DEBUG_DEMCR |= CORE_DEBUG_TRCENA;
    DWT_CYCCNT = 0U;
    DWT_CTRL |= DWT_CYCCNTENA;
}

static void wait_cycles(uint32_t start, uint32_t cycles)
{
    while ((uint32_t)(DWT_CYCCNT - start) < cycles) {
    }
}

__attribute__((naked, noinline)) static void d8_send_byte(uint8_t value __attribute__((unused)))
{
    __asm volatile(
        "ldr r1, =0x48000418\n"
        "movs r2, #4\n"
        "movs r3, #4\n"
        "lsls r3, r3, #16\n"
        "mov.w r12, #128\n"
        "1:\n"
        "str r2, [r1]\n"
        "tst r0, r12\n"
        "beq 2f\n"
        "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
        "str r3, [r1]\n"
        "nop\n nop\n"
        "lsrs r12, r12, #1\n"
        "bne 1b\n"
        "b 3f\n"
        "2:\n"
        "str r3, [r1]\n"
        "nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"
        "lsrs r12, r12, #1\n"
        "bne 1b\n"
        "3:\n"
        "bx lr\n");
}

static void d8_set(uint8_t red, uint8_t green, uint8_t blue)
{
    d8_send_byte(green);
    d8_send_byte(red);
    d8_send_byte(blue);
    gpio_low(GPIOB_BASE, PIN_2);

    const uint32_t reset_start = DWT_CYCCNT;
    wait_cycles(reset_start, 1600U);
}

static void uart_putc(char value)
{
    while ((USART_ISR & (1UL << 7)) == 0U) {
    }
    USART_TDR = (uint32_t)(uint8_t)value;
}

static void uart_write(const char *text)
{
    while (*text != '\0') {
        uart_putc(*text++);
    }
}

static void uart_line(const char *text)
{
    uart_write(text);
    uart_write("\r\n");
}

static bool uart_try_getc(char *value)
{
    if ((USART_ISR & (1UL << 5)) == 0U) {
        return false;
    }

    *value = (char)(USART_RDR & 0xFFU);
    return true;
}

static void uart_capture(void)
{
    char value;

    if (rx_ready || !uart_try_getc(&value)) {
        return;
    }
    if (value == '\r' || value == '\n') {
        if (rx_len > 0U) {
            rx_buffer[rx_len] = '\0';
            rx_ready = true;
        }
    } else if (rx_len < (RX_BUFFER_LEN - 1U)) {
        rx_buffer[rx_len++] = value;
    } else {
        rx_len = 0U;
    }
}

static bool streq(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        if (*left++ != *right++) {
            return false;
        }
    }

    return *left == '\0' && *right == '\0';
}

static bool starts_with(const char *text, const char *prefix)
{
    while (*prefix != '\0') {
        if (*text++ != *prefix++) {
            return false;
        }
    }
    return true;
}

static void write_hex_byte(uint8_t value)
{
    static const char hex[] = "0123456789ABCDEF";

    uart_putc(hex[(value >> 4U) & 0x0FU]);
    uart_putc(hex[value & 0x0FU]);
}

static void write_uint(uint32_t value)
{
    char digits[10];
    size_t count = 0U;

    do {
        digits[count++] = (char)('0' + (value % 10U));
        value /= 10U;
    } while (value != 0U);

    while (count > 0U) {
        uart_putc(digits[--count]);
    }
}

static void status(void)
{
    g_probe.gpioa_idr = GPIO_IDR(GPIOA_BASE);
    g_probe.gpiob_idr = GPIO_IDR(GPIOB_BASE);
    g_probe.gpioa_odr = GPIO_ODR(GPIOA_BASE);
    g_probe.gpiob_odr = GPIO_ODR(GPIOB_BASE);

    uart_write("STATUS OCP=");
    uart_putc(gpio_read(GPIOB_BASE, PIN_0) ? '1' : '0');
    uart_write(" OVP=");
    uart_putc(gpio_read(GPIOB_BASE, PIN_1) ? '1' : '0');
    uart_write(" PA8_PWM=");
    uart_putc((GPIO_ODR(GPIOA_BASE) & PIN_8) != 0U ? '1' : '0');
    uart_write(" PB5_EN=");
    uart_putc((GPIO_ODR(GPIOB_BASE) & PIN_5) != 0U ? '1' : '0');
    uart_write(" PA5_LEGACY=");
    uart_putc((GPIO_ODR(GPIOA_BASE) & PIN_5) != 0U ? '1' : '0');
    uart_write("\r\n");
}

static void help(void)
{
    uart_line("Commands:");
    uart_line("  ?            help");
    uart_line("  status       print faults and output latch states");
    uart_line("  fault        print FAULT_OCP/PB0 and FAULT_OVP/PB1");
    uart_line("  i2c quiet    stop automatic AHT20/OLED bus traffic");
    uart_line("  i2c resume   resume automatic AHT20/OLED bus traffic");
    uart_line("  i2cscan      software I2C scan on PB6=data, PB7=clock");
    uart_line("  i2cscan full report ACK/NACK for every address 0x08-0x77");
    uart_line("  i2cscan unknown repeat  scan 4x, excluding 0x38 and 0x3C");
    uart_line("  u6 probe     address-only probe of LM51772 at 0x6A");
    uart_line("  u6 probe repeat  send 16 address-only probes at 10 ms intervals");
    uart_line("  u6 status    read LM51772 STATUS_BYTE without clearing it");
    uart_line("  aht20        read temperature and humidity");
    uart_line("  oled test    initialize SSD1306 and draw checkerboard");
    uart_line("  clock HH:MM:SS  set dashboard time of day");
    uart_line("  d8 off       turn D8 off");
    uart_line("  d8 red       minimum-brightness red");
    uart_line("  d8 green     minimum-brightness green");
    uart_line("  d8 blue      minimum-brightness blue");
    uart_line("  safe         turn D8 off; force PA5/PA8/PB5 low");
    uart_line("  adc          not enabled in stage1");
}

static void i2c_delay(void)
{
    delay_cycles(120U);
    uart_capture();
}

static void i2c_start(void)
{
    gpio_high(GPIOB_BASE, PIN_6 | PIN_7);
    i2c_delay();
    gpio_low(GPIOB_BASE, PIN_6);
    i2c_delay();
    gpio_low(GPIOB_BASE, PIN_7);
}

static void i2c_stop(void)
{
    gpio_low(GPIOB_BASE, PIN_6);
    i2c_delay();
    gpio_high(GPIOB_BASE, PIN_7);
    i2c_delay();
    gpio_high(GPIOB_BASE, PIN_6);
    i2c_delay();
}

static bool i2c_write_byte(uint8_t value)
{
    for (uint8_t mask = 0x80U; mask != 0U; mask >>= 1U) {
        if ((value & mask) != 0U) {
            gpio_high(GPIOB_BASE, PIN_6);
        } else {
            gpio_low(GPIOB_BASE, PIN_6);
        }

        i2c_delay();
        gpio_high(GPIOB_BASE, PIN_7);
        i2c_delay();
        gpio_low(GPIOB_BASE, PIN_7);
    }

    gpio_high(GPIOB_BASE, PIN_6);
    i2c_delay();
    gpio_high(GPIOB_BASE, PIN_7);
    i2c_delay();
    const bool acked = !gpio_read(GPIOB_BASE, PIN_6);
    gpio_low(GPIOB_BASE, PIN_7);
    return acked;
}

static uint8_t i2c_read_byte(bool acknowledge)
{
    uint8_t value = 0U;

    gpio_high(GPIOB_BASE, PIN_6);
    for (uint8_t bit = 0U; bit < 8U; bit++) {
        value <<= 1U;
        i2c_delay();
        gpio_high(GPIOB_BASE, PIN_7);
        i2c_delay();
        if (gpio_read(GPIOB_BASE, PIN_6)) {
            value |= 1U;
        }
        gpio_low(GPIOB_BASE, PIN_7);
    }

    if (acknowledge) {
        gpio_low(GPIOB_BASE, PIN_6);
    }
    i2c_delay();
    gpio_high(GPIOB_BASE, PIN_7);
    i2c_delay();
    gpio_low(GPIOB_BASE, PIN_7);
    gpio_high(GPIOB_BASE, PIN_6);
    return value;
}

static bool i2c_write(uint8_t address, const uint8_t *data, size_t count)
{
    i2c_start();
    if (!i2c_write_byte((uint8_t)(address << 1U))) {
        i2c_stop();
        return false;
    }

    for (size_t index = 0U; index < count; index++) {
        if (!i2c_write_byte(data[index])) {
            i2c_stop();
            return false;
        }
    }

    i2c_stop();
    return true;
}

static bool i2c_read(uint8_t address, uint8_t *data, size_t count)
{
    i2c_start();
    if (!i2c_write_byte((uint8_t)((address << 1U) | 1U))) {
        i2c_stop();
        return false;
    }

    for (size_t index = 0U; index < count; index++) {
        data[index] = i2c_read_byte(index + 1U < count);
    }

    i2c_stop();
    return true;
}

static bool i2c_read_register(uint8_t address, uint8_t register_address, uint8_t *value)
{
    i2c_start();
    if (!i2c_write_byte((uint8_t)(address << 1U)) || !i2c_write_byte(register_address)) {
        i2c_stop();
        return false;
    }

    i2c_start();
    if (!i2c_write_byte((uint8_t)((address << 1U) | 1U))) {
        i2c_stop();
        return false;
    }

    *value = i2c_read_byte(false);
    i2c_stop();
    return true;
}

static void lm51772_status(void)
{
    uint8_t value;

    if (!i2c_read_register(LM51772_ADDRESS, LM51772_STATUS_BYTE, &value)) {
        uart_line("U6 STATUS_BYTE read failed at I2C address 0x6A");
        return;
    }

    uart_write("U6 STATUS_BYTE=0x");
    write_hex_byte(value);
    uart_write(" BUSY=");
    uart_putc((value & 0x80U) != 0U ? '1' : '0');
    uart_write(" OFF=");
    uart_putc((value & 0x40U) != 0U ? '1' : '0');
    uart_write(" VOUT=");
    uart_putc((value & 0x20U) != 0U ? '1' : '0');
    uart_write(" IOUT=");
    uart_putc((value & 0x10U) != 0U ? '1' : '0');
    uart_write(" INPUT=");
    uart_putc((value & 0x08U) != 0U ? '1' : '0');
    uart_write(" TEMP=");
    uart_putc((value & 0x04U) != 0U ? '1' : '0');
    uart_write(" CML=");
    uart_putc((value & 0x02U) != 0U ? '1' : '0');
    uart_write(" OTHER=");
    uart_putc((value & 0x01U) != 0U ? '1' : '0');
    uart_write("\r\n");
}

static bool aht20_measure(void)
{
    static const uint8_t trigger[] = {0xACU, 0x33U, 0x00U};
    uint8_t data[6];

    if (!i2c_write(0x38U, trigger, sizeof(trigger))) {
        return false;
    }

    const uint32_t conversion_start = DWT_CYCCNT;
    wait_cycles(conversion_start, 1600000U);

    if (!i2c_read(0x38U, data, sizeof(data))) {
        return false;
    }
    if ((data[0] & 0x80U) != 0U) {
        return false;
    }

    const uint32_t humidity_raw = ((uint32_t)data[1] << 12U) |
                                  ((uint32_t)data[2] << 4U) |
                                  ((uint32_t)data[3] >> 4U);
    const uint32_t temperature_raw = ((uint32_t)(data[3] & 0x0FU) << 16U) |
                                     ((uint32_t)data[4] << 8U) |
                                     (uint32_t)data[5];
    last_humidity_tenths = (humidity_raw * 1000U) >> 20U;
    last_temperature_tenths = (int32_t)((temperature_raw * 2000U) >> 20U) - 500;
    last_aht20_status = data[0];
    return true;
}

static void aht20_read(void)
{
    if (!aht20_measure()) {
        stability_errors++;
        uart_line("AHT20 measurement failed");
        return;
    }
    sample_count++;

    uart_write("AHT20 status=0x");
    write_hex_byte(last_aht20_status);
    uart_write(" temperature=");
    if (last_temperature_tenths < 0) {
        uart_putc('-');
        write_uint((uint32_t)(-last_temperature_tenths) / 10U);
        uart_putc('.');
        write_uint((uint32_t)(-last_temperature_tenths) % 10U);
    } else {
        write_uint((uint32_t)last_temperature_tenths / 10U);
        uart_putc('.');
        write_uint((uint32_t)last_temperature_tenths % 10U);
    }
    uart_write("C humidity=");
    write_uint(last_humidity_tenths / 10U);
    uart_putc('.');
    write_uint(last_humidity_tenths % 10U);
    uart_line("%RH");
}

static bool ssd1306_write(uint8_t control, const uint8_t *data, size_t count)
{
    i2c_start();
    if (!i2c_write_byte(0x3CU << 1U) || !i2c_write_byte(control)) {
        i2c_stop();
        return false;
    }

    for (size_t index = 0U; index < count; index++) {
        if (!i2c_write_byte(data[index])) {
            i2c_stop();
            return false;
        }
    }

    i2c_stop();
    return true;
}

static bool ssd1306_init(void)
{
    static const uint8_t init[] = {
        0xAEU, 0xD5U, 0x80U, 0xA8U, 0x3FU, 0xD3U, 0x00U, 0x40U,
        0x8DU, 0x14U, 0x20U, 0x00U, 0xA1U, 0xC8U, 0xDAU, 0x12U,
        0x81U, 0x20U, 0xD9U, 0xF1U, 0xDBU, 0x40U, 0xA4U, 0xA6U,
        0xAFU
    };

    return ssd1306_write(0x00U, init, sizeof(init));
}

#define GLYPH(row0, row1, row2, row3, row4) \
    ((uint16_t)(row0) | ((uint16_t)(row1) << 3U) | ((uint16_t)(row2) << 6U) | \
     ((uint16_t)(row3) << 9U) | ((uint16_t)(row4) << 12U))

static uint16_t glyph(char value)
{
    switch (value) {
    case '0': return GLYPH(7, 5, 5, 5, 7);
    case '1': return GLYPH(2, 6, 2, 2, 7);
    case '2': return GLYPH(7, 1, 7, 4, 7);
    case '3': return GLYPH(7, 1, 7, 1, 7);
    case '4': return GLYPH(5, 5, 7, 1, 1);
    case '5': return GLYPH(7, 4, 7, 1, 7);
    case '6': return GLYPH(7, 4, 7, 5, 7);
    case '7': return GLYPH(7, 1, 2, 2, 2);
    case '8': return GLYPH(7, 5, 7, 5, 7);
    case '9': return GLYPH(7, 5, 7, 1, 7);
    case 'A': return GLYPH(2, 5, 7, 5, 5);
    case 'C': return GLYPH(7, 4, 4, 4, 7);
    case 'E': return GLYPH(7, 4, 6, 4, 7);
    case 'H': return GLYPH(5, 5, 7, 5, 5);
    case 'I': return GLYPH(7, 2, 2, 2, 7);
    case 'M': return GLYPH(5, 7, 7, 5, 5);
    case 'N': return GLYPH(5, 7, 7, 7, 5);
    case 'P': return GLYPH(6, 5, 6, 4, 4);
    case 'R': return GLYPH(6, 5, 6, 5, 5);
    case 'S': return GLYPH(7, 4, 7, 1, 7);
    case 'T': return GLYPH(7, 2, 2, 2, 2);
    case 'U': return GLYPH(5, 5, 5, 5, 7);
    case ':': return GLYPH(0, 2, 0, 2, 0);
    case '.': return GLYPH(0, 0, 0, 0, 2);
    case '%': return GLYPH(5, 1, 2, 4, 5);
    case '-': return GLYPH(0, 0, 7, 0, 0);
    default: return 0U;
    }
}

static void oled_clear(void)
{
    for (size_t index = 0U; index < sizeof(oled_buffer); index++) {
        oled_buffer[index] = 0U;
    }
}

static void oled_pixel(uint32_t x, uint32_t y)
{
    if (x < 128U && y < 64U) {
        oled_buffer[x + ((y >> 3U) * 128U)] |= (uint8_t)(1U << (y & 7U));
    }
}

static void oled_text(uint32_t x, uint32_t y, const char *text)
{
    while (*text != '\0' && x <= 120U) {
        const uint16_t pattern = glyph(*text++);
        for (uint32_t row = 0U; row < 5U; row++) {
            for (uint32_t column = 0U; column < 3U; column++) {
                if ((pattern & (1U << ((row * 3U) + (2U - column)))) != 0U) {
                    oled_pixel(x + (column * 2U), y + (row * 2U));
                    oled_pixel(x + (column * 2U) + 1U, y + (row * 2U));
                    oled_pixel(x + (column * 2U), y + (row * 2U) + 1U);
                    oled_pixel(x + (column * 2U) + 1U, y + (row * 2U) + 1U);
                }
            }
        }
        x += 8U;
    }
}

static void two_digits(char *output, uint32_t value)
{
    output[0] = (char)('0' + ((value / 10U) % 10U));
    output[1] = (char)('0' + (value % 10U));
}

static bool oled_dashboard(void)
{
    static const uint8_t address[] = {0x21U, 0x00U, 0x7FU, 0x22U, 0x00U, 0x07U};
    char time_line[] = "TIME 00:00:00";
    char uptime_line[] = "UP 000:00:00";
    char sensor_line[] = "T 00.0C H000.0%";
    char health_line[] = "N00000 E000";
    const uint32_t hours = (wall_seconds / 3600U) % 24U;
    const uint32_t minutes = (wall_seconds / 60U) % 60U;
    const uint32_t seconds = wall_seconds % 60U;
    const uint32_t uptime_hours = (uptime_seconds / 3600U) % 1000U;
    const uint32_t uptime_minutes = (uptime_seconds / 60U) % 60U;
    const uint32_t uptime_display_seconds = uptime_seconds % 60U;
    uint32_t temperature_magnitude;

    two_digits(&time_line[5], hours);
    two_digits(&time_line[8], minutes);
    two_digits(&time_line[11], seconds);
    uptime_line[3] = (char)('0' + ((uptime_hours / 100U) % 10U));
    uptime_line[4] = (char)('0' + ((uptime_hours / 10U) % 10U));
    uptime_line[5] = (char)('0' + (uptime_hours % 10U));
    two_digits(&uptime_line[7], uptime_minutes);
    two_digits(&uptime_line[10], uptime_display_seconds);

    if (last_temperature_tenths < 0) {
        sensor_line[1] = '-';
        temperature_magnitude = (uint32_t)(-last_temperature_tenths);
    } else {
        temperature_magnitude = (uint32_t)last_temperature_tenths;
    }
    two_digits(&sensor_line[2], temperature_magnitude / 10U);
    sensor_line[5] = (char)('0' + (temperature_magnitude % 10U));
    sensor_line[9] = (char)('0' + ((last_humidity_tenths / 1000U) % 10U));
    sensor_line[10] = (char)('0' + ((last_humidity_tenths / 100U) % 10U));
    sensor_line[11] = (char)('0' + ((last_humidity_tenths / 10U) % 10U));
    sensor_line[13] = (char)('0' + (last_humidity_tenths % 10U));

    for (uint32_t place = 0U, divisor = 10000U; place < 5U; place++, divisor /= 10U) {
        health_line[1U + place] = (char)('0' + ((sample_count / divisor) % 10U));
    }
    health_line[8] = (char)('0' + ((stability_errors / 100U) % 10U));
    health_line[9] = (char)('0' + ((stability_errors / 10U) % 10U));
    health_line[10] = (char)('0' + (stability_errors % 10U));

    oled_clear();
    oled_text(0U, 0U, time_line);
    oled_text(0U, 14U, uptime_line);
    oled_text(0U, 28U, sensor_line);
    oled_text(0U, 42U, health_line);

    if (!ssd1306_write(0x00U, address, sizeof(address))) {
        return false;
    }
    for (size_t offset = 0U; offset < sizeof(oled_buffer); offset += 16U) {
        if (!ssd1306_write(0x40U, &oled_buffer[offset], 16U)) {
            return false;
        }
    }
    return true;
}

static void ssd1306_test(void)
{
    static const uint8_t address[] = {0x21U, 0x00U, 0x7FU, 0x22U, 0x00U, 0x07U};
    uint8_t pixels[16];

    if (!ssd1306_init() || !ssd1306_write(0x00U, address, sizeof(address))) {
        uart_line("SSD1306 init failed");
        return;
    }

    for (uint32_t offset = 0U; offset < 1024U; offset += sizeof(pixels)) {
        for (size_t index = 0U; index < sizeof(pixels); index++) {
            pixels[index] = (((offset + index) & 1U) == 0U) ? 0xAAU : 0x55U;
        }
        if (!ssd1306_write(0x40U, pixels, sizeof(pixels))) {
            uart_line("SSD1306 data failed");
            return;
        }
    }

    uart_line("SSD1306 checkerboard drawn");
}

static bool set_clock(const char *text)
{
    if (text[0] < '0' || text[0] > '9' || text[1] < '0' || text[1] > '9' ||
        text[2] != ':' || text[3] < '0' || text[3] > '9' ||
        text[4] < '0' || text[4] > '9' || text[5] != ':' ||
        text[6] < '0' || text[6] > '9' || text[7] < '0' || text[7] > '9' ||
        text[8] != '\0') {
        return false;
    }

    const uint32_t hours = (uint32_t)(text[0] - '0') * 10U + (uint32_t)(text[1] - '0');
    const uint32_t minutes = (uint32_t)(text[3] - '0') * 10U + (uint32_t)(text[4] - '0');
    const uint32_t seconds = (uint32_t)(text[6] - '0') * 10U + (uint32_t)(text[7] - '0');
    if (hours > 23U || minutes > 59U || seconds > 59U) {
        return false;
    }

    wall_seconds = (hours * 3600U) + (minutes * 60U) + seconds;
    return true;
}

static void i2c_scan(bool report_all)
{
    uint8_t found = 0U;

    uart_line(report_all ? "I2C full scan start 0x08-0x77" : "I2C scan start");
    for (uint8_t address = 0x08U; address <= 0x77U; address++) {
        i2c_start();
        const bool acked = i2c_write_byte((uint8_t)(address << 1U));
        i2c_stop();

        if (report_all || acked) {
            uart_write("I2C 0x");
            write_hex_byte(address);
            uart_line(acked ? " ACK" : " NACK");
        }
        if (acked) {
            found++;
        }
    }

    uart_write("I2C scan done, found ");
    if (found >= 10U) {
        uart_putc((char)('0' + (found / 10U)));
    }
    uart_putc((char)('0' + (found % 10U)));
    uart_write("\r\n");
}

static void i2c_scan_unknown_repeat(void)
{
    uint16_t acknowledged = 0U;

    automatic_i2c_enabled = false;
    uart_line("I2C unknown scan start: 4 sweeps, excluding 0x38 and 0x3C");
    for (uint8_t sweep = 0U; sweep < 4U; sweep++) {
        for (uint8_t address = 0x08U; address <= 0x77U; address++) {
            if (address == 0x38U || address == 0x3CU) {
                continue;
            }

            i2c_start();
            const bool acked = i2c_write_byte((uint8_t)(address << 1U));
            i2c_stop();
            if (acked) {
                uart_write("Unexpected I2C ACK sweep=");
                write_uint((uint32_t)sweep + 1U);
                uart_write(" address=0x");
                write_hex_byte(address);
                uart_write("\r\n");
                acknowledged++;
            }
        }
    }

    uart_write("I2C unknown scan done probes=440 ACKs=");
    write_uint(acknowledged);
    uart_write("\r\n");
}

static bool lm51772_probe_once(void)
{
    i2c_start();
    const bool acked = i2c_write_byte(LM51772_ADDRESS << 1U);
    i2c_stop();
    return acked;
}

static void lm51772_probe(void)
{
    const bool acked = lm51772_probe_once();
    uart_line(acked ? "U6 0x6A ACK" : "U6 0x6A NACK");
}

static void lm51772_probe_repeat(void)
{
    uint8_t acknowledged = 0U;

    for (uint8_t probe = 0U; probe < 16U; probe++) {
        if (lm51772_probe_once()) {
            acknowledged++;
        }
        if (probe < 15U) {
            const uint32_t gap_start = DWT_CYCCNT;
            wait_cycles(gap_start, 160000U);
        }
    }

    uart_write("U6 repeated probes=16 ACKs=");
    write_uint(acknowledged);
    uart_write(" NACKs=");
    write_uint(16U - acknowledged);
    uart_write("\r\n");
}

static void command(const char *text)
{
    if (streq(text, "?") || streq(text, "help")) {
        g_probe.last_command = 0x3FUL;
        help();
    } else if (streq(text, "status") || streq(text, "fault")) {
        g_probe.last_command = 0x53544154UL;
        status();
    } else if (streq(text, "i2c quiet")) {
        g_probe.last_command = 0x49325154UL;
        automatic_i2c_enabled = false;
        gpio_high(GPIOB_BASE, PIN_6 | PIN_7);
        uart_line("Automatic I2C traffic stopped");
    } else if (streq(text, "i2c resume")) {
        g_probe.last_command = 0x49325253UL;
        automatic_i2c_enabled = true;
        uart_line("Automatic I2C traffic resumed");
    } else if (streq(text, "i2cscan")) {
        g_probe.last_command = 0x493243UL;
        i2c_scan(false);
    } else if (streq(text, "i2cscan full")) {
        g_probe.last_command = 0x49324655UL;
        i2c_scan(true);
    } else if (streq(text, "i2cscan unknown repeat")) {
        g_probe.last_command = 0x4932554EUL;
        i2c_scan_unknown_repeat();
    } else if (streq(text, "u6 probe")) {
        g_probe.last_command = 0x55365052UL;
        lm51772_probe();
    } else if (streq(text, "u6 probe repeat")) {
        g_probe.last_command = 0x55365250UL;
        lm51772_probe_repeat();
    } else if (streq(text, "u6 status")) {
        g_probe.last_command = 0x55365354UL;
        lm51772_status();
    } else if (streq(text, "aht20")) {
        g_probe.last_command = 0x41485432UL;
        aht20_read();
    } else if (streq(text, "oled test")) {
        g_probe.last_command = 0x4F4C4544UL;
        ssd1306_test();
    } else if (starts_with(text, "clock ")) {
        g_probe.last_command = 0x434C4F43UL;
        uart_line(set_clock(&text[6]) ? "Clock set" : "ERR use clock HH:MM:SS");
    } else if (streq(text, "d8 off")) {
        g_probe.last_command = 0x44384F46UL;
        d8_set(0U, 0U, 0U);
        uart_line("D8 off");
    } else if (streq(text, "d8 red")) {
        g_probe.last_command = 0x44385244UL;
        d8_set(1U, 0U, 0U);
        uart_line("D8 red level 1");
    } else if (streq(text, "d8 green")) {
        g_probe.last_command = 0x4438474EUL;
        d8_set(0U, 1U, 0U);
        uart_line("D8 green level 1");
    } else if (streq(text, "d8 blue")) {
        g_probe.last_command = 0x4438424CUL;
        d8_set(0U, 0U, 1U);
        uart_line("D8 blue level 1");
    } else if (streq(text, "safe")) {
        g_probe.last_command = 0x53414645UL;
        safe_outputs();
        d8_set(0U, 0U, 0U);
        status();
    } else if (streq(text, "adc")) {
        g_probe.last_command = 0x414443UL;
        uart_line("ADC not enabled in bare-metal stage1");
    } else {
        g_probe.last_command = 0x455252UL;
        uart_line("ERR unknown command");
    }
}

int main(void)
{
    const uint32_t heartbeat_on_cycles = 4000000U;
    const uint32_t heartbeat_off_cycles = 12000000U;
    uint32_t d8_change_at;
    uint32_t time_last_cycle;
    uint32_t second_cycle_accumulator = 0U;
    uint32_t last_sample_second = 0xFFFFFFFFU;
    uint32_t last_display_second = 0xFFFFFFFFU;
    bool d8_on = false;
    uint8_t d8_color = 0U;

    gpio_init();
    safe_outputs();
    cycle_counter_init();
    uart_init();
    d8_set(0U, 0U, 0U);
    d8_change_at = DWT_CYCCNT;
    time_last_cycle = DWT_CYCCNT;

    g_probe.magic = PROBE_MAGIC;
    g_probe.version = 2U;
    g_probe.heartbeat = 0U;
    g_probe.last_command = 0U;

    uart_line("");
    uart_line("Solar Rev0 STM32G47x stage1 ready");
    uart_line("USART1 115200 8N1; type ? for commands");
    status();

    if (!ssd1306_init()) {
        stability_errors++;
    }

    for (;;) {
        char command_buffer[RX_BUFFER_LEN];

        g_probe.heartbeat++;
        g_probe.gpioa_idr = GPIO_IDR(GPIOA_BASE);
        g_probe.gpiob_idr = GPIO_IDR(GPIOB_BASE);
        g_probe.gpioa_odr = GPIO_ODR(GPIOA_BASE);
        g_probe.gpiob_odr = GPIO_ODR(GPIOB_BASE);

        const uint32_t now = DWT_CYCCNT;
        second_cycle_accumulator += (uint32_t)(now - time_last_cycle);
        time_last_cycle = now;
        while (second_cycle_accumulator >= 16000000U) {
            second_cycle_accumulator -= 16000000U;
            uptime_seconds++;
            wall_seconds = (wall_seconds + 1U) % 86400U;
        }

        if (automatic_i2c_enabled &&
            (uptime_seconds % 10U) == 0U && uptime_seconds != last_sample_second) {
            if (aht20_measure()) {
                sample_count++;
            } else {
                stability_errors++;
            }
            last_sample_second = uptime_seconds;
        }
        if (automatic_i2c_enabled && uptime_seconds != last_display_second) {
            if (!oled_dashboard()) {
                stability_errors++;
            }
            last_display_second = uptime_seconds;
        }

        const uint32_t d8_interval = d8_on ? heartbeat_on_cycles : heartbeat_off_cycles;
        if ((uint32_t)(DWT_CYCCNT - d8_change_at) >= d8_interval) {
            d8_on = !d8_on;
            if (!d8_on) {
                d8_set(0U, 0U, 0U);
                d8_color = (uint8_t)((d8_color + 1U) % 3U);
            } else if (d8_color == 0U) {
                d8_set(1U, 0U, 0U);
            } else if (d8_color == 1U) {
                d8_set(0U, 1U, 0U);
            } else {
                d8_set(0U, 0U, 1U);
            }
            d8_change_at = DWT_CYCCNT;
        }

        uart_capture();
        if (rx_ready) {
            size_t index = 0U;
            do {
                command_buffer[index] = rx_buffer[index];
            } while (rx_buffer[index++] != '\0');
            rx_len = 0U;
            rx_ready = false;
            command(command_buffer);
        }
    }
}
