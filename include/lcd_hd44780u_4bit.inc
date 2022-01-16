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
  sta LCD_PORT       ; send the command to the MCU
  inc LCD_PORT
  sta LCD_PORT
  jsr lcd_busy_wait

  lda #%00101000    ; Function Set 4-bit mode;
  jsr lcd_cmd       ; send the command to the MCU
  
  lda #%00001110    ; Display on / Cursor on / Blinking off
  jsr lcd_cmd

  lda #%00000110    ; Entry Mode Set
  jsr lcd_cmd

  rts               ; return

lcd_print_char:
  phx               ; Save the X register
  tax               ; Save the original value to X register.
  and #%11110000    ; First we send the high nibble
  ora #LCD_RS       ; set the rs bit, as we don't send an instruction.
  sta LCD_PORT      ; Write command / CLOCK low
  inc LCD_PORT      ; Clock high
  sta LCD_PORT      ; CLOCK low
  txa               ; Restore the value
  asl               ; Shift it four times to move the low nibble to the
  asl               ; high nibble. ASL add zeros to the right.
  asl               ; Therefor also E is zero
  asl
  ora #LCD_RS       ; Set the RS bit for data transfer.
  sta LCD_PORT      ; CLOCK low
  inc LCD_PORT      ; CLOCK high
  sta LCD_PORT      ; CLOCK low
  plx               ; Restore X register
  jsr lcd_busy_wait
  rts               ; Go back  

lcd_clear_display:  ; 8-Bit instraction: 0000 0010
  lda #%00000001    ; clear display command. No parameters
  jsr lcd_cmd       ; send the command
  rts               ; return

lcd_return_home:    ; 8-Bit instraction: 0000 0010
  lda #%00000010
  jsr lcd_cmd       ; send the command
  rts               ; return

lcd_busy_wait:
  lda #%00000111    ; Data bits are input bits
  sta LCD_DDR       ; Set direction
  
.lbw_0:
  lda #LCD_RW       ; Set Busy Read cmd (Clock low / RW high / RS low)
  sta LCD_PORT      ; and write it to port
  inc LCD_PORT      ; Clock high / RW high / RS low
  lda LCD_PORT      ; Read the result (high nibble)
  tay               ; Save high nibble
  lda #LCD_RW       ; Set Busy Read cmd  (Clock low / RW high)
  sta LCD_PORT
  inc LCD_PORT      ; Clock high / RW high
  lda LCD_PORT      ; Read the result (low nibble) (But we don't need it)
  lda #LCD_RW
  sta LCD_PORT      ; Clock low
  tya               ; restore high nibble
  cmp #%10000000    ; busy flag set?
  bne .lbw_0
  lda #%11110111    ; Data bits are output bits
  sta LCD_DDR       ; Set direction
  rts               ; No more busy. Return
  
lcd_cmd:            ; 22 cycles
  phy
  tay               ; save accu for the low nibble to be sent
  and #%11110000    ; Only high nibble may contain data
  sta LCD_PORT      ; Write command (Clock low / RS low / RW low)
  inc LCD_PORT      ; Clock high
  sta LCD_PORT      ; Clock low
  tya               ; Restore command
  asl
  asl
  asl
  asl
  sta LCD_PORT      ; Write command (Clock low / RS low / RW low)
  inc LCD_PORT      ; CLOCK high
  sta LCD_PORT      ; CLOCK low
  ply
  jsr lcd_busy_wait ; Wait until busy flag is low
  rts               ;                        

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