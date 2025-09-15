get
cmp ax 18
ja [13]
mov [14] ax
fun [17] 1 [14]
put
halt



mov bx [16]
mov bx [bx+1]
mov cx [bx]
get
add cx -1
mov [39] cx
cmp cx 0
JBE [36]
fun [17] 1 [39]
put
ret

