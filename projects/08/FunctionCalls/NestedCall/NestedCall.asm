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
(Sys.init)
@4000
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
@3
M=D
@SP
M=M-1
@5000
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
@4
M=D
@SP
M=M-1
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
@0 // number of args
D=D-A // SP-=arg_count
@ARG
M=D // ARG = SP-5-arg_count
@Sys.main
0;JMP
(ret_address_1)
@SP
A=M-1
D=M
@6
M=D
@SP
M=M-1
(Sys.init$LOOP)
@Sys.init$LOOP
0;JMP
(Sys.main)
@SP
A=M
M=0
@SP
M=M+1
@SP
A=M
M=0
@SP
M=M+1
@SP
A=M
M=0
@SP
M=M+1
@SP
A=M
M=0
@SP
M=M+1
@SP
A=M
M=0
@SP
M=M+1
@4001
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
@3
M=D
@SP
M=M-1
@5001
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
@4
M=D
@SP
M=M-1
@200
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@1
D=D+A
@14
M=D
@SP
A=M-1
D=M
@14
A=M
M=D
@SP
M=M-1
@40
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@2
D=D+A
@14
M=D
@SP
A=M-1
D=M
@14
A=M
M=D
@SP
M=M-1
@6
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@3
D=D+A
@14
M=D
@SP
A=M-1
D=M
@14
A=M
M=D
@SP
M=M-1
@123
D=A
@SP
A=M
M=D
@SP
M=M+1
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
@Sys.add12
0;JMP
(ret_address_2)
@SP
A=M-1
D=M
@5
M=D
@SP
M=M-1
@LCL
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@2
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@3
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@4
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
M=D+M
@SP
AM=M-1
D=M
A=A-1
M=D+M
@SP
AM=M-1
D=M
A=A-1
M=D+M
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
(Sys.add12)
@4002
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
@3
M=D
@SP
M=M-1
@5002
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
@4
M=D
@SP
M=M-1
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
@12
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
