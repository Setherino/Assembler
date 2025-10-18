; Program for linked lists by Seth Ciancio
; Fills linked list to a certain number of items (entered by user), and then prints it back.
; Get desired length of list
get
mov [MV] ax
; Create and print list
fun [CL] 2 [LS] [MV]
fun [PL] 1 [LS]
halt
; Main variable - variable for the starting bit of code
MV




; Createlist parameter
CP

; Createlist. Takes a location to put the list and a desired length
CL
mov bx [CP]
mov bx [bx+2]
mov cx [bx]
mov bx [CP]
mov bx [bx+1]
mov dx bx
; CreateListLoop start
CS
cmp cx 0
jbe [CE]
; Move BX to the next item in the list
mov bx dx
get
; I'm adding three instead of two just so there's a gap between...
add dx 3
; Put the address of the next list item & the value into the list
mov [bx] dx
mov [bx+1] ax
add cx -1
jmp [CS]
; Createlist end
CE
; So we know the list is over.
mov ax -1
mov [bx] ax4
ret




; Printlist parameter
PP

; Print List function. Takes the start of the list
PL
mov bx [PP]
mov bx [bx+1]
; For finding the end of the list later
mov cx -1
; Printlistloop Start
PS
mov ax [bx+1]
put
; Check if the list is over
cmp cx [bx]
mov bx [bx]
jne [PS]
ret



; List start
LS
