mov bx 10
mov cx 0
get
add cx ax
add bx -1
cmp bx 0
ja [4]
mov [17] cx
mov ax cx
put
halt