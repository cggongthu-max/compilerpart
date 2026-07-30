# Compilerpart

Simple arithmetic compilation (below 10) to MMIX target (十以内的四则混合运算编译到MMIX) - simple-arith.c
Sample input: "3*(5-2)"
Sample output: simple-arith-out/simple-arith-out0.mms
       LOC	#100
Main	SET	$1,3
	SET	$2,5
	SET	$3,2
	SUB	$2,$2,$3
	MUL	$1,$1,$2
	TRAP	0,0,0

To compile: invoke MMIX toolchain
chengguang-macmini simple-arith-out % ~/Downloads/mmix-20131017/mmixal simple-arith-out0.mms               
chengguang-macmini simple-arith-out % ~/Downloads/mmix-20131017/mmix -t1 simple-arith-out0
         1. 0000000000000100: e3010003 (SETL) $1=l[1] = #3
         1. 0000000000000104: e3020005 (SETL) rL=3, $2=l[2] = #5
         1. 0000000000000108: e3030002 (SETL) rL=4, $3=l[3] = #2
         1. 000000000000010c: 24020203 (SUB) $2=l[2] = 5 - 2 = 3
         1. 0000000000000110: 18010102 (MUL) $1=l[1] = 3 * 3 = 9
         1. 0000000000000114: 00000000 (TRAP) Halt(0)
  6 instructions, 0 mems, 19 oops; 0 good guesses, 0 bad
  (halted at location #0000000000000114)

Check the result value is 9.

TODO 下趟: int s;int i;s=0;i=1;while(i<=100){
  s=s+i;i=i+1;
}
