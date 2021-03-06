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

  .macro ZPUSHA
  dex
  sta 0,x
  .endm  

  .macro ZPOPA
  lda 0,x
  inx
  .endm

  .macro ZPUSHD
  dex
  lda #\1
  sta 0,x
  .endm

  .macro ZPUSHN
  dex
  .endm

  .macro ZPOPN
  inx
  .endm

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

test_ram:
  lda #$11
  lda #$22
  rts

viatest:
  lda #$ff
  sta DDRB
  sta DDRA
  lda #%10101010
  sta PORTB
  sta PORTA
  rts

lcdtest:
  phy
  jsr lcd_init_mcu
  jsr lcd_clear_display
  jsr lcd_return_home

  ; BCD Test
  lda #84             ; Load vaklue 127 into accu
  ZPUSHA
  jsr bin2bcd8         ; convert bin to bcd
  jsr lcd_print_bcd8   ; print bcd on lcd at current position

  lda #"/"
  jsr lcd_print_char
  
  ; HEX Test
  ZPUSHD $EA
  jsr bin2hex
  ZPOPA
  jsr lcd_print_char
  ZPOPA
  jsr lcd_print_char
  
  ; Print zero terminated string from memory  
  phx
  ldx #0
next_char:
  lda $9000,x
  beq string_end
  jsr lcd_print_char
  inx
  jmp next_char

string_end:
  plx
  ply
  rts  


; ----------------------------------------------------------------------------
; Boot sequence of the FirstKlaas OS
; ----------------------------------------------------------------------------
boot:
  ldx #$FF        ; We use the x register as the stack pointer for the zeropage
                  ; stack. So all routines need to take care not to change x
  txs             ; Also set the stack pointer to FF

  jsr lcdtest
  lda #$AA
  lda #$BB
  lda #$CC
  

  brk             ; We are finished. Now the processor should ftech the break
                  ; vector and jump to the routine.

; ****************************************************************************
; Include the necessary libraries
; ****************************************************************************
  ;.include spi.inc
  .include lcd_hd44780u_4bit.inc
  .include numbers.inc

  .org $9000


  .string " <-- BCD"
; ****************************************************************************
; The three vectors
; ****************************************************************************
  .org $FFFA
  .word nmi_routine
  .word boot
  .word irq_brk_routine