BCD  .equ $0200
BIN  .equ $0208


;*****************************************************************************
; BIN2HEX - Converting a one byte binary into a two byte hex-ascii
;           representation
;
;*****************************************************************************
bin2hex:
    ZPUSHN
    lda 1,x
    and #$F0            ; Isolate higher nibble
    clc                 ; clear carry, so we don't shift 1 in
    ror                 ; move from high nibble to low nibble
    ror
    ror
    ror
    cmp #$0a            ; less then 10?
    bcc .higher_0       ; yes, so we need a digit
.higher_A:              ; no, we need a letter from A to F
    adc #54             ; Add 55 to the value 10 + 55 = 56 which is the 
                        ; ASCII of letter A. We use 54, as the carry bit is set
                        ; in this situation and is taken into account, when using
                        ; adc (add with carry) 
    jmp .lower_nibble   ; check the lower nibble

.higher_0:
    ora #%00110000      ; 0 has code 48 in ascii. just or to bits to add 48
.lower_nibble:          ; Now let's analyse the loer nibble
    sta 0,x             ; First save the result of the higher nibble conversion
                        ; back to the stack as return value
    lda 1,x             ; Get the input value back again
    and #$0F            ; Isolate lower nibble
    cmp #$0a            ; less then ten?
    bcc .lower_0
.lower_A
    adc #54             ; Add 55 (54 plus carry) to get the correct character
    jmp .return

.lower_0
    ora #%00110000
.return
    sta 1,x             ; Write back the result to the zero page stack
    rts 

;*****************************************************************************
; BIN2BCD8 - Converting a binary byte to a BCD word
;
; In (packed) BCD each digit of the number is represented in a nibble. for a
; single binary byte we can represent values from 0 to 255. So we have a
; maximum of three digits/nibbles. The result is represented in 2 bytes.
;
; Parameters are passed and the result is returned using the zeropage stack.
;
; IN:
;  - The value to be converted in binary format
;
; OUT:
;  - The lower byte of the BCD value
;  - The higher byte of the BCD value    
;
; SINCE_ 0.1
;
bin2bcd8:
    phy
    ZPUSHN              ; Add on byte to zero stack for the second return value
    ZPUSHN              ; Another one for a local temp variable
    lda 2,x             ; load the original input variable
    sta 0,x             ; store it to the temp variable
    sed
    lda #0
    sta 1,x             ; Set return value zo 0
    sta 2,x
    ldy #8
 bin2bcd8_0:
    asl 0,x
    lda 1,x
    adc 1,x
    sta 1,x
    lda 2,x
    adc 2,x
    sta 2,x
    dey
    bne bin2bcd8_0

    cld 
    ZPOPN               ; Remove the temp variable
    ply
    rts

bin2bcd16:
    phx
    sed
    lda #0
    sta BCD+0
    sta BCD+1
    sta BCD+2
    ldx #16
 bin2bcd16_0:
    asl BIN
    rol BIN+1
    lda BCD+0
    adc BCD+0
    sta BCD+0
    lda BCD+1
    adc BCD+1
    sta BCD+1
    lda BCD+2
    adc BCD+2
    sta BCD+2
    dex
    bne bin2bcd16_0
    cld 
    plx
    rts
