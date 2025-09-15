; Program to take values and then print them backwards
; Get the number of values to enter.
get
; Check size (for memory safety!)
cmp ax 18
ja [EH]
mov [MV] ax
; Get & print all values
fun [FN] 1 [MV]
; Put the number of values, just so the lists are perfect opposite.
put
; Early Halt (for memory safety)
EH
halt
; Main ( like main() function ) variable
MV


; Function parameter
FP

; Function
FN
; get the pointer to the pointer to the value we want.
mov bx [FP]
; get the pointer to the value we want
mov bx [bx+1]
; get the value we want (the counter)
mov cx [bx]
get
; DEC counter
add cx -1
; Store counter to pass forward
mov [VR] cx
cmp cx 0
JBE [EN]
fun [FN] 1 [VR]
; END (of function)
EN
put
ret

; Variable for storage of the counter between functions
VR