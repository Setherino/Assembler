mov bx 10
mov cx 0
fun [FN] 2 0 100
mov ax [9]
add cx ax
add bx -1
cmp bx 0
ja [4]
mov ax cx
put
halt


; Function param
FP

; Function. Takes range and loops through inputs until it gets value within range.
FN
; This function takes a number & a range. Returns number if it's within range
mov bx [FP]
mov cx [bx+1]
mov dx [bx+2]
; Get next
GN
get
cmp ax cx
jb [GN]
cmp ax dx
ja [GN]
ret