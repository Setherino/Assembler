Problems 3: 
Problems: in the RunMachineCode Function, the second parameter was 8 times what it should be, 
because I never shifted it to the left. I fixed this by dividing it by 8, putting it back into 
the ones place.

Problems 3-2: 
Lots of issues with returning from the whichReg() function when it's fed something 
other than a register. At first it returned -1 to indicate an error. This resulted in a 159 
being put into the command buffer. Then I had it return 5, for a constant. Then I realiszed 
the add function doesn't need that, so I had it return nothing. Then I had undefined behaviour, 
so now it returns zero.

Problems 3-3: 
Lots of different issues with putting a value at a particular memory address.

Problem 4-1: 
I spent like an hour and a half trying to get MOV reg address to work, because I had
a single unnecessary "address++" after accessing the memory location with the value in it. This
wasn't part of the runtime, so it didn't need the address++. And because it just skipped
whatever line was in front of it, it's behevaiour was super unpredictible. At one point while testing 
it, I removed everything from the .asm file except the MOV REG ADDRESS and the HALT to make the testing
faster. At that point, I started getting exceptions! I was so confused as to how that happened. I thought 
somehow I had messed up one of the functions, so I'm going back and fourth, tripple checking the 
runMachineCode() function and the convertToMachineCode() like- what is wrong with these?? Of course, 
nothing was wrong with either of them. The problem was that it was skipping the HALT command and 
overruning forever. So I added a warning for overruns.

5-1: 
I had an issue with negative numbers, where paramDecode was comparing "param2 != '-'" instead of
"param2[0] != '-'". That meant it could not deal with negative constants.

5-2: 
I actually had no issues with my code, but I did struggle with my .asm file. I kept mixing up the 
JA/JB and JAE/JBE instructions, so something would be equal but not greater and it would (or would't)
jump when I expected it to do the opposite. I eventually made a new memorydump function that prints 
the memory in a way which is much more readible, and then I went step-by-step through my .asm program 
in order to debug it. 

6-0 (Correcting mistakes in 5):
The reason my asm had an infinite loop in your compiler was because it didn't have brackets around the jumps.

6-1:
I had no issues other than minor syntax stuff.

7-0:
Off by one error: Are you supposed to run the code at the point you jump to? 

Problems 7: There were really a lot of small problems. I'm just going to list a few.

7-1:
I  had the RET function pulling AX from the  stack before it put AX into the return location. That one was 
pretty easy, since I knew the problem was going to be in the runRET function, and I'd already had other 
issues with the order of the stack. 

7-2:
I had an issue with the way [BX+xxx] was implemented, because I'd initially assumed we were supposed to put spaces
on either side of the plus [BX + xxx]. That broke splitCommand, and almost forced me to either redesign the entire 
assembler, or create some truly horrible bodge. I remember the sinking feeling of dread as I tried to come up with
some solution that wouldn't be horrible. that's when I had the thought, "we don't use spaces in anything." I think
it's really interesting how such a small assumption can run incredibly deep throughout piece of computer software.

7-3:
I had so many issues with my memoryDumpReadable() function, and it still totally doesn't work. I know I shouldn't be focusing
on it, but I find it to be a lot of fun to work on, and I like it when it works. With that said, in heindsight, it
might have actually been better before. Now it relies on some complex patchwork of overlapping state that's nearly
impossible for me to get my head around. It's almost like I couldn't have designed it worse if I tried. Like the functions
set their parameters which set the addresses they point to? This means that a single value being somewhere the function
doesn't expect can lead to strange and unpredictible casade failures, causing it to read half your program
as if it's a signle function with a hundred parameters. That said, it has no impact on the running of the code, so
it doesn't really matter if it goes totally whack. And it's correct enough that it's still very helpful for me.

8-0:
I finally made the memoryDumpReadable() function good, and changed the way the compiler works.

8-1:
Most of my issues were just with the addresses pointed to by the program. Although I also had to change the register
used to count the number of numbers entered from bx to dx so I could access memory within the loop. Also I accidentally
tried to do a "mov [100] 0", which of course is not possible.

9-1:
I had a bug where there would be an infinite loop if there wasn't an endline at the end of the file, because the getLineWithoutTrash
function would just keep looking for endlines over and over again. Fixed that, and also made that function finally work.

10-1:
This one is fun: I wasn't initializing the param1 & param2 variables. This was fine with most ASM programs, which open with some sort of mov
instruction. But my linked list program opens with a get, so it crashed when it tried to do a getType() on an uninitialized variable. So
I just initialize them to whatever and that works fine.


Problem log 2.0, Web-Assembly
10/24/25:
fopen is windows specific, we'll need to do some restructuring to make our filesystem work on the web.
For now, I'll do it the easy way. (see: data.h)

10/25/25:
The easy way is harder than I thought it would be, but I do want to keep as much of the code in main.c the same,
because (1) it already works and changing it would be a whole different challenge, and (2) we could imagine a much
larger and more complex "enterprise" codebase, in which creating a compatible API really is by far the best solution
to a re-compilation / porting project like this, even if that wouldn't be what you'd do if you started from scratch.

While there were a few issues I dealt with, like sgets() returning lines with no newline, or how sgets() needs to seperately
keep track of its position in a file and its position in the output string, what really stumped me for over an hour were
syntax errors that I introduced into my test file when coppying it into the sfile struct.
