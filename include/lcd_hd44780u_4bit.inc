; ****************************************************************************
; LCD HD44780U Display
;
; A single HD44780U can display up to one 8-character line or two 8-character
; lines.
;
; This ist the four bit interface. The pins are connected as followed: 
; 
; E   -> Bit 0
; RW  -> Bit 1
; RS  -> Bit 2
; DB7 -> Bit 7 
; DB6 -> Bit 6 
; DB5 -> Bit 5 
; DB4 -> Bit 4 

; DB7 | DB6 | DB5 | DB4 | * | RS | RW | E

LCD_RS       .equ %00000100
LCD_RW       .equ %00000010
LCD_E        .equ %00000001
LCD_OUTPUT   .equ %11110111 ; Pins needed to be set as output pins
LCD_PORT     .equ PORTA
LCD_DDR      .equ DDRA
LCD_CMASK    .equ %00000111

lcd_init_mcu:
  lda #LCD_OUTPUT   ; Set output pins
  sta LCD_DDR       ; in PORT A

  lda #%00100000    ; Function Set 4 Bit
  jsr lcd_cmd       ; send the command to the MCU
  lda #%00100000    ; Function Set 4-bit mode;
  jsr lcd_cmd       ; send the command to the MCU
  lda #%10000000    ; 2 lines / 5 x 8 font
  jsr lcd_cmd       ; send the command to the MCU

  lda #%00000000    ; Display on / Cursor on / Blinking off
  jsr lcd_cmd
  lda #%11100000    ; 
  jsr lcd_cmd

  lda #%00000000    ; Entry Mode Set
  jsr lcd_cmd
  lda #%01100000    ; Inc address / Don't shift display
  jsr lcd_cmd

  rts               ; return

lcd_print_char:
  phx               ; Save the X register
  tax               ; Save the original value to X register.
  and #%11110000    ; First we send the high nibble, also E to low
  ora #LCD_RS       ; set the rs bit, as we don't send an instruction.
  jsr lcd_cmd       ; Send the data to the MCU
  txa               ; Restore the value
  asl               ; Shift it four times to move the low nibble to the
  asl               ; high nibble. ASL add zeros to the right.
  asl               ; Therefor also E is zero
  asl
  ora #LCD_RS       ; Set the RS bit for data transfer.
  jsr lcd_cmd       ; Send the data to the mcu
  plx               ; Restore X register
  rts               ; Go back  

lcd_clear_display:  ; 8-Bit instraction: 0000 0010
  lda #%00000000    ; clear display command. No parameters
  jsr lcd_cmd       ; send the command
  lda #%00010000    ; clear display command. No parameters
  jsr lcd_cmd       ; send the command
  rts               ; return

lcd_return_home:    ; 8-Bit instraction: 0000 0010
  lda #%00000000
  jsr lcd_cmd       ; send the command
  lda #%00100000    ; Return cursor to home. Content not changed.
  jsr lcd_cmd       ; send the command
  rts               ; return

lcd_cmd:            ; 22 cycles
  sta LCD_PORT      ; Write command                  (4)
  ora #LCD_E        ; Set the E Bit to 1             (2)
  sta LCD_PORT      ; Write command                  (4)
  and #(~LCD_E)     ; Clear the E Bit                (2)
  sta LCD_PORT      ; Write command                  (4)
  rts               ;                                (6)

lcd_print_bcd8:
  lda 1,x 
  and #%00001111
  ora #%00110000
  jsr lcd_print_char
  lda 0,x
  ror
  ror
  ror
  ror
  and #%00001111
  ora #%00110000
  jsr lcd_print_char
  lda 0,x
  and #%00001111
  ora #%00110000
  jsr lcd_print_char
  rts 