fun [22] 2 [119] 10fun [66] 2 [119] 10fun [44] 2 [119] 10



mov bx [21]mov cx [bx+2]mov bx [bx+1]add cx bxadd cx -1cmp bx cxja [40]getmov [bx] axadd bx 1jmp [31]ret


mov bx [43]mov cx [bx+2]mov bx [bx+1]add cx bxadd cx -1cmp bx cxja [62]mov ax [bx]putadd bx 1jmp [53]ret


mov bx [65]mov dx [bx+2]mov bx [bx+1]add bx -1add dx bxmov ax 0mov [106] axadd bx 1cmp bx dxje [99]mov ax [bx]mov cx [bx+1]cmp ax cxjbe [79]mov [bx+1] axmov [bx] cxmov ax 1mov [106] axjmp [79]mov ax 0cmp ax [106]JNE [66]ret












