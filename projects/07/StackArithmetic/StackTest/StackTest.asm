@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
D=D-M
M=0
@cmp_lbl_1
D;JNE
@SP
A=M-1
A=A-1
M=-1
(cmp_lbl_1)
@SP
M=M-1
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
D=D-M
M=0
@cmp_lbl_2
D;JNE
@SP
A=M-1
A=A-1
M=-1
(cmp_lbl_2)
@SP
M=M-1
@16
D=A
@SP
A=M
M=D
@SP
M=M+1
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
D=D-M
M=0
@cmp_lbl_3
D;JNE
@SP
A=M-1
A=A-1
M=-1
(cmp_lbl_3)
@SP
M=M-1
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
D=M-D
M=0
@cmp_lbl_4
D;JGE
@SP
A=M-1
A=A-1
M=-1
(cmp_lbl_4)
@SP
M=M-1
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@892
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
D=M-D
M=0
@cmp_lbl_5
D;JGE
@SP
A=M-1
A=A-1
M=-1
(cmp_lbl_5)
@SP
M=M-1
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@891
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
D=M-D
M=0
@cmp_lbl_6
D;JGE
@SP
A=M-1
A=A-1
M=-1
(cmp_lbl_6)
@SP
M=M-1
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
D=M-D
M=0
@cmp_lbl_7
D;JLE
@SP
A=M-1
A=A-1
M=-1
(cmp_lbl_7)
@SP
M=M-1
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
D=M-D
M=0
@cmp_lbl_8
D;JLE
@SP
A=M-1
A=A-1
M=-1
(cmp_lbl_8)
@SP
M=M-1
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
D=M-D
M=0
@cmp_lbl_9
D;JLE
@SP
A=M-1
A=A-1
M=-1
(cmp_lbl_9)
@SP
M=M-1
@57
D=A
@SP
A=M
M=D
@SP
M=M+1
@31
D=A
@SP
A=M
M=D
@SP
M=M+1
@53
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
M=D+M
@SP
M=M-1
@112
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
M=M-D
@SP
M=M-1
@SP
A=M-1
D=M
M=-D
@SP
A=M-1
D=M
A=A-1
M=D&M
@SP
M=M-1
@82
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
A=M-1
D=M
A=A-1
M=D|M
@SP
M=M-1
@SP
A=M-1
D=M
M=!D
