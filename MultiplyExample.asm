get
mov [50] ax
get
mov [51] ax
fun [20] 3 [50] [51] [52]
mov ax [52]
put
halt


;this gap is here so that the function is in the right place.

mov cx 1
mov bx [19]
mov bx [bx+1]
mov dx [bx]
mov ax dx
mov bx [19]
mov bx [bx+2]
mov bx [bx]
cmp cx bx
jae [41]
add ax dx
add cx 1
jmp [33]
mov bx [19]
mov bx [bx+3]
mov [bx] ax
ret