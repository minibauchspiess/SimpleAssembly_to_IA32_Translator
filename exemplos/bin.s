section .data
msgBytesLidos1 db 'Foram lidos '
szMsgBytesLidos1 EQU $-msgBytesLidos1
msgBytesLidos2 db ' bytes', 0dh, 0ah
szMsgBytesLidos2 EQU $-msgBytesLidos2
msgOverflow db 0dh, 0ah, 'Muliplicacao com overflow para 32 bits', 0dh, 0ah
szMsgOverflow EQU $-msgOverflow


DOIS dd 2
section .bss
bytesLidos resd 1

OLD_DATA resd 1
NEW_DATA resd 1
TMP_DATA resd 1
section .text
;Utiliza registradores eax e edx para conferir se multiplicacao de 32 bits ocasionou overflow
_Overflow:
    push msgOverflow
    push szMsgOverflow
    call _EscreverString

    mov eax, 1      ;chamada de parar
    mov ebx, 0      ;retorno de parada
    int 80h         ;chama interrupcao para parar    

;Utiliza valor de eax para printar mensagem de bytes lidos
_PrintBytesLidos:
    enter 0,0
    push eax

    push msgBytesLidos1
    push szMsgBytesLidos1
    call _EscreverString

    pop eax
    mov [bytesLidos], eax
    push bytesLidos
    call _EscreverInteiro

    push msgBytesLidos2
    push szMsgBytesLidos2
    call _EscreverString


    leave
    ret

;Parametro: endereco de memoria onde inserir a leitura
_LeerInteiro:
    enter 0,0
    ;Salva o valor anterior dos registradores utilizados (menos eax, ja reservado para retorno, como padrao)
    push ebx
    push ecx
    push edx
    push esi

    ;Le a string de 4 posicoes (leitura de teclado eh apenas de byte em byte)
    mov eax, 3
    mov ebx, 0
    mov ecx, [EBP+8]
    mov edx, 4
    int 80h

    ;Print da quantidade de bytes lidos
    call _PrintBytesLidos

    
    mov eax, 0
    mov esi, [EBP+8]    ;Endereco base de leitura do numero
    mov ecx, 0          ;Deslocamento na string com o numero
    mov edx, 1          ;edx determinara o sinal do numero
    ;Confere se tem o sinal -
    cmp byte [esi], '-'
    jne _LeerInteiro_Loop
    inc esi
    mov edx, -1

    _LeerInteiro_Loop:  ;ler char a char como numero, conferindo se eh de fato um numero ou se eh o enter
    mov ebx, 0      ;Zera os bits mais significativos, utilizar apenas o ultimo byte
    mov bl, byte [esi+ecx]
   

    ;Confere se o char lido eh um numero (se nao for, acabaram os numeros, sair da funcao)
    sub bl, 30h   ;Transforma de ASCII pra inteiro
    cmp bl, 0
    jl _LeerInteiro_End
    cmp bl, 9
    jg _LeerInteiro_End

    


    ;Sendo um numero, faz valor = valor*10 + novo algarismo
    push edx    ;mul afeta edx tambem
    push eax
    mov eax, 10
    mul dword [ESP]
    mov dword [ESP], eax
    pop eax     ;eax=eax*10
    pop edx
    add eax, ebx    ;eax = eax*10+novo valor
    inc ecx
    
    

    ;Se nao tiver lido a quantidade maxima de algarismos, continua lendo
    cmp ecx, 3
    jb _LeerInteiro_Loop

    _LeerInteiro_End:
    ;Considera o sinal lido no comeco para o numero final
    
    imul edx

    ;Desempilha tudo que foi empilhado
    pop esi
    pop edx
    pop ecx
    pop ebx

    ;Sai da funcao, com eax sendo o retorno
    leave
    ret 4


;Parametro: endereco de memoria de onde pegar o numero pra escrever
_EscreverInteiro:
    enter 0,0
    ;Salva o valor anterior dos registradores utilizados (menos eax, ja reservado para retorno, como padrao)
    push ebx
    push ecx
    push edx

    mov ebx, dword [EBP +8]   ;ebx possui o endereco onde esta o numero
    mov eax, dword [ebx]       ;eax possui o numero, como dword

    mov ebx, 10     ;Para pegar unidades, dezenas e centenas com divisao
    sub ecx, ecx    ;ecx=0, contador de quantos algarismos o numero tem

    cmp eax, 0
    jge _EscreverInteiro_Loop   ;Se for maior que 0, nao escreve o '-'

    push eax

    push dword '-'
    
    ;Escreve o '-'
    mov eax, 4
    mov ebx, 1
    mov ecx, ESP
    mov edx, 4
    int 80h

    pop eax  ;Tira o '-' da pilha
    pop eax
    
    neg eax
    mov ebx, 10     ;Para pegar unidades, dezenas e centenas com divisao
    sub ecx, ecx    ;ecx=0, contador de quantos algarismos o numero tem
    _EscreverInteiro_Loop:
    cdq
    div ebx         ;Divide edxeax por 10
    add edx, 30h    ;Transforma o resto em ASCII
    push edx        ;Insere o resto da divisao na pilha
    inc ecx
    ;cmp ecx, 3      ;Escreve no maximo 3 algarismos
    ;je _EscreverInteiro_Print
    cmp eax, 0      ;Confere se teve quociente a divisao
    jne _EscreverInteiro_Loop

    _EscreverInteiro_Print:

    ;Printa na tela os ASCII inseridos na pilha
    mov eax, 4
    mul ecx
    mov edx, eax        ;edx = 4*contador

    mov eax, 4
    mov ebx, 1
    mov ecx, ESP
    int 80h

    ;Desempilha a quantidade de dwords empilhada dos resultados
    mov eax, edx
    cdq
    mov ebx, 4
    div ebx
    _EscreverInteiro_Desempilha:
    pop ebx     ;Registrador auxiliar pra receber os pops dword
    dec eax
    cmp eax, 0
    jne _EscreverInteiro_Desempilha
    

    ;Desempilha tudo que foi empilhado
    pop edx
    pop ecx
    pop ebx

    ;Sai da funcao, com eax sendo o retorno
    leave
    ret 4



;Parametro: endereco de memoria onde inserir a leitura
_LeerChar:
    enter 0,0

    push ebx
    push ecx
    push edx

    mov eax, 3
    mov ebx, 0
    mov ecx, [EBP+8]
    mov edx, 2      ;Incluir o \n
    int 80h

    ;Print da quantidade de bytes lidos
    call _PrintBytesLidos

    pop edx
    pop ecx
    pop ebx

    leave
    ret 4

;Parametro: endereco de memoria de onde pegar o char a escrever
_EscreverChar:  
    enter 0,0
    push ebx
    push ecx
    push edx

    mov eax, 4
    mov ebx, 1
    mov ecx, [EBP+8]
    mov edx, 4
    int 80h

    pop edx
    pop ecx
    pop ebx

    leave
    ret 4

;Param1: endereco de memoria onde inserir a leitura
;Param2: quantos bytes ler
_LeerString:
    enter 0,0

    push ebx
    push ecx
    push edx

    mov eax, 3
    mov ebx, 0
    mov ecx, [EBP+12]
    mov edx, [EBP+8]
    int 80h

    ;Print da quantidade de bytes lidos
    call _PrintBytesLidos

    pop edx
    pop ecx
    pop ebx

    leave
    ret 8

;Param1: endereco de onde comeca a string
;Param2: tamanho da mesma, em bytes
_EscreverString:
    enter 0,0
    push ebx
    push ecx
    push edx

    mov eax, 4
    mov ebx, 1
    mov ecx, [EBP+12]
    mov edx, [EBP+8]
    int 80h

    pop edx
    pop ecx
    pop ebx

    leave
    ret 8


global _start
_start: 

push eax
push dword OLD_DATA
call _LeerInteiro
mov [OLD_DATA], eax
pop eax
mov eax, [OLD_DATA]
L1:push edx
cdq
idiv dword [DOIS]
pop edx
mov [NEW_DATA], eax
push edx
imul dword [DOIS]
jo _Overflow
pop edx
mov [TMP_DATA], eax
mov eax, [OLD_DATA]
sub eax, [TMP_DATA]
mov [TMP_DATA], eax
push eax
push dword TMP_DATA
call _EscreverInteiro
pop eax
push ebx
mov ebx, [NEW_DATA]
mov [OLD_DATA], ebx
pop ebx
mov eax, [OLD_DATA]
cmp eax, 0
jg L1
mov eax, 1
mov ebx, 0
int 80h
