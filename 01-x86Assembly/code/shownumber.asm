;show 65535 
init:
    mov ax,0xb800          ; 0xb800 显存地址
    mov es,ax              ;将显存段地址放到 es 中，后续只使用 es+偏移 就行

xor dx,dx                  ;异或置 0
mov ax,65535
mov bx,10
div bx

xor dx,dx 
div bx
add dl,0x30
mov byte [es:0x00],dl
mov byte [es:0x01],0x70

xor dx,dx 
div bx
add dl,0x30
mov byte [es:0x02],dl
mov byte [es:0x03],0x70

xor dx,dx 
div bx
add dl,0x30
mov byte [es:0x04],dl
mov byte [es:0x05],0x70

xor dx,dx 
div bx
add dl,0x30
mov byte [es:0x06],dl
mov byte [es:0x07],0x70

xor dx,dx 
div bx
add dl,0x30
mov byte [es:0x08],dl
mov byte [es:0x09],0x70

number db 0,0,0,0,0

current:
    times 510-(current-init) db 0

db 0x55,0xaa 