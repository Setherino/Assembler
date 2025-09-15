fun [22] 2 [119] 10
fun [66] 2 [119] 10
fun [44] 2 [119] 10




mov bx [21]
mov cx [bx+2]
mov bx [bx+1]
add cx bx
add cx -1
cmp bx cx
ja [40]
get
mov [bx] ax
add bx 1
jmp [31]
ret



mov bx [43]
mov cx [bx+2]
mov bx [bx+1]
add cx bx
add cx -1
cmp bx cx
ja [62]
mov ax [bx]
put
add bx 1
jmp [53]
ret



mov bx [65]
mov dx [bx+2]
mov bx [bx+1]
add bx -1
add dx bx
mov ax 0
mov [106] ax
add bx 1
cmp bx dx
je [99]
mov ax [bx]
mov cx [bx+1]
cmp ax cx
jbe [79]
mov [bx+1] ax
mov [bx] cx
mov ax 1
mov [106] ax
jmp [79]
mov ax 0
cmp ax [106]
JNE [66]
ret













