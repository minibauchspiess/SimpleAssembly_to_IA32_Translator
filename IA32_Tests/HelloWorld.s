section .data
msg db 'Hey mundonasuansaosas', 0dh, 0ah
SIZE EQU $-msg


section .text
global _start
_start: mov eax, 4      ;chamada print
        mov ebx, 1      ;usar monitor
        mov ecx, msg    ;primeiro endereço com conteúdo a ser printado
        mov edx, SIZE     ;quantos bytes a serem printados
        int 80h         ;chama a interrupcao

        mov eax, 1      ;chamada de parar
        mov ebx, 0      ;retorno de parada
        int 80h         ;chama interrupcao para parar
