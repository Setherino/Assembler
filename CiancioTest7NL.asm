mov bx 10
mov cx 0
fun [25] 2 0 100
mov ax [9]
add cx ax
add bx -1
cmp bx 0
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
ret GN]
