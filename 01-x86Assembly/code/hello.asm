section .data

section .text

global main

main:
    mov eax, 1
    inc eax
    mov ebx, eax

mov eax, 1
mov ebx, 0
int 80h
