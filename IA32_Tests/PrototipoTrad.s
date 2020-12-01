section .data
msgLidos1 db 'O char eh: '
sz_msgLidos1 EQU $-msgLidos1
char db 'b'
szChar EQU $-char

section .bss
acc resb 1
charLido resb 1
szCharLido EQU $-charLido


section .text

%define CHAR_ESCCHAR [EBP+8]
_EscreverChar:  
    enter 0,0
    mov eax, 4
    mov ebx, 1
    mov ecx, CHAR_ESCCHAR
    mov edx, szChar
    int 80h
    leave
    ret

%define CHAR_LECHAR [EBP+8]
_LeerChar:
    enter 0,0
    mov eax, 3
    mov ebx, 0
    mov ecx, CHAR_LECHAR
    mov edx, szCharLido
    int 80h
    leave
    ret


global _start
_start:        
    
    push charLido
    call _LeerChar

    mov eax, 4              ;chamada print
    mov ebx, 1              ;usar monitor
    mov ecx, msgLidos1      ;primeiro endereço com conteúdo a ser printado
    mov edx, sz_msgLidos1   ;quantos bytes a serem printados
    int 80h                 ;chama a interrupcao

    push charLido
    call _EscreverChar

 _end:          
    mov eax, 1      ;chamada de parar
    mov ebx, 0      ;retorno de parada
    int 80h         ;chama interrupcao para parar

