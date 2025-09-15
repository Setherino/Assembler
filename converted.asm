mov dx 10
mov cx 0
get
mov [68] ax
fun [35] 3
mov bx [10]
cmp bx 0
je [24] 
mov ax [bx]
add cx ax
add dx -1
cmp dx 0
ja [4] 
mov ax cx
put
halt



mov bx [33]
mov bx [bx+1]
mov ax [bx]
mov bx [33]
mov cx [bx+2]
mov dx [bx+3]
cmp ax cx
jb [58] 
cmp ax dx
ja [58] 
mov [100] ax
mov ax 100
ret
mov bx 0
mov [100] bx
mov ax 100
ret

0
0
