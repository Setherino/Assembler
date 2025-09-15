get
mov [15] ax
fun [20] 2 [72] [15]
fun [54] 1 [72]
halt





mov bx [19]
mov bx [bx+2]
mov cx [bx]
mov bx [19]
mov bx [bx+1]
mov dx bx
cmp cx 0
jbe [45]
mov bx dx
get
add dx 3
mov [bx] dx
mov [bx+1] ax
add cx -1
jmp [30]
mov ax -1
mov [bx] ax
ret





mov bx [53]
mov bx [bx+1]
mov cx -1
mov ax [bx+1]
put
cmp cx [bx]
mov bx [bx]
jne [60]
ret



