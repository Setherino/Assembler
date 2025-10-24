get
mov [10] ax
fun [15] 2 [66] [10]
halt





mov bx [14]
mov bx [bx+2]
mov cx [bx]
mov bx [14]
mov bx [bx+1]
mov dx bx
cmp cx 0
jbe [40]
mov bx dx
get
add dx 3
mov [bx] dx
mov [bx+1] ax
add cx -1
jmp [25]
mov ax -1
mov [bx] ax4
ret





mov bx [48]
mov bx [bx+1]
mov cx -1
mov ax [bx+1]
put
cmp cx [bx]
mov bx [bx]
jne [55]
ret



