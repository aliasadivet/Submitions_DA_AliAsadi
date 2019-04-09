;
; DA2C-T2.asm
;
; Created: 3/18/2019 5:32:23 AM
; Author : Ali Asadi
;


; Replace with your application code

.org 0
rjmp main
.org 0x010
    rjmp Int_int_ASM        ; ISR Handler

.org 0x100
Int_int_ASM:
		in r18, SREG
		push r18
        ldi r18, 1
        add r16, r18
        clr r18
        adc r17, r18        ; Increment registers r17:r16
		pop r18
		out SREG, r18
        reti
main: 
	;stack initialization
	ldi r16, high (RAMEND)
	out SPH, r16
	ldi r16, low (RAMEND)
	out SPL, r16 

	rcall Timer_Init_int_ASM
	;rcall Pin_Init_int_ASM			;make this line comment when executing the second part of the task
	rcall Switch_Init_int_ASM		;make this line instruction  when executing the second part of the task

loop:
	;rcall Pulse_int_ASM			;make this line comment when executing the second part of the task
	rcall LED_int_ASM				;make this line instruction  when executing the second part of the task
	rjmp loop
        

Pin_Init_int_ASM:
    sbi DDRB, 2              ; sbi - set bit I/O - set bit 2 in DDRB register - Output Mode
    sbi PORTB, 2             ; cbi - clear bit I/O - clear bit 2 in PORTB register - Output Low
    ret                      ; return from the function

Timer_Init_int_ASM:
    ldi r16, 0x03            ; divide input clock by 64 => 8Mhz/64 ; CS02 = 0 CS01 = 1 CS00 = 1
    out TCCR0B, r16          ; save it in register   
    ldi r16, 0x01            ; enable Overflow interrupt TOIE0 
    sts TIMSK0, r16          ; save it in register   
    sei                      ; Enable global interrupt
    ret 
    
Pulse_int_ASM:

    ldi r24, low(435)       ; ldi - load immediate  load value of 87 in r24 - 435mS
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
    clr r16                  ; clr r17:r16 which is the counter in ISR
    clr r17
    cp r24, r16              ; compare two resister r24-r16
    cpc r25, r17             ; compare with carry   r25-r17-C
    brne PC-2                 ; branch if not equal i.e r25:r24 and r17:r16 are not equal   
    ret                      ; exit or return

Switch_Init_int_ASM:

    cbi DDRD, 2                 ; DDRD bit 2 = 0 INPUT Mode
    sbi PORTD, 2                ; PORTD bit 2 = 1 PULLUP Enable
    sbi DDRB, 2                 ; PORTB.2 - Output 
    sbi PORTB, 2                ; PORTB.2 - Output Low
    ret                         ; return from the function

LED_int_ASM:

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
    
