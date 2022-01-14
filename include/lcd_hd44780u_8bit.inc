; ****************************************************************************
; LCD HD44780U Display
;
; A single HD44780U can display up to one 8-character line or two 8-character
; lines.

LCD_RS       .equ %00100000
LCD_RW       .equ %01000000
LCD_E        .equ %10000000

LCD_DATA_PORT .equ PORTA
LCD_DATA_DDR  .equ DDRA

LCD_CMD_PORT  .equ PORTB
LCD_CMD_DDR   .equ DDRB

lcd_init_mcu:
  lda #%11111111  ; All pins to output for data
  sta DDRA        ; in PORT A
  lda #%11100000  ; Top three pins to output for control signals
  sta DDRB        ; in PORT B

  lda #%00111000  ; Set 8-bit mode; 2-lines display; 5x8 font
  jsr lcd_cmd     ; send the command to the MCU
  lda #%00001110  ; Set 8-bit mode; 2-lines display; 5x8 font
  jsr lcd_cmd     ; send the command to the MCU      
  lda #%00000110  ; Display on / Increment position / No shifting
  jsr lcd_cmd
  rts             ; return


lcd_clear_display:
  lda #%00000001    ; clear display command. No parameters
  jsr lcd_cmd       ; send the command
  rts               ; return

lcd_return_home:
  lda #%00000010    ; Return cursor to home. Content not changed.
  jsr lcd_cmd       ; send the command
  rts               ; return

lcd_print_char:
  jsr lcd_data
  

  
lcd_busy_wait:
  lda #01111111
  sta LCD_DATA_DDR
lbw_0:  
  lda #LCD_RW
  sta LCD_CMD_PORT
  lda #(LCD_E | LCD_RS)
  sta LCD_CMD_PORT
  lda LCD_DATA_PORT
  bne lbw_0
  lda #LCD_RW
  sta LCD_CMD_PORT
  lda #$ff
  sta LCD_DATA_DDR
  jsr

lcd_cmd:
  sta PORTA
  lda #0
  sta PORTB
  lda #LCD_E
  sta PORTB
  lda #0
  sta PORTB
  rts

lcd_data:                ; 28
  sta PORTA              ; 4
  lda #LCD_RS            ; 2
  sta PORTB              ; 4
  lda #(LCD_RS | LCD_E)  ; 2
  sta PORTB              ; 4
  lda #LCD_RS            ; 2
  sta PORTB              ; 4
  rts                    ; 6
