// Find factorial of number in RAM[16] and store it in RAM[R17]
@SP
M=1024 // set stack segment to point to RAM[1024]

@16
D=M
@17
M=1

(fact)
@END
D-1;JLE

@SP
D=M
@sp_temp
M=D
@16
D=M // data
@sp_temp
A=M
M=D // M[SP] = number

@SP
M=M+1

D=M
@sp_temp
M=M+1
M=D

(END)
@END
0;JMP


(MULT)
@R1
D=M // load second number in D register
@R2
M=D // load second number in result memory R2
@END
D;JEQ // if second number is 0 then END

@R0
D=M // load first number in D
@R2
M=D // load first number in result memory R2
@END
D;JEQ // if first number is 0 then END

(LOOP)
@R1
D=M-1
@END
D;JEQ // stop when second number is 1
@R1
M=M-1 // decrease second num by 1
@R0
D=M // load first number in D
@R2
M=M+D // increase result by first num
@LOOP
0;JMP
(END)
@END
0;JMP