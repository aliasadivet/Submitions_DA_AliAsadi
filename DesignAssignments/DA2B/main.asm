;
; DA2B.asm
;
; Created: 3/5/2019 1:51:07 AM
; Author : Ali Asadi
;

rjmp main
.org 0x0001                     ; location of Interrupt service routine
    rjmp INT0_vect              ; jump to INT0_vect location

.org 0x0020                      
INT0_vect:
    cbi PORTB, 2                ; PORTB.2 - Output High 
    ldi r24, 250                ; load r24 with 250 to generate a delay of 1250mS
    rcall Delay_ASM             ; call delay
    sbi PORTB, 2                ; PORTB.2 - Output Low
    reti                        ; return from thr interrupt


main:
	;stack initialization 
	ldi r16, high (RAMEND)
	out SPH, r16
	ldi r16, low (RAMEND)
	out SPL, r16 
	rcall Int_Init_ASM

loop:
	rjmp loop



    


Int_Init_ASM:

    cbi DDRD, 2                 ; DDRD bit 2 = 0 INPUT Mode
    sbi PORTD, 2                ; PORTD bit 2 = 1 PULLUP Enable
    sbi DDRB, 2                 ; PORTB.2 - Output 
    sbi PORTB, 2                ; PORTB.2 - Output Low
    
    ldi r16, 0x03               ; load 0x00000011 in r16
    sts EICRA, r16              ; store r16 in EICRA
    sbi EIMSK, 0                ; set bit 0 in EIMSK register to enable INT0
    sei                         ; Enable Global interrupt
    ret                         ; return from the function
    
;   Delay_ASM generates a delay of 5mS for each loop
;   So to generate a delay of 100mS value of 100/5 = 20 is sent via r24 register
;   ldi - 1 Cycle
;   sbiw - 2 cycle
;   brne - 2 cycle 
;   so sbiw and brne loop till the value in r24 decrements to zero
;   for example if r25 = 1 , then those two lines executes 1 time which is 4 cycle
;   if r24 = 10, then executes 10 times which is 4 * 10 = 40 cycle
;   frequency is 8Mhz then = each cycle is 1/8Mhz = 1/8uS  
;   therefore 4Cycle = 4*(1/8)uS = 0.5uS delay
;   to get 5mS = 5000uS , number of cycles = 5000uS / (1/8)uS = 5000 * 8 cycles
;   but this loop already generates delay of 4 cycles  
;   to take that into account 5000 * 8 / 4 = 10000 cycles
;   there 10000 * 4 = 40000cycles * (1/8uS) = 5000 uS 
Delay_ASM:    

    mov r16, r24                ; move r24 to r16 
    ldi r24, low(5000 * 8 / 4 ) ; load lower byte of the delay value to r24
    ldi r25, high(5000 * 8 / 4 ) ; load higher byte of the delay value to r25
    sbiw r24, 1                 ; subtract Immediate from word  -  r25:r24 - 1
    brne PC-1                   ; branch if not equal ie branch till result of previous instruction is Zero 
    dec r16                     ; decrement r16                        
    brne PC-5                   ; branch till r16 becomes zero
    ret                         ; exit or return
    