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

_LeerInteiro:
    enter 0,0
    ;Salva o valor anterior dos registradores utilizados (menos eax, ja reservado para retorno, como padrao)
    push ebx
    push ecx
    push edx

    ;Le a string de 3 algarismos, no maximo
    mov eax, 3
    mov ebx, 0
    mov ecx, [EBP+8]
    mov edx, 3
    int 80h

    mov ecx, 0
    ;Confere se tem o sinal -
    cmp [EBP+8], '0'


    _LeerInteiro_Loop:  ;ler char a char como numero, conferindo se eh de fato um numero ou se eh o enter
    
    mov eax, [EBP+8+ecx]





_EscreverInteiro:

;Parametro: endereco de memoria onde inserir a leitura
_LeerChar:
    enter 0,0
    mov eax, 3
    mov ebx, 0
    mov ecx, [EBP+8]
    mov edx, 1
    int 80h
    leave
    ret

;Parametro: endereco de memoria de onde pegar o char a escrever
_EscreverChar:  
    enter 0,0
    mov eax, 4
    mov ebx, 1
    mov ecx, [EBP+8]
    mov edx, 1
    int 80h
    leave
    ret

;Parametros: endereco de memoria onde inserir a leitura, e quantos bytes ler
_LeerString:
    enter 0,0
    mov eax, 3
    mov ebx, 0
    mov ecx, [EBP+8]
    mov edx, [EBP+12]
    int 80h
    leave
    ret


_EscreverString:


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

