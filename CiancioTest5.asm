mov cx 20
mov [100] cx
mov dx 0
mov [101] dx
mov bx [100]
add bx [101]
mov [102] bx
mov ax bx
mov ax 0
put
add ax 1
cmp ax cx
jbe [17]
add ax -1
cmp ax dx
put
ja [23]
add ax -5
cmp ax -15
put
jae [29]
cmp ax -20
jne [18]
mov bx 20
cmp ax bx
jb [47]
mov bx 1000
add ax bx
cmp ax bx
jb [47]
cmp ax [100]
je [57]
mov ax 999
jmp [61]
mov dx 999
halt