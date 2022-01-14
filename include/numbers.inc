BCD  .equ $0200
BIN  .equ $0208

bin2bcd8:
    phx
    sta BIN
    lda #0
    sta BCD+0
    sta BCD+1
    ldx #8
 bin2bcd8_0:
    asl BIN
    lda BCD+0
    adc BCD+0
    sta BCD+0
    lda BCD+1
    adc BCD+1
    sta BCD+1
    dex
    bne bin2bcd8_0
    cld 
    plx
    rts
