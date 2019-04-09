;
; DA2C-T1.asm
;
; Created: 3/15/2019 6:32:44 AM
; Author : Ali Asadi
;

main: 
	;stack initialization

	ldi r16, high (RAMEND)
	out SPH, r16
	ldi r16, low (RAMEND)
	out SPL, r16 

	rcall Timer_Init_ovf_ASM
	;rcall Pin_Init_ovf_ASM			;change to comment to execute second part of the task(just for convenience to avoid making two seperate projects)
	rcall Switch_Init_ovf_ASM		;change to instruction in executing part 2	

loop:
	;rcall Pulse_ovf_ASM				;change to comment to execute second part of the task(just for convenience to avoid making two seperate projects)
	rcall LED_ovf_ASM				;change to instruction in executing part 2	
	rjmp loop



Pin_Init_ovf_ASM:
    sbi DDRB, 2              ; sbi - set bit I/O - set bit 2 in DDRB register - Output Mode
    sbi PORTB, 2             ; cbi - clear bit I/O - clear bit 2 in PORTB register - Output Low
    ret                      ; return from the function

Timer_Init_ovf_ASM:
    ldi r16, 0x03            ; divide input clock by 64 => 8Mhz/64 ; CS02 = 0 CS01 = 1 CS00 = 1
    out TCCR0B, r16          ; save it in register   
    ret 
    
Pulse_ovf_ASM:

    ldi r24, low(435)       ; ldi - load immediate  load value of 435 in r24 - 435mS
    ldi r25, high(435)
    cbi PORTB, 2             ; set bit 2 in PORTB register - Output High
    rcall Delay_ASM          ; rcall - relative call , call Delay_ASM Function 
    sbi 0x05, 2              ; clear bit 2 in PORTB register - Output Low
    ldi r24, low(290)        ; load value of 58 in r24 - 145*2 = 290mS 
    ldi r25, high(290)
    rcall Delay_ASM          ; call Delay_ASM Function 
    ret                      ; return from the function

    
Delay_ASM:

    lsr r25                  ; r25:r24 contains delay in mS but clock generates a delay of 2.048mS
    ror r24                  ; so divide by 2 to get actual time of delays
    sbiw r24, 4              ; subtract 4 from r25:r24 to compensate 0.048mS delay in each cycle   
    sbis TIFR0, TOV0         ; check whether Overflow occured
    rjmp PC-1                ; wait till the Overflow occur
    sbi TIFR0, TOV0          ; clear Overflow flag
    sbiw r24, 1              ; decrement r25:r24 mS counter
    brne PC-4                ; branch till r25:r24 becomes zero
    ret                      ; exit or return

Switch_Init_ovf_ASM:

    cbi DDRD, 2                ; DDRD bit 2 = 0 INPUT Mode
    sbi PORTD, 2                ; PORTD bit 2 = 1 PULLUP Enable
    sbi DDRB, 2                 ; PORTB.2 - Output 
    sbi PORTB, 2                ; PORTB.2 - Output Low
    ret                         ; return from the function

LED_ovf_ASM:

    sbis PIND, 2                ; skip if bit is set in I/O register, skip if HIGH state
    rjmp pressed                ; relative jump - jump to pressed label if State is low
    brtc return                 ; branch if T bit is cleared - check the previous state
    ldi r24, 10                  ; load r24 with 2 to generate a delay of 10mS
    ldi r25, 0
    rcall Delay_ASM             ; call Delay_ASM to generate debounce delay 
    sbis PIND, 2                ; check if the bit stays the same
    rjmp return                 ; return if not
    cbi PORTB, 2                ; PORTB.2 - Output High
    ldi r24, low(1250)          ; load r24 with 250 to generate a delay of 1250mS
    ldi r25, high(1250)
    sbiw r24, 10
    rcall Delay_ASM             ; call delay
    sbi PORTB, 2                ; PORTB.2 - Output Low
    clt                         ; clear T , previous state is high
    rjmp return                 ; return
pressed: 
    ldi r24, 10                 ; if state is low, then generate a debounce delay of 10mS
    ldi r25, 0
    rcall Delay_ASM             ; call delay    
    sbis PIND, 2                ; check if the bit stays the same
    set                         ; set T bit to indicate previous state is Low
return:
    ret                         ; return from the function
    

