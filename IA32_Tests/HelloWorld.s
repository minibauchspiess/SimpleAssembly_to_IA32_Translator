section .data
msg1 db 'um  '
msg2 db 'dois'

section .bss
var1 resd 1
var2 resd 1


global _start
section .text
_start:
mov eax, [msg1]
mov [var2], eax
;mov [var2], [var1]

mov eax, 4
mov ebx, 1
mov ecx, var2
mov edx, 4


int 80h

mov eax, 1
mov ebx, 0
int 80h