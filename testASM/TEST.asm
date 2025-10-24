mov cx 0
mov dx 0
get
add dx ax
add cx 1
cmp cx 10
jb [4]
mov [50] dx
mov ax dx
put
halt