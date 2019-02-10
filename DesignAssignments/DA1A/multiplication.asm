;
; multiplication.asm
;
; Created: 08.02.2019. 23:08:51
; Author : Ali Asadi
;

.macro multiplicand_l
cbr @0, $FF
sbr @0, low(@2)
cbr @1, $FF
sbr @1, high(@2)
.endm 

start:

	multiplicand_l r24, r25, 65535 ; THIS IS WHERE MULTIPLICAND VALUE CAN BE CHANGED
; Clear R22 register and assign 8-bit multiplier value
	cbr r22, $FF
	sbr r22, 255
; Clear registers R18, R19 and R20, this registers are going to hold multiplication result
	cbr r18, $FF
	cbr r19, $FF
	cbr r20, $FF

; Clear register R26, R26 is used as a counter, in every loop iteration R26 is incremented
; then its value is compared with value of multiplier
	cbr r26, $FF
	sbr r26, 0
; Clear R27 register, R27 is added to R20 which holds the most significant byte of the result.
; R27 is set to 0 and it is used in order to perform adc instruction with R20 which means if carry (C) flag 
; is set as a result of previous adc instruction (adc r19, r25), instruction adc r20, r27 will push carry bit into
; the R20 register, in every next itteration carry bit (if any) will be added to R20 register
	cbr r27, $FF

; loop block, itterate addition as many times as multiplier value is
; example: if multiplier value is 5 and multiplicand value is 3
; we need to add 3 five times in order to multiply numbers.
; R18, R19 and R20 are cleared, first we add least significant byte 
; of the multiplicand to R18 without carry (add) because this is first addition
; next we add middle byte of the multiplicand to R19 together with carry bit
; if instruction add r18, r24 set carry flag
; and finaly we add 0 to R20 together with carry bit if previous addition produced carry bit
; at the end of the loop block R26 which is used as a counter is incremented and compared to
; multiplier value, if they are equal programs steps out of the loop.

loop:
; add lowest byte
	add r18, r24
; add middle byte
	adc r19, r25
; add carry if any
	adc r20, r27
; increment counter
	inc r26
; check if counter reached its final value
	cpse r26, r22
; if not go back to begining of the loop, if yes skip next instruction step out of the loop
	rjmp loop
; execute mul instruction to compare results	
	mul r24, r22
	nop
end:
    rjmp end
