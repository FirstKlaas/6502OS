; ****************************************************************************
; Author : Klaas Nebuhr
; Version: 0.0.1
; ----------------------------------------------------------------------------
; ROM Basic Routines
; ----------------------------------------------------------------------------

; The register adresses of the 6522 VIA chip
PORTB        .equ $6000
PORTA        .equ $6001
DDRB         .equ $6002
DDRA         .equ $6003
SHIFTREG     .equ $600A
ACR          .equ $600B ; Auxiliary Control Register

TEMP0        .equ $0200 ; Address for temporary variables used by the routines

; ****************************************************************************
; Memory Layout
; ****************************************************************************
; $0000 - $00FF  00000000 00000000 - 00000000 11111111 : Zeropage RAM
; $0100 - $01FF  00000001 00000000 - 00000001 11111111 : Stack
; $0200 - $02FF  00000010 00000000 - 00000010 11111111 : Reserved RAM for BIOS
; $0300 - $5FFF  00000011 00000000 - 01011111 11111111 : Free RAM (21 KB)
; $6000 - $7FFF  01100000 00000000 - 01111111 11111111 : VIA 6522 Address space (8KB)
; $8000 - $FFFF  10000000 00000000 - 11111111 11111111 : ROM
;
; ****************************************************************************
; Important Adresses
; ****************************************************************************
; $FFFA : NMI Vector / 2 Bytes
; $FFFC : Reset Vector (Adress to jump to after reset) / 2 Bytes
; $FFFE : IRQ/BRK Vector / 2 Bytes
; 
; ****************************************************************************
; Routine Table
; ****************************************************************************
; Here I save the adresses to the subroutine, so other program can make use
; of them without the need to have the symbols. These adresses can be used as
; subroutines. For example:7
;   jsr $8003 ; read write byte via spi

  .org $8000
  jmp spi_init_shift_register ; $8000
  jmp spi_rw_byte             ; $8003
  jmp spi_r_byte              ; $8006

; ****************************************************************************
; NMI Routine
; ****************************************************************************
nmi_routine:
  jmp nmi_routine

; ****************************************************************************
; IRQ/BRK Routine
; ****************************************************************************
; In case of break condition, the B flag in the processor status is set.
; We just do an endless loop.
irq_brk_routine:
  jmp irq_brk_routine

viatest:
  lda #$FF
  sta DDRA
  sta PORTA
  sta DDRB
  lda #%10101010
  sta PORTB
  
vt:
  jmp vt

lcdtest:
  jsr lcd_init_mcu
  jsr lcd_clear_display
  jsr lcd_return_home
  
  lda #"*"
  jsr lcd_print_char
  ldx #0
next_char:
  lda $9000,x
  beq string_end
  jsr lcd_data
  inx
  jmp next_char
string_end:
  jmp string_end  


; ----------------------------------------------------------------------------
; First routine, that doesn't make any sense, but in the beginning, we have
; only ROM, so we cannot use the stack, subroutines or the VIA chip.
; The first loop should run 10 times. The second loop endless.
; ----------------------------------------------------------------------------
reset:
  ldx #$FF
  txs
  lda #$42
  sta $2000
  lda $2000
  lda #$11
  lda #$22
  lda #$33
  brk             ; We are finished. Now the processor should ftech the break
                  ; vector and jump to the routine.

; ****************************************************************************
; Include the necessary libraries
; ****************************************************************************
  .include spi.inc
  .include lcd_hd44780u_8bit.inc
  .include numbers.inc

  .org $9000
  .string "   FirstKlaas"
; ****************************************************************************
; The three vectors
; ****************************************************************************
  .org $FFFA
  .word nmi_routine
  .word lcdtest
  .word irq_brk_routine