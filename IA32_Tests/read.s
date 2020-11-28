section .data
msg1 db 'voce escreveu: '
sz1 EQU $-msg1
msg2 db 'certo?', 0dh, 0ah
sz2 EQU $-msg2

section .bss
leitura resb 5
sz EQU $-leitura
check resb 1
szc EQU $-check

section .text
global _start
_start: mov eax, 3  ;leitura
        mov ebx, 0  ;ler do teclado
        mov ecx, leitura    ;pra onde escrever
        mov edx, sz ;quantos bytes ler
        int 80h     ;faz a leitura

        mov eax, 4  ;print
        mov ebx, 1  ;escrever no monitor
        mov ecx, msg1    ;escrever o que tem no endereco de leitura
        mov edx, sz1  ;escrever com o tamanho que foi pega a leitura
        int 80h     ;manda escrever


        add eax, 30h
        mov dword [check], eax
        mov eax, 4  ;print
        mov ebx, 1  ;escrever no monitor
        mov ecx, check
        mov edx, sz2
        int 80h

        mov eax, 4  ;print
        mov ebx, 1  ;escrever no monitor
        mov ecx, leitura
        mov edx, sz
        int 80h


    
        mov eax, 4  ;print
        mov ebx, 1  ;escrever no monitor
        mov ecx, msg2
        mov edx, sz2
        int 80h

        

        mov eax, 1  ;manda parar
        mov ebx, 0  ;para mandando 0
        int 80h     ;para de fato

