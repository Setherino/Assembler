; Program to create and sort an array.
; Create array based on user input
fun [CA] 2 [AS] 10
; Sort array w/ bubble sort
fun [SA] 2 [AS] 10
; Print sorted array.
fun [PA] 2 [AS] 10
halt


;CreateArray parameter
CP

;Create Array function, takes the location and size of the array
CA
; Get array size
mov bx [CP]
mov cx [bx+2]
; Get array start location
mov bx [bx+1]
; Add to get array end location
add cx bx
; So we don't go overboard
add cx -1
;start of loop
NI
cmp bx cx
ja [CE]
get
mov [bx] ax
add bx 1
jmp [NI]
; CreateArray End
CE
ret


;PrintArray parameter
PP

;Print Array function, takes the location and size of the array
PA
; Get array size
mov bx [PP]
mov cx [bx+2]
; Get array start location
mov bx [bx+1]
; Get array end location
add cx bx
; So we don't go overboard
add cx -1
;start of loop
NO
cmp bx cx
ja [PE]
mov ax [bx]
put
add bx 1
jmp [NO]
;PrintArray End
PE
ret


; SortArray parameter
SP

; SortArray function, takes the location and size of the array
SA
; Get parameters
mov bx [SP]
; Array size
mov dx [bx+2]
; Array start location
mov bx [bx+1]
; Offset the first add in the loop
add bx -1
; Array end location
add dx bx
; Reset sortVariable
mov ax 0
mov [SV] ax
; Sort next (main loop)
SN
add bx 1
; Check if we're at the end of the array
cmp bx dx
je [SE]
; Get two items from array
mov ax [bx]
mov cx [bx+1]
cmp ax cx
; If first is smaller, go to the next one
jbe [SN]
; If A is bigger, swap the values.
mov [bx+1] ax
mov [bx] cx
; Take note that we had to swap
mov ax 1
mov [SV] ax
; Check the next one
jmp [SN]
; Sortloop End
SE
; Check if we had to swap anything
mov ax 0
cmp ax [SV]
JNE [SA]
ret
; Sort(swap?) variable
SV













; Array start
AS
13
41
22
66
22
12
66
11
4
12