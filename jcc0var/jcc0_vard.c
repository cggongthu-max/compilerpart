/* test_query_variable.c
 * Unit tests for query_variable() in a scope-table-based C compiler.
 *
 * Structures (assumed defined in your compiler header):
 *
 *   T: flat array of triples {brace_begin_idx, interval_len, nested_brace_cnt}
 *   Q: flat array of pairs  {decl_idx, ident_name}
 *
 * query_variable(char *ident, int idx) -> declaration index, or -1 if not visible.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

/* ---------- Minimal declarations to link against your implementation ---------- */

/* You would include your actual header; here we declare the extern interface. */
extern int  query_variable(const char *ident, int idx);
extern void set_scope_table(const int *T, int t_count);   /* t_count = number of triples */
const int *FT;
int FT_COUNT;
void set_scope_table(const int *T, int t_count)
{
  FT=T;
  FT_COUNT=t_count;
}
extern void set_decl_table(const char **Q_idents, const int *Q_indices, int q_count);
const char **FQ_name;
const int *FQ_idx;
int FQ_COUNT;
void set_decl_table(const char **Q_idents, const int *Q_indices, int q_count)
{
  FQ_name=Q_idents;
  FQ_idx=Q_indices;
  FQ_COUNT=q_count;
}

/* If your implementation uses globals directly, adapt the setup helpers below. */

/* ---------- Helpers ---------- */

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        tests_run++; \
        printf("  [TEST] %-50s ", #name); \
    } while(0)

#define EXPECT_EQ(got, want) \
    do { \
        if ((got) == (want)) { \
            tests_passed++; \
            printf("PASS\n"); \
        } else { \
            printf("FAIL  (got %d, want %d)\n", (got), (want)); \
        } \
    } while(0)

/* ---------- Fixture setup ---------- */

/*
 * We model a token stream like this (indices are token positions):
 *
 *   0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
 *  {  int a ;  int b ;  {  int a ;  b = a ;  }  a = b ;  }
 *
 *   20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
 *  {  int c ;  {  int d ;  c = d ;  }  c = c ;  }  <EOF>
 *
 * Scope table T (triples: begin, len, nested_cnt):
 *   Scope A (outermost):  begin=0,  len=20, nested=1
 *   Scope B (inner):      begin=7,  len=8,  nested=0   (nested inside A)
 *   Scope C (second top): begin=20, len=15, nested=1
 *   Scope D (inner of C): begin=24, len=6,  nested=0
 *
 * Declaration table Q (idx, name):
 *   (2,  "a")   -- declared in Scope A
 *   (5,  "b")   -- declared in Scope A
 *   (9,  "a")   -- declared in Scope B (shadows outer 'a')
 *   (22, "c")   -- declared in Scope C
 *   (26, "d")   -- declared in Scope D
 */

static const int T[] = {
    /* Scope A */  0, 20, 1,
    /* Scope B */  7,  8, 0,
    /* Scope C */ 20, 15, 1,
    /* Scope D */ 24,  6, 0,
};
static const int T_COUNT = 4;  /* 4 scopes */

static const int    Q_idx[]   = { 2, 5, 9, 22, 26 };
static const char  *Q_name[]  = { "a", "b", "a", "c", "d" };
static const int    Q_COUNT   = 5;

static void setup_fixture(void) {
  set_scope_table(T, T_COUNT);
  set_decl_table(Q_name, Q_idx, Q_COUNT);
}

/*
 * Implementation: min_enclosing_scope(int idx)
 * Given the declaration index, finds the smallest nested scope that contains it.
 * Returns the start index (brace_begin_idx) of the enclosing scope.
 */
// this could be optimized
int min_enclosing_scope(int idx) {
    int enclosing_start = -1; // Default to not found

    for (int i = 0; i < T_COUNT; i++) {
        int start = T[i * 3];
        int len = T[i * 3 + 1];
        
        // Check if idx falls within the current scope's interval [start, start + len)
        if (idx >= start && idx < start + len) {
	  enclosing_start = i; //start;
            // We continue searching because subsequent scopes might be nested inside this one.
            // Since T is ordered outer->inner, the last match will be the most nested.
        }
    }

    return enclosing_start;
}

// This works as long as no variable declaration overshadows
int query_variable(const char *ident, int idx)
{
  int r,s;  int ret=-1;
  for (r=0;r<FT_COUNT*3;r+=3)
    for (s=0;s<FQ_COUNT;s++)
      {
	int i = min_enclosing_scope(FQ_idx[s]);
	if (FT[r]<=idx && idx<FT[r]+FT[r+1]
	    // find min enclosing scope of var decl & ensure not out of scope
	    && (i == -1 || idx<FT[3*i]+FT[3*i+1])
	    // declare var before use
	    && FQ_idx[s] < idx
	    && !strcmp(FQ_name[s],ident))
	  ret = FQ_idx[s];
      }
  return ret;
}

/* ---------- Test cases ---------- */

static void test_same_scope_lookup(void) {
    printf("\n== Same-scope visibility ==\n");

    setup_fixture();

    /* 'b' used at idx=14 (inside Scope A, after decl at 5) */
    TEST(b_used_in_own_scope);
    EXPECT_EQ(query_variable("b", 14), 5);

    /* 'a' (outer) used at idx=17, which is after Scope B ended (7+8=15),
       so we are back in Scope A. Should resolve to outer 'a' at idx=2. */
    TEST(a_after_inner_scope_ends);
    EXPECT_EQ(query_variable("a", 17), 2);
}

static void test_inner_scope_shadow(void) {
    printf("\n== Shadowing in inner scope ==\n");

    setup_fixture();

    /* 'a' used at idx=12 inside Scope B. Inner 'a' declared at 9 shadows outer. */
    TEST(a_shadowed_in_inner_scope);
    EXPECT_EQ(query_variable("a", 12), 9);

    /* 'b' used at idx=11 inside Scope B. 'b' only declared in outer Scope A. */
    TEST(b_visible_from_outer_in_inner);
    EXPECT_EQ(query_variable("b", 11), 5);
}

static void test_inner_not_visible_outside(void) {
    printf("\n== Inner-scope variable NOT visible outside ==\n");

    setup_fixture();

    /* 'a' at idx=17 is outside Scope B (which ends at 7+8=15).
       The inner 'a' (decl 9) should NOT be visible. Only outer 'a' (decl 2). */
    TEST(inner_a_not_visible_after_scope_B);
    EXPECT_EQ(query_variable("a", 17), 2);

    /* 'd' declared in Scope D (begin=24, len=6, ends at 30).
       Used at idx=32 which is in Scope C but after Scope D ended. Not visible. */
    TEST(d_not_visible_after_scope_D);
    EXPECT_EQ(query_variable("d", 32), -1);
}

static void test_nested_visibility(void) {
    printf("\n== Nested scope visibility ==\n");

    setup_fixture();

    /* 'c' declared at 22 in Scope C. Used at idx=28 inside Scope D. Should see it. */
    TEST(c_visible_in_nested_scope_D);
    EXPECT_EQ(query_variable("c", 28), 22);

    /* 'd' declared at 26 in Scope D. Used at idx=29, still inside D. */
    TEST(d_visible_in_own_scope);
    EXPECT_EQ(query_variable("d", 29), 26);
}

static void test_not_declared(void) {
    printf("\n== Undeclared identifier ==\n");

    setup_fixture();

    TEST(undeclared_x);
    EXPECT_EQ(query_variable("x", 10), -1);

    TEST(undeclared_empty_string);
    EXPECT_EQ(query_variable("", 10), -1);
}

static void test_use_before_declaration(void) {
    printf("\n== Use before declaration (same scope) ==\n");

    setup_fixture();

    /* 'b' declared at idx=5. Query at idx=3 (before decl) should fail. */
    TEST(b_used_before_decl);
    EXPECT_EQ(query_variable("b", 3), -1);

    /* 'a' outer declared at 2. Query at idx=1 should fail. */
    TEST(a_used_before_decl);
    EXPECT_EQ(query_variable("a", 1), -1);
}

static void test_deeply_nested(void) {
    printf("\n== Deeply nested scopes ==\n");

    /*
     * Construct a 3-level nesting:
     *   Scope X: begin=0, len=30, nested=2
     *   Scope Y: begin=5, len=20, nested=1   (inside X)
     *   Scope Z: begin=10, len=10, nested=0  (inside Y)
     *
     * Declarations:
     *   (2,  "v") in X
     *   (7,  "v") in Y  (shadows X's v)
     *   (12, "v") in Z  (shadows Y's v)
     */
    static const int T2[] = {
        0, 30, 2,
        5, 20, 1,
        10, 10, 0,
    };
    static const int    Q2_idx[]  = { 2, 7, 12 };
    static const char  *Q2_name[] = { "v", "v", "v" };

    set_scope_table(T2, 3);
    set_decl_table(Q2_name, Q2_idx, 3);

    /* Inside Z: should see Z's 'v' */
    TEST(v_in_innermost_Z);
    EXPECT_EQ(query_variable("v", 15), 12);

    /* Inside Y but outside Z (idx=22, Z ends at 10+10=20): should see Y's 'v' */
    TEST(v_in_Y_after_Z_ends);
    EXPECT_EQ(query_variable("v", 22), 7);

    /* Inside X but outside Y (idx=27, Y ends at 5+20=25): should see X's 'v' */
    TEST(v_in_X_after_Y_ends);
    EXPECT_EQ(query_variable("v", 27), 2);
}

static void test_sibling_scopes(void) {
    printf("\n== Sibling scopes (no cross-visibility) ==\n");

    /*
     *   Scope P: begin=0, len=10, nested=0
     *   Scope R: begin=12, len=10, nested=0
     * Declarations:
     *   (3, "s") in P
     *   (15, "s") in R
     */
    static const int T3[] = {
        0, 10, 0,
        12, 10, 0,
    };
    static const int    Q3_idx[]  = { 3, 15 };
    static const char  *Q3_name[] = { "s", "s" };

    set_scope_table(T3, 2);
    set_decl_table(Q3_name, Q3_idx, 2);

    /* 's' in Scope R should see R's declaration */
    TEST(s_in_sibling_R);
    EXPECT_EQ(query_variable("s", 18), 15);

    /* 's' in Scope P should see P's declaration */
    TEST(s_in_sibling_P);
    EXPECT_EQ(query_variable("s", 7), 3);

    /* 's' at idx=11 (between scopes, in neither) => not visible */
    TEST(s_between_siblings);
    EXPECT_EQ(query_variable("s", 11), -1);
}

/* ---------- Main ---------- */

int main(void) {
    printf("=== query_variable test suite ===\n");
    printf("(Sakura's scope-table compiler)\n");

    test_same_scope_lookup();
    test_inner_scope_shadow();
    test_inner_not_visible_outside();
    test_nested_visibility();
    test_not_declared();
    test_use_before_declaration();
    test_deeply_nested();
    test_sibling_scopes();

    printf("\n=== Results: %d / %d passed ===\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
