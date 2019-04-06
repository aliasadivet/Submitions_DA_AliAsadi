;
; DA2A.asm.asm
;
; Created: 2/27/2019 1:19:00 AM
; Author : Ali Asadi
;

rjmp main
.org 0x0100

main:
	;stack initialization 
	ldi r16, high (RAMEND)
	out SPH, r16
	ldi r16, low (RAMEND)
	out SPL, r16 
	;rcall Pin_Init_ASM			;change to comment when executing second part of the task (just for convenience)
	rcall Switch_Init_ASM		;change to code instruction when executing second part of the task

loop:
	;rcall Pulse_ASM			;change to comment when executing second part of the task (just for convenience)
	rcall LED_ASM			;change to code instruction when executing second part of the task
	rjmp loop



Pin_Init_ASM:
    sbi DDRB, 2              ; sbi - set bit I/O - set bit 2 in DDRB register - Output Mode
    sbi PORTB, 2             ; cbi - clear bit I/O - clear bit 2 in PORTB register - Output Low
    ret                      ; return from the function
    
Pulse_ASM:

    ldi r24, 87              ; ldi - load immediate  load value of 87 in r24 - 87*5mS = 435mS
    cbi PORTB, 2             ; set bit 2 in PORTB register - Output High
    rcall Delay_ASM          ; rcall - relative call , call Delay_ASM Function 
    sbi 0x05, 2              ; clear bit 2 in PORTB register - Output Low
    ldi r24, 58              ; load value of 58 in r24 - 58*5mS = 290mS 
    rcall Delay_ASM          ; call Delay_ASM Function 
    ret                      ; return from the function

;   Delay_ASM generates a delay of 5mS for each loop
;   So to generate a delay of 100mS value of 100/5 = 20 is sent via r24 register
;   ldi - 1 Cycle
;   sbiw - 2 cycle
;   brne - 2 cycle 
;   so sbiw and brne loop till the value in r24 decrements to zero
;   for example if r24 = 1 , then those two lines executes 1 time which is 4 cycle
;   if r24 = 10, then executes 10 times which is 4 * 10 = 40 cycle
;   frequency is 8Mhz then = each cycle is 1/8Mhz = 1/8uS  
;   therefore 4Cycle = 4*(1/8)uS = 0.5uS delay
;   to get 5mS = 5000uS , number of cycles = 5000uS / (1/8)uS = 5000 * 8 cycles
;   but this loop already generates delay of 4 cycles  
;   to take that into account 5000 * 8 / 4 = 10000 cycles
;   there 10000 * 4 = 40000cycles * (1/8uS) = 5000 uS 
Delay_ASM:    

    mov r16, r24                ; move r24 to r16 
    ldi r24, low(5000 * 8 / 4)  ; load lower byte of the delay value to r24
    ldi r25, high(5000 * 8 / 4) ; load higher byte of the delay value to r25
    sbiw r24, 1                 ; subtract Immediate from word  -  r25:r24 - 1
    brne PC-1                   ; branch if not equal ie branch till result of previous instruction is Zero 
    dec r16                     ; decrement r16                        
    brne PC-5                   ; branch till r16 becomes zero
    ret                         ; exit or return

Switch_Init_ASM:
            
    cbi DDRD, 2                 ; DDRD bit 2 = 0 INPUT Mode
    sbi PORTD, 2                ; PORTD bit 2 = 1 PULLUP Enable
    sbi DDRB, 2                 ; PORTB.2 - Output 
    sbi PORTB, 2                ; PORTB.2 - Output Low
    ret                         ; return from the function

LED_ASM:    

    sbis PIND, 2                ; skip if bit is set in I/O register, skip if HIGH state
    rjmp pressed                ; relative jump - jump to pressed label if State is low
    brtc return                 ; branch if T bit is cleared - check the previous state
    ldi r24, 2                  ; load r24 with 2 to generate a delay of 10mS
    rcall Delay_ASM             ; call Delay_ASM to generate debounce delay 
    sbis PIND, 2                ; check if the bit stays the same
    rjmp return                 ; return if not
    cbi PORTB, 2                ; PORTB.2 - Output High
    ldi r24, 250                ; load r24 with 250 to generate a delay of 1250mS
    rcall Delay_ASM             ; call delay
    sbi PORTB, 2                ; PORTB.2 - Output Low
    clt                         ; clear T , previous state is high
    rjmp return                 ; return
pressed: 
    ldi r24, 2                  ; if state is low, then generate a debounce delay of 10mS
    rcall Delay_ASM             ; call delay    
    sbis PIND, 2                ; check if the bit stays the same
    set                         ; set T bit to indicate previous state is Low
return:
    ret                         ; return from the function
    