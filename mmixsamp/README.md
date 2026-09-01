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


## TODO
- TBD