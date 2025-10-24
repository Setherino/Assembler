mov dx 10
mov cx 0
get
mov [55] ax
fun [28] 3 [5] 0 100
mov bx [5]
cmp bx 0
je [5]
mov ax [bx]
add cx ax
add dx -1
cmp dx 0
ja [5]
mov ax cx
put
halt



mov bx [5]
mov bx [bx+1]
mov ax [bx]
mov bx [5]
mov cx [bx+2]
mov dx [bx+3]
cmp ax cx
jb [5]
cmp ax dx
ja [5]
mov [100] ax
mov ax 100
ret
mov bx 0
mov [100] bx
mov ax 100
ret

23
17
31
40