.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

.global g_pfnVectors
.global Reset_Handler
.extern main

.section .isr_vector,"a",%progbits
g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word Default_Handler
    .word Default_Handler

.section .text.Reset_Handler,"ax",%progbits
.type Reset_Handler, %function
.thumb_func
Reset_Handler:
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
copy_data:
    cmp r1, r2
    bcc copy_data_word
    b zero_bss
copy_data_word:
    ldr r3, [r0], #4
    str r3, [r1], #4
    b copy_data
zero_bss:
    ldr r1, =_sbss
    ldr r2, =_ebss
zero_bss_loop:
    cmp r1, r2
    bcc zero_bss_word
    b call_main
zero_bss_word:
    movs r3, #0
    str r3, [r1], #4
    b zero_bss_loop
call_main:
    bl main
    b .

.section .text.Default_Handler,"ax",%progbits
.type Default_Handler, %function
.thumb_func
Default_Handler:
    b .
