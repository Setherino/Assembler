mov dx 10
mov cx 0
fun [25] 2 0 100
mov bx [9]
add cx [bx]
add dx -1
cmp dx 0
ja [4]
mov ax cx
put
halt



mov bx [24]
mov cx [bx+1]
mov dx [bx+2]
get
cmp ax cx
jb [31]
cmp ax dx
ja [31]
mov [44] ax
mov ax 44
ret

