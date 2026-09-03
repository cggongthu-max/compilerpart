# Mmixsamp/rope

The `Mmixsamp/rope` subfolder contains some programs related to `rope`.

## Ranges-overlap.mms

**compile to MMIX**
```c
int range_overlaps(int von0,int bis0,int von,int bis)
{
  return von0==von || (bis0>von&&
          bis>von0);
}
```

**Testing**
```bash
rope % ~/Downloads/mmix-20131017/mmixal ranges-overlap.mms
rope % ~/Downloads/mmix-20131017/mmix -t1 ranges-overlap  
         1. 0000000000000100: 2305fe00 (ADDUI) rL=6, $5=l[5] = 2305843009213693952 = #2000000000000000
         1. 0000000000000104: e3010008 (SETL) $1=l[1] = #8
         1. 0000000000000108: e3020010 (SETL) $2=l[2] = #10
         1. 000000000000010c: e3030018 (SETL) $3=l[3] = #18
         1. 0000000000000110: 8d060500 (LDOI) rL=7, $6=l[6] = M8[#2000000000000000] = 0
         1. 0000000000000114: 8c070501 (LDO) rL=8, $7=l[7] = M8[#2000000000000000+#8] = 3
         1. 0000000000000118: 8c080502 (LDO) rL=9, $8=l[8] = M8[#2000000000000000+#10] = 3
         1. 000000000000011c: 8c090503 (LDO) rL=10, $9=l[9] = M8[#2000000000000000+#18] = 5
         1. 0000000000000120: 300b0608 (CMP) rL=12, $11=l[11] = 0 cmp 3 = -1
         1. 0000000000000124: 63150b01 (CSZI) rL=22, $21=l[21] = -1==0? 1: 0 = 0
         1. 0000000000000128: 300c0708 (CMP) $12=l[12] = 3 cmp 3 = 0
         1. 000000000000012c: 300d0906 (CMP) $13=l[13] = 5 cmp 0 = 1
         1. 0000000000000130: 200e0c0d (ADD) $14=l[14] = 0 + 1 = 1
         1. 0000000000000134: e30f0002 (SETL) $15=l[15] = #2
         1. 0000000000000138: 30100e0f (CMP) $16=l[16] = 1 cmp 2 = -1
         1. 000000000000013c: 4a100002 (BNZ) -1!=0?  Yes, -> #144 (bad guess)
         1. 0000000000000144: 00000000 (TRAP) Halt(0)
  17 instructions, 4 mems, 23 oops; 0 good guesses, 1 bad
  (halted at location #0000000000000144)
```
Verify that $21 (_return value_) is set to `0`.

Then in `max.mms`, change input args to `2 7 4 9`.
```bash
rope % ~/Downloads/mmix-20131017/mmixal ranges-overlap.mms
rope % ~/Downloads/mmix-20131017/mmix -t1 ranges-overlap  
         1. 0000000000000100: 2305fe00 (ADDUI) rL=6, $5=l[5] = 2305843009213693952 = #2000000000000000
         1. 0000000000000104: e3010008 (SETL) $1=l[1] = #8
         1. 0000000000000108: e3020010 (SETL) $2=l[2] = #10
         1. 000000000000010c: e3030018 (SETL) $3=l[3] = #18
         1. 0000000000000110: 8d060500 (LDOI) rL=7, $6=l[6] = M8[#2000000000000000] = 2
         1. 0000000000000114: 8c070501 (LDO) rL=8, $7=l[7] = M8[#2000000000000000+#8] = 7
         1. 0000000000000118: 8c080502 (LDO) rL=9, $8=l[8] = M8[#2000000000000000+#10] = 4
         1. 000000000000011c: 8c090503 (LDO) rL=10, $9=l[9] = M8[#2000000000000000+#18] = 9
         1. 0000000000000120: 300b0608 (CMP) rL=12, $11=l[11] = 2 cmp 4 = -1
         1. 0000000000000124: 63150b01 (CSZI) rL=22, $21=l[21] = -1==0? 1: 0 = 0
         1. 0000000000000128: 300c0708 (CMP) $12=l[12] = 7 cmp 4 = 1
         1. 000000000000012c: 300d0906 (CMP) $13=l[13] = 9 cmp 2 = 1
         1. 0000000000000130: 200e0c0d (ADD) $14=l[14] = 1 + 1 = 2
         1. 0000000000000134: e30f0002 (SETL) $15=l[15] = #2
         1. 0000000000000138: 30100e0f (CMP) $16=l[16] = 2 cmp 2 = 0
         1. 000000000000013c: 4a100002 (BNZ) 0!=0?  No
         1. 0000000000000140: e3150001 (SETL) $21=l[21] = #1
         1. 0000000000000144: 00000000 (TRAP) Halt(0)
  18 instructions, 4 mems, 22 oops; 1 good guess, 0 bad
  (halted at location #0000000000000144)
```
And verify that $21 (_return value_) is set to `1`, which means the ranges [2,7), [4,9) overlap.


## TODO
- TBD