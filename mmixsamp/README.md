# Mmixsamp

The `Mmixsamp` folder contains some MMIX sample programs.

## Max.mms

**compile to MMIX**
```c
int a=3, b=7;
// take maximum of a and b, store at a
if (a<b)
    a=b;
```

**Testing**
```bash
mmixsamp % ~/Downloads/mmix-20131017/mmixal max.mms
mmixsamp % ~/Downloads/mmix-20131017/mmix -t1 max  
         1. 0000000000000100: e3c80003 (SETL) rL=201, $200=l[200] = #3
         1. 0000000000000104: c102c800 (ORI) $2=l[2] = 3 = #3
         1. 0000000000000108: e3c90007 (SETL) rL=202, $201=l[201] = #7
         1. 000000000000010c: c103c900 (ORI) $3=l[3] = 7 = #7
         1. 0000000000000110: 30040203 (CMP) $4=l[4] = 3 cmp 7 = -1
         1. 0000000000000114: 44040002 (BP) -1>0?  No
         1. 0000000000000118: c1020300 (ORI) $2=l[2] = 7 = #7
         1. 000000000000011c: 00000000 (TRAP) Halt(0)
  8 instructions, 0 mems, 12 oops; 1 good guess, 0 bad
  (halted at location #000000000000011c)
```
Verify that $2 (`a`) is set to 7.

Then in `max.mms`, you could change the line
```SET $200,3```
to ```SET $200,13```
and reassembble.
```bash
mmixsamp % ~/Downloads/mmix-20131017/mmixal max.mms
mmixsamp % ~/Downloads/mmix-20131017/mmix -t1 max  
         1. 0000000000000100: e3c8000d (SETL) rL=201, $200=l[200] = #d
         1. 0000000000000104: c102c800 (ORI) $2=l[2] = 13 = #d
         1. 0000000000000108: e3c90007 (SETL) rL=202, $201=l[201] = #7
         1. 000000000000010c: c103c900 (ORI) $3=l[3] = 7 = #7
         1. 0000000000000110: 30040203 (CMP) $4=l[4] = 13 cmp 7 = 1
         1. 0000000000000114: 44040002 (BP) 1>0?  Yes, -> #11c (bad guess)
         1. 000000000000011c: 00000000 (TRAP) Halt(0)
  7 instructions, 0 mems, 13 oops; 0 good guesses, 1 bad
```
And verify that the branch takes effect and $2 (`a`) is still the maximum value 13.

## Maxa2.mms

**compile to MMIX**
```c
int a[] = {12,4,8,9,42};
int max=0, i;
for(i=0;i<5;i++)
  if (a[i]>max)
    max=a;
```

**Testing**
In `maxa2.mms` the looping variable `max` uses `$1`, and at the end of the loop `$8` is set to `$1`.
This is a trick to inspect it more easily for testing.
```bash
mmixsamp % ~/Downloads/mmix-20131017/mmixal maxa2.mms
chengguang-macmini@chengguang-macminideMac-mini mmixsamp % ~/Downloads/mmix-20131017/mmix -t1 maxa2  
         1. 0000000000000100: e3030000 (SETL) rL=4, $3=l[3] = #0
         1. 0000000000000104: 2302fe00 (ADDUI) $2=l[2] = 2305843009213693952 = #2000000000000000
         1. 0000000000000108: e3010000 (SETL) $1=l[1] = #0
         1. 000000000000010c: 8c040203 (LDO) rL=5, $4=l[4] = M8[#2000000000000000+#0] = 12
         1. 0000000000000110: 30050104 (CMP) rL=6, $5=l[5] = 0 cmp 12 = -1
         1. 0000000000000114: 44050002 (BP) -1>0?  No
         1. 0000000000000118: c1010400 (ORI) $1=l[1] = 12 = #c
         1. 000000000000011c: e3060008 (SETL) rL=7, $6=l[6] = #8
         1. 0000000000000120: 20030306 (ADD) $3=l[3] = 0 + 8 = 8
         1. 0000000000000124: 31070328 (CMPI) rL=8, $7=l[7] = 8 cmp 40 = -1
         1. 0000000000000128: 4107fff9 (BNB) -1<0?  Yes, -> #10c (bad guess)
 ...............................................
         1. 000000000000012c: c1080100 (ORI) rL=9, $8=l[8] = 42 = #2a
         1. 0000000000000130: 00000000 (TRAP) Halt(0)
  42 instructions, 5 mems, 60 oops; 3 good guesses, 7 bad
  (halted at location #0000000000000130)
```
Verify that $8 (`max`) is set to 8.


## TODO
- TBD