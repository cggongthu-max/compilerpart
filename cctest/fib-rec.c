int ret;

int fib(int i)
{
  if (i == 1) return 1;
  if (i == 0) return 0;
  return fib(i-1) + fib(i-2);
}

ret = fib(10);
