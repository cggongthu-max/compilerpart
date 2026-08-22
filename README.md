# Compilerpart

The Compilerpart project is an experimental mini C compiler (`JCC0`) to MMIX
assembly.

With the self-lifting goal in mind, software dependencies are minimized---
it requires only the mmix toolchain, a C compiler, and `make`.

Albeit `JCC0` is currently bare bones and incomplete, it is able
to compile some programs. You're welcome to submit programs that you
would like to compile as a feature request.

**Input:**
```c
int ret;
int s; int i;
s = 0; i = 1;
while (i <= 100) {
    s = s + i;
    i = i + 1;
}
ret = s; /* under debug mode `ret` would be printed */
```

**Build & Run:**
```bash
cd jcc0
make debug # debug mode would print the var ret
./jcc02m input.c      # or compile from file
```

---

## Testing

Use `debug` mode to test the compiler and inspect the `ret` variable.
```
compilerpart % jcc0/jcc02m cctest/i100-sum.c >i100-sum-out/i100-sum-jcc0-mdebug-out.mms
compilerpart % ~/Downloads/mmix-20131017/mmixal i100-sum-out/i100-sum-jcc0-mdebug-out.mms
compilerpart % ~/Downloads/mmix-20131017/mmix i100-sum-out/i100-sum-jcc0-mdebug-out   
ret = 5050
```


End-to-end testing can also be achieved through inspecting registers in the MMIX simulator.

1. **Assemble** the generated `.mms` file with `mmixal`.
2. **Run** with `mmix -t1` (trace mode) to observe every instruction.
3. **Inspect registers** at halt to confirm the expected result.

| Part | Expected Register | Expected Value | Meaning |
|------|-------------------|----------------|---------|
| simple-arith | `$1` | `9` | `3*(5-2)` |
| jcc0 (i100-sum) | `$2` | `5050` | `1+2+...+100` |

**Simple Arithmetic**

**Verification (mmix simulator trace):**
```
$ mmix -t1 simple-arith-out0
1. 0000000000000100: e3010003 (SETL) $1=l[1] = #3
1. 0000000000000104: e3020005 (SETL) rL=3, $2=l[2] = #5
1. 0000000000000108: e3030002 (SETL) rL=4, $3=l[3] = #2
1. 000000000000010c: 24020203 (SUB)  $2=l[2] = 5 - 2 = 3
1. 0000000000000110: 18010102 (MUL)  $1=l[1] = 3 * 3 = 9
1. 0000000000000114: 00000000 (TRAP) Halt(0)
```

`$1 = 9` ✓ — matches `3*(5-2) = 9`.

**I100-SUM**

**Verification:** Run under the MMIX simulator and inspect `$2` after halt.
Expected value: **5050** (= 1+2+...+100).

```bash
../../mmix-20131017/mmix -t120 i100-sum-out |less
# ... (trace omitted for brevity) ...
       100. 0000000000000110: e3ca0064 (SETL) $202=l[202] = #64
       100. 0000000000000114: 30cb03ca (CMP) $203=l[203] = 100 cmp 100 = 0
       100. 0000000000000118: 44cb0007 (BP) 0>0?  No
       100. 000000000000011c: 20cc0203 (ADD) $204=l[204] = 4950 + 100 = 5050
       100. 0000000000000120: c102cc00 (ORI) $2=l[2] = 5050 = #13ba
       100. 0000000000000124: e3cd0001 (SETL) $205=l[205] = #1
       100. 0000000000000128: 20ce03cd (ADD) $206=l[206] = 100 + 1 = 101
       100. 000000000000012c: c103ce00 (ORI) $3=l[3] = 101 = #65
       100. 0000000000000130: f1fffff8 (JMPB) -> #110
       101. 0000000000000110: e3ca0064 (SETL) $202=l[202] = #64
# Confirm $2 = #13BA (= 5050 decimal) at halt
```

## TODO
- [High] Lexer: Support char and string literals
- [Medium] Support Unicode
- [Low] Document JCC0STK
