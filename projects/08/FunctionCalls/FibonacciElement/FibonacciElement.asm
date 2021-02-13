@256
D=A
@SP
M=D
@ret_address_0
D=A
@SP
A=M
M=D // push RETURN address
@SP
M=M+1 // inc SP
@LCL
D=M
@SP
A=M
M=D // push LCL
@SP
M=M+1 // inc SP
@ARG
D=M
@SP
A=M
M=D // push ARG
@SP
M=M+1 // inc SP
@THIS
D=M
@SP
A=M
M=D // push THIS
@SP
M=M+1 // inc SP
@THAT
D=M
@SP
A=M
M=D // push THAT
@SP
MD=M+1 // inc SP
@LCL
M=D // LCL=SP
@5
D=D-A // SP-=5
@0 // number of args
D=D-A // SP-=arg_count
@ARG
M=D // ARG = SP-5-arg_count
@Sys.init
0;JMP
(ret_address_0)
(Main.fibonacci)
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
M=0
@cmp_lbl_0
D;JGE
@SP
A=M-1
M=-1
(cmp_lbl_0)
@SP
AM=M-1
D=M
@Main.fibonacci$IF_TRUE
D;JNE
@Main.fibonacci$IF_FALSE
0;JMP
(Main.fibonacci$IF_TRUE)
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@5
D=A
@LCL
A=M-D
D=M // D=return_address
@R13
M=D // saving return_address
@SP
A=M-1
D=M
@ARG
A=M
M=D // *ARG = *(SP - 1)
@ARG
D=M
@SP
M=D+1 // SP=ARG+1
@LCL
AM=M-1
D=M
@THAT
M=D // restoring THAT
@LCL
AM=M-1
D=M
@THIS
M=D // restoring THIS
@LCL
AM=M-1
D=M
@ARG
M=D // restoring ARG
@LCL
A=M-1
D=M
@LCL
M=D // restoring LCL
@R13
A=M
0;JMP
(Main.fibonacci$IF_FALSE)
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
M=M-D
@ret_address_1
D=A
@SP
A=M
M=D // push RETURN address
@SP
M=M+1 // inc SP
@LCL
D=M
@SP
A=M
M=D // push LCL
@SP
M=M+1 // inc SP
@ARG
D=M
@SP
A=M
M=D // push ARG
@SP
M=M+1 // inc SP
@THIS
D=M
@SP
A=M
M=D // push THIS
@SP
M=M+1 // inc SP
@THAT
D=M
@SP
A=M
M=D // push THAT
@SP
MD=M+1 // inc SP
@LCL
M=D // LCL=SP
@5
D=D-A // SP-=5
@1 // number of args
D=D-A // SP-=arg_count
@ARG
M=D // ARG = SP-5-arg_count
@Main.fibonacci
0;JMP
(ret_address_1)
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
M=M-D
@ret_address_2
D=A
@SP
A=M
M=D // push RETURN address
@SP
M=M+1 // inc SP
@LCL
D=M
@SP
A=M
M=D // push LCL
@SP
M=M+1 // inc SP
@ARG
D=M
@SP
A=M
M=D // push ARG
@SP
M=M+1 // inc SP
@THIS
D=M
@SP
A=M
M=D // push THIS
@SP
M=M+1 // inc SP
@THAT
D=M
@SP
A=M
M=D // push THAT
@SP
MD=M+1 // inc SP
@LCL
M=D // LCL=SP
@5
D=D-A // SP-=5
@1 // number of args
D=D-A // SP-=arg_count
@ARG
M=D // ARG = SP-5-arg_count
@Main.fibonacci
0;JMP
(ret_address_2)
@SP
AM=M-1
D=M
A=A-1
M=D+M
@5
D=A
@LCL
A=M-D
D=M // D=return_address
@R13
M=D // saving return_address
@SP
A=M-1
D=M
@ARG
A=M
M=D // *ARG = *(SP - 1)
@ARG
D=M
@SP
M=D+1 // SP=ARG+1
@LCL
AM=M-1
D=M
@THAT
M=D // restoring THAT
@LCL
AM=M-1
D=M
@THIS
M=D // restoring THIS
@LCL
AM=M-1
D=M
@ARG
M=D // restoring ARG
@LCL
A=M-1
D=M
@LCL
M=D // restoring LCL
@R13
A=M
0;JMP
(Sys.init)
@4
D=A
@SP
A=M
M=D
@SP
M=M+1
@ret_address_3
D=A
@SP
A=M
M=D // push RETURN address
@SP
M=M+1 // inc SP
@LCL
D=M
@SP
A=M
M=D // push LCL
@SP
M=M+1 // inc SP
@ARG
D=M
@SP
A=M
M=D // push ARG
@SP
M=M+1 // inc SP
@THIS
D=M
@SP
A=M
M=D // push THIS
@SP
M=M+1 // inc SP
@THAT
D=M
@SP
A=M
M=D // push THAT
@SP
MD=M+1 // inc SP
@LCL
M=D // LCL=SP
@5
D=D-A // SP-=5
@1 // number of args
D=D-A // SP-=arg_count
@ARG
M=D // ARG = SP-5-arg_count
@Main.fibonacci
0;JMP
(ret_address_3)
(Sys.init$WHILE)
@Sys.init$WHILE
0;JMP
