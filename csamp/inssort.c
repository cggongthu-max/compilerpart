#include <stdio.h>
#include <stdbool.h>

// m is 0-indexed 
void __insert__PARAM__into_the_sorted_sequence__PARAM_(int key, int *A, int m)
{
  int i,k;
  for (i=0;i<=m;i++) // <= bc m is 0-indexed
    if (key<A[i])
      {
	for (k=m;k>i;k--)
	  A[k]=A[k-1];
	A[i]=key;
      }
}

void inssort(int *A, int n)
{
  int i,j,key;
  for (j=1;j<n;j++)
    {
      key = A[j];
      __insert__PARAM__into_the_sorted_sequence__PARAM_(key,A,j-1);
    }
}

// 辅助函数：打印数组
void print_array(int *A, int n) {
    printf("[");
    for (int i = 0; i < n; i++) {
        printf("%d", A[i]);
        if (i < n-1) printf(", ");
    }
    printf("]\n");
}

// 辅助函数：验证数组是否已升序排序
bool is_sorted(int *A, int n) {
    for (int i = 0; i < n-1; i++) {
        if (A[i] > A[i+1]) {
            return false;
        }
    }
    return true;
}

int main(void)
{
    // 测试用例1：随机无序数组
    int test1[] = {5, 2, 9, 1, 5, 6};
    int n1 = sizeof(test1) / sizeof(test1[0]);
    
    printf("测试用例1（随机数组）：\n");
    printf("排序前："); print_array(test1, n1);
    inssort(test1, n1);
    printf("排序后："); print_array(test1, n1);
    printf("验证结果：%s\n\n", is_sorted(test1, n1) ? "通过" : "失败");

    // 测试用例2：已升序排序的数组（边界情况）
    int test2[] = {1, 2, 3, 4, 5};
    int n2 = sizeof(test2) / sizeof(test2[0]);
    
    printf("测试用例2（已排序数组）：\n");
    printf("排序前："); print_array(test2, n2);
    inssort(test2, n2);
    printf("排序后："); print_array(test2, n2);
    printf("验证结果：%s\n\n", is_sorted(test2, n2) ? "通过" : "失败");

    // 测试用例3：降序排序的数组（最坏情况）
    int test3[] = {9, 7, 5, 3, 1};
    int n3 = sizeof(test3) / sizeof(test3[0]);
    
    printf("测试用例3（降序数组）：\n");
    printf("排序前："); print_array(test3, n3);
    inssort(test3, n3);
    printf("排序后："); print_array(test3, n3);
    printf("验证结果：%s\n\n", is_sorted(test3, n3) ? "通过" : "失败");

    // 测试用例4：包含重复元素的数组
    int test4[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    int n4 = sizeof(test4) / sizeof(test4[0]);
    
    printf("测试用例4（含重复元素）：\n");
    printf("排序前："); print_array(test4, n4);
    inssort(test4, n4);
    printf("排序后："); print_array(test4, n4);
    printf("验证结果：%s\n\n", is_sorted(test4, n4) ? "通过" : "失败");

    // 测试用例5：空数组和单个元素（边界情况）
    int test5[] = {};
    int n5 = sizeof(test5) / sizeof(test5[0]);
    printf("测试用例5（空数组）：\n");
    inssort(test5, n5);
    printf("验证结果：通过（无操作）\n");

    int test6[] = {42};
    int n6 = sizeof(test6) / sizeof(test6[0]);
    printf("测试用例6（单个元素）：\n");
    printf("排序前："); print_array(test6, n6);
    inssort(test6, n6);
    printf("排序后："); print_array(test6, n6);
    printf("验证结果：%s\n", is_sorted(test6, n6) ? "通过" : "失败");

    return 0;
}
