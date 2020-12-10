// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed.
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

(LOOP)
    @24576 // memory address for keybord input (contains ASCII code or 0)
    D=M
    @WHITE
    D;JEQ
    @BLACK
    0;JMP

(WHITE)
    @24575 // memory address of last pixel
    D=A
    @end_white
    M=D

    @16384 // memory address of first pixel
    D=A
    @current_white
    M=D
(WHITE_LOOP)
    @current_white
    A=M // deref - load in A value of Memory[A]
    M=0 // loading black pixel into SCREEN[current_white]
    @end_white
    D=M
    @current_white
    D=D-M // calculating current_white - end_white
    @LOOP
    D;JEQ
    @current_white
    M=M+1
    @WHITE_LOOP
    0;JMP

(BLACK)
    @24575
    D=A
    @end_black
    M=D

    @16384
    D=A
    @current_black
    M=D
(BLACK_LOOP)
    @current_black
    A=M
    M=-1 // because binary it is `1111111111111111`
    @end_black
    D=M
    @current_black
    D=D-M
    @LOOP
    D;JEQ
    @current_black
    M=M+1
    @BLACK_LOOP
    0;JMP
