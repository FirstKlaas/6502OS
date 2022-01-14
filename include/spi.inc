SPI_SH_MASK  .equ %11100011 ; ACR MASK for the shift register
SPI_SH_CFG   .equ %00001100 ; Configuration for "Shift In under control of external clock"

; ****************************************************************************
; SPI Interface Routines
; ****************************************************************************
; Bit 0 Port B: CLK
; Bit 7 Port B: MOSI / Daten senden 
; CB1 : MISO / Daten empfangen über das shiftregister
;
; The shift register gets an external clock, so the Bits 2-4 of the Auxiliare Control Register
; have to be set to 1
; In future versions, the other bits of port B will be used to select the correct slave.
; If we use the free bits directly, we could slect one out of 6 devices. But 
; we have to make shure not to activate two slaves parallel. Or we could use
; the 6 bits as an address an could address 63 devices in theory (But than we also need 63
; lines). Six devices may be enough. Possible devices are:
;
; 1: SD Card
; 2: Graphics Output (epaper or amoled output)
; 3: Joystick A
; 4: Joystick B
; 5: Keyboard
; 6: LCD Display
;
; Another idea (although not really puristic, is to use an arduino as a slave, which handles)
; all the other slaves. Is this already cheating? 
;
; ----------------------------------------------------------------------------
; Connection 
; ----------------------------------------------------------------------------
; Port B is dedicated to SPI communication, as we handle most of the external
; components via SPI.
; We use CB1 as clock, which is triggered bei Bit 0 of the port B data. So we 
; connect CB1 with PB0. Bit 1 to 4 (four bits) can be used to activate
; different slaves. Bit 7 is MOSI (Master Out / Slave In). And incoming data
; is handeld via the shift register and therefore MISO is connected to CB2.
; The bits for the shift register in the ACR have to be vonfigured as:
; "Shift In – External CB1 Clock Control (011)”-mode
; ----------------------------------------------------------------------------
; SPI send and receive a byte via the shift register 
; ----------------------------------------------------------------------------

; We use an external clock for the shift register. To set this mode,
; the bits 2 to 4 of the ACR have to be set to 1.  
spi_init_shift_register:
  lda ACR          ; Load current value from the ACR
  and #SPI_SH_MASK ; Clear out shift register bits
  ora #SPI_SH_CFG  ; Shift in and shift out under control of external clock
  sta ACR          ; Store the new ACR value
  rts

; Write a single byte via the "MOSI" bit of PORT B. Receiving data
; via the shift register.
; After writing has finished, the received byte is stored in the accu

; The value from the accumulator is written via the shift register.
; Because we need the x and the y register, we push them to the stack first.
spi_rw_byte:
  phx             ; save the X register
  phy             ; sva ethe y register

  sta TEMP0       ; Save the value to memory, because we need the accu.
  ldx #$08        ; We need to write eight bits. 
  lda PORTB       ; Load current value from PORT B
  and #%11111110  ; Clear the SPI CLK Bit
  asl PORTB       ; Shift to left. The loop will shift it back
  tay             ; Wert in Y register sichern

spi_rw_byte_0:
  rol TEMP0       ; rotate the value to the left. The highest bit is in the carry flag
  tya             ; Portvalue back to accu
  ror             ; shifting to the right. The carry bit is now the highest bit in the accu
  sta PORTB       ; Write back to port B.  Because bit 0 is 0, set clock low
  inc PORTB       ; Clock high, because the clock is connected to bit 0
  sta PORTB       ; Set clock low again. TODO: Do we need the really? In the loop the clock is set to low twice.
  dex             ; Decrement counter
  bne spi_rw_byte_0 ; next loop iteration
  lda SHIFTREG    ; Read the received value from the shift register
  ply             ; Restore the y register
  plx             ; Resore the x register
  rts             ; Return from subroutine

; ----------------------------------------------------------------------------
; SPI fast send a byte via the shift register 
; ----------------------------------------------------------------------------
; Of course we could clock through a loop and the codesize would be smaller.
; But also slower. As a trade off decision we prefer speed over size.
; 
; After reading 8 buts the result is stored in the accu
;
; 2022-10-01
;   
spi_r_byte:
  phy             ; Save y register to stack
  lda PORTB       ; Load current value from port B
  and #%01111110  ; clear data and clock bit
  tay
  ora #$01        ; Set clock bit
  sta PORTB       ; Clock high / Bit 7
  sty PORTB       ; Clock low / Bit 7
  sta PORTB       ; Clock high / Bit 6
  sty PORTB       ; Clock low / Bit 6
  sta PORTB       ; Clock high / Bit 5
  sty PORTB       ; Clock low / Bit 5
  sta PORTB       ; Clock high / Bit 4
  sty PORTB       ; Clock low / Bit 4
  sta PORTB       ; Clock high / Bit 3
  sty PORTB       ; Clock low / Bit 3
  sta PORTB       ; Clock high / Bit 2
  sty PORTB       ; Clock low / Bit 2
  sta PORTB       ; Clock high / Bit 1
  sty PORTB       ; Clock low / Bit 1
  sta PORTB       ; Clock high / Bit 0
  sty PORTB       ; Clock low / Bit 0
  lda SHIFTREG    ; Load the value from the VIA shift register into accu
  ply             ; Restore y register from stack
  rts 
