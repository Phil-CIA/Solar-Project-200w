/*
 * Solar Project 200W Rev 0 low-energy bring-up firmware.
 *
 * Target family: STM32G431/STM32G473 class, UFQFPN-48 Rev 0 recovered boards.
 * Framework: STM32Cube HAL. Drop this file into a CubeMX-generated STM32G4
 * project with HAL drivers enabled, or use it as the pin-behavior reference for
 * an equivalent project.
 */

#include "main.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart1;

#define CTRL_PWM_MAIN_PORT GPIOA
#define CTRL_PWM_MAIN_PIN GPIO_PIN_8
#define CTRL_EN_CHG_PORT GPIOB
#define CTRL_EN_CHG_PIN GPIO_PIN_5
#define LEGACY_V_IN_ON_PORT GPIOA
#define LEGACY_V_IN_ON_PIN GPIO_PIN_5

#define FAULT_OCP_PORT GPIOB
#define FAULT_OCP_PIN GPIO_PIN_0
#define FAULT_OVP_PORT GPIOB
#define FAULT_OVP_PIN GPIO_PIN_1

#define SW_I2C_DATA_PORT GPIOB
#define SW_I2C_DATA_PIN GPIO_PIN_6
#define SW_I2C_CLK_PORT GPIOB
#define SW_I2C_CLK_PIN GPIO_PIN_7

#define UART_RX_BUFFER_LEN 64u

static char uart_rx_buffer[UART_RX_BUFFER_LEN];
static size_t uart_rx_len;

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void Error_Handler(void);

static void safe_outputs(void);
static void print_line(const char *text);
static void print_help(void);
static void print_status(void);
static void print_faults(void);
static void print_adc(void);
static void process_command(char *command);
static uint16_t read_adc_channel(uint32_t channel);
static void pulse_line(GPIO_TypeDef *port, uint16_t pin, uint32_t high_ms);

static void i2c_delay(void);
static void i2c_release(GPIO_TypeDef *port, uint16_t pin);
static void i2c_drive_low(GPIO_TypeDef *port, uint16_t pin);
static GPIO_PinState i2c_read_data(void);
static void i2c_start(void);
static void i2c_stop(void);
static bool i2c_write_byte(uint8_t value);
static void i2c_scan(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    safe_outputs();
    MX_ADC1_Init();
    MX_USART1_UART_Init();

    print_line("");
    print_line("Solar Rev0 STM32G4 safe bring-up ready");
    print_line("USART1 115200 8N1; type ? for commands");
    print_status();

    for (;;) {
        uint8_t byte = 0u;

        if (HAL_UART_Receive(&huart1, &byte, 1u, 20u) != HAL_OK) {
            continue;
        }

        if (byte == '\r' || byte == '\n') {
            if (uart_rx_len > 0u) {
                uart_rx_buffer[uart_rx_len] = '\0';
                process_command(uart_rx_buffer);
                uart_rx_len = 0u;
            }
        } else if (uart_rx_len < (UART_RX_BUFFER_LEN - 1u)) {
            uart_rx_buffer[uart_rx_len++] = (char)byte;
        } else {
            uart_rx_len = 0u;
            print_line("ERR line too long");
        }
    }
}

static void safe_outputs(void)
{
    HAL_GPIO_WritePin(CTRL_PWM_MAIN_PORT, CTRL_PWM_MAIN_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CTRL_EN_CHG_PORT, CTRL_EN_CHG_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEGACY_V_IN_ON_PORT, LEGACY_V_IN_ON_PIN, GPIO_PIN_RESET);
}

static void print_line(const char *text)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)text, (uint16_t)strlen(text), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n", 2u, HAL_MAX_DELAY);
}

static void print_help(void)
{
    print_line("Commands:");
    print_line("  ?            help");
    print_line("  status       print faults and output latch states");
    print_line("  fault        print FAULT_OCP/PB0 and FAULT_OVP/PB1");
    print_line("  adc          print raw PA4/PA6/PA7 ADC values");
    print_line("  i2cscan      software I2C scan on PB6=data, PB7=clock");
    print_line("  pulse en     pulse CTRL_EN_CHG/PB5 for 100 ms");
    print_line("  pulse legacy pulse V_in_ON/PA5 for 100 ms");
    print_line("  pulse pwm    pulse CTRL_PWM_MAIN/PA8 for 10 ms, scope-only");
    print_line("  safe         force all control outputs low");
}

static void print_status(void)
{
    char line[160];

    snprintf(line, sizeof(line),
             "STATUS OCP=%u OVP=%u PA8_PWM=%u PB5_EN=%u PA5_LEGACY=%u",
             HAL_GPIO_ReadPin(FAULT_OCP_PORT, FAULT_OCP_PIN) == GPIO_PIN_SET ? 1u : 0u,
             HAL_GPIO_ReadPin(FAULT_OVP_PORT, FAULT_OVP_PIN) == GPIO_PIN_SET ? 1u : 0u,
             HAL_GPIO_ReadPin(CTRL_PWM_MAIN_PORT, CTRL_PWM_MAIN_PIN) == GPIO_PIN_SET ? 1u : 0u,
             HAL_GPIO_ReadPin(CTRL_EN_CHG_PORT, CTRL_EN_CHG_PIN) == GPIO_PIN_SET ? 1u : 0u,
             HAL_GPIO_ReadPin(LEGACY_V_IN_ON_PORT, LEGACY_V_IN_ON_PIN) == GPIO_PIN_SET ? 1u : 0u);
    print_line(line);
}

static void print_faults(void)
{
    char line[80];

    snprintf(line, sizeof(line), "FAULT OCP=%u OVP=%u",
             HAL_GPIO_ReadPin(FAULT_OCP_PORT, FAULT_OCP_PIN) == GPIO_PIN_SET ? 1u : 0u,
             HAL_GPIO_ReadPin(FAULT_OVP_PORT, FAULT_OVP_PIN) == GPIO_PIN_SET ? 1u : 0u);
    print_line(line);
}

static void print_adc(void)
{
    char line[128];
    const uint16_t pv_v = read_adc_channel(ADC_CHANNEL_4);
    const uint16_t temp_pwr = read_adc_channel(ADC_CHANNEL_6);
    const uint16_t temp_board = read_adc_channel(ADC_CHANNEL_7);

    snprintf(line, sizeof(line), "ADC PA4_SENSE_PV_V=%u PA6_TEMP_PWR=%u PA7_TEMP_BOARD=%u",
             pv_v, temp_pwr, temp_board);
    print_line(line);
}

static void process_command(char *command)
{
    if (strcmp(command, "?") == 0 || strcmp(command, "help") == 0) {
        print_help();
    } else if (strcmp(command, "status") == 0) {
        print_status();
    } else if (strcmp(command, "fault") == 0) {
        print_faults();
    } else if (strcmp(command, "adc") == 0) {
        print_adc();
    } else if (strcmp(command, "i2cscan") == 0) {
        i2c_scan();
    } else if (strcmp(command, "pulse en") == 0) {
        pulse_line(CTRL_EN_CHG_PORT, CTRL_EN_CHG_PIN, 100u);
        print_status();
    } else if (strcmp(command, "pulse legacy") == 0) {
        pulse_line(LEGACY_V_IN_ON_PORT, LEGACY_V_IN_ON_PIN, 100u);
        print_status();
    } else if (strcmp(command, "pulse pwm") == 0) {
        pulse_line(CTRL_PWM_MAIN_PORT, CTRL_PWM_MAIN_PIN, 10u);
        print_status();
    } else if (strcmp(command, "safe") == 0) {
        safe_outputs();
        print_status();
    } else {
        print_line("ERR unknown command");
    }
}

static uint16_t read_adc_channel(uint32_t channel)
{
    ADC_ChannelConfTypeDef config = {0};

    config.Channel = channel;
    config.Rank = ADC_REGULAR_RANK_1;
    config.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
    config.SingleDiff = ADC_SINGLE_ENDED;
    config.OffsetNumber = ADC_OFFSET_NONE;
    config.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc1, &config) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_PollForConversion(&hadc1, 10u) != HAL_OK) {
        HAL_ADC_Stop(&hadc1);
        return 0u;
    }

    const uint16_t value = (uint16_t)HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return value;
}

static void pulse_line(GPIO_TypeDef *port, uint16_t pin, uint32_t high_ms)
{
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    HAL_Delay(high_ms);
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

static void i2c_delay(void)
{
    for (volatile uint32_t count = 0u; count < 80u; count++) {
        __NOP();
    }
}

static void i2c_release(GPIO_TypeDef *port, uint16_t pin)
{
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

static void i2c_drive_low(GPIO_TypeDef *port, uint16_t pin)
{
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

static GPIO_PinState i2c_read_data(void)
{
    return HAL_GPIO_ReadPin(SW_I2C_DATA_PORT, SW_I2C_DATA_PIN);
}

static void i2c_start(void)
{
    i2c_release(SW_I2C_DATA_PORT, SW_I2C_DATA_PIN);
    i2c_release(SW_I2C_CLK_PORT, SW_I2C_CLK_PIN);
    i2c_delay();
    i2c_drive_low(SW_I2C_DATA_PORT, SW_I2C_DATA_PIN);
    i2c_delay();
    i2c_drive_low(SW_I2C_CLK_PORT, SW_I2C_CLK_PIN);
}

static void i2c_stop(void)
{
    i2c_drive_low(SW_I2C_DATA_PORT, SW_I2C_DATA_PIN);
    i2c_delay();
    i2c_release(SW_I2C_CLK_PORT, SW_I2C_CLK_PIN);
    i2c_delay();
    i2c_release(SW_I2C_DATA_PORT, SW_I2C_DATA_PIN);
    i2c_delay();
}

static bool i2c_write_byte(uint8_t value)
{
    for (uint8_t mask = 0x80u; mask != 0u; mask >>= 1u) {
        if ((value & mask) != 0u) {
            i2c_release(SW_I2C_DATA_PORT, SW_I2C_DATA_PIN);
        } else {
            i2c_drive_low(SW_I2C_DATA_PORT, SW_I2C_DATA_PIN);
        }

        i2c_delay();
        i2c_release(SW_I2C_CLK_PORT, SW_I2C_CLK_PIN);
        i2c_delay();
        i2c_drive_low(SW_I2C_CLK_PORT, SW_I2C_CLK_PIN);
    }

    i2c_release(SW_I2C_DATA_PORT, SW_I2C_DATA_PIN);
    i2c_delay();
    i2c_release(SW_I2C_CLK_PORT, SW_I2C_CLK_PIN);
    i2c_delay();
    const bool acked = i2c_read_data() == GPIO_PIN_RESET;
    i2c_drive_low(SW_I2C_CLK_PORT, SW_I2C_CLK_PIN);
    return acked;
}

static void i2c_scan(void)
{
    char line[64];
    uint8_t found = 0u;

    print_line("I2C scan start");

    for (uint8_t address = 0x08u; address <= 0x77u; address++) {
        i2c_start();
        const bool acked = i2c_write_byte((uint8_t)(address << 1u));
        i2c_stop();

        if (acked) {
            snprintf(line, sizeof(line), "I2C device 0x%02X", address);
            print_line(line);
            found++;
        }
    }

    snprintf(line, sizeof(line), "I2C scan done, found %u", found);
    print_line(line);
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef oscillator = {0};
    RCC_ClkInitTypeDef clock = {0};

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    oscillator.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    oscillator.HSIState = RCC_HSI_ON;
    oscillator.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    oscillator.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&oscillator) != HAL_OK) {
        Error_Handler();
    }

    clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clock.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock.APB1CLKDivider = RCC_HCLK_DIV1;
    clock.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_ADC1_Init(void)
{
    __HAL_RCC_ADC12_CLK_ENABLE();

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.GainCompensation = 0;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.OversamplingMode = DISABLE;

    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_USART1_UART_Init(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOA, CTRL_PWM_MAIN_PIN | LEGACY_V_IN_ON_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, CTRL_EN_CHG_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, SW_I2C_DATA_PIN | SW_I2C_CLK_PIN, GPIO_PIN_SET);

    gpio.Pin = CTRL_PWM_MAIN_PIN | LEGACY_V_IN_ON_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = CTRL_EN_CHG_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpio);

    gpio.Pin = FAULT_OCP_PIN | FAULT_OVP_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &gpio);

    gpio.Pin = SW_I2C_DATA_PIN | SW_I2C_CLK_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpio);

    gpio.Pin = GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &gpio);
}

static void Error_Handler(void)
{
    safe_outputs();
    __disable_irq();
    for (;;) {
    }
}
