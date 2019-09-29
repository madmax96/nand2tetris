// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.

@R1
D=M
@R2
M=D // load second number in result memory R2
@END
D;JEQ // if second number is 0 then END

@R0
D=M
@R2
M=D // load first number in result memory R2
@END
D;JEQ // if first number is 0 then END

(LOOP)
    @R1
    D=M-1
    @END
    D;JEQ
    @R1
    M=M-1 // decrease second num by 1
    @R0
    D=M
    @R2
    M=M+D // increase result by first num
    @LOOP
    0;JMP
(END)
    @END
    0;JMP
