# JCC0 Architecture

JCC0 does not have the usual compiler modules `Lexer → Parser → AST → Semantic Analyzer → Code Generator`, but instead uses
```
Lexer → Token Buffer + Match Table → (Parse + Generate)
```

In a conventional compiler, the parser allocates heap nodes for every syntactic construct:

```c
ASTNode *n = ast_new(ND_WHILE);
n->cond = parse_expression();
n->body = parse_statement();
```

Then the code generator walks these nodes:

```c
switch (node->type) {
    case ND_WHILE:  gen_while(node);  break;
    case ND_ASSIGN: gen_assign(node); break;
    // ... dozens more cases
}
```

In JCC0 we simplify this. When the parser sees:

```c
while (i <= 100) { s = s + i; i = i + 1; }
```

It does not construct a `ND_WHILE` node with `cond` and `body` pointers.
Instead, it recursively descends into the condition, emits comparison code,
then recursively descends into the body, emits assignment code,
and finally emits the backward jump.

The stack frames of `gen_statement()` → `gen_expr()` → `gen_additive()` → ...
*are* the tree path. The tree exists in time, not in space.

---

## 3. Two-Pass Design with a Sparse Tree

If we eliminate the AST, how do we know where a `while` block ends,
or which `)` matches a given `(`?

We use a **two-pass strategy** over the flat token stream:

### Pass 1: Lex + Structural Metadata

All tokens are read into a flat array. Simultaneously, we build a **match table**—a parallel integer array where:

- `match[i]` = the index of the matching `)` or `}` for a `(` or `{` at position `i`.
- `match[i]` = the index of the matching `(` or `{` for a `)` or `}` at position `i`.

This is a **sparse tree representation**. It tells us the *span* of every parenthesized expression and every braced block without allocating a single tree node.

### Pass 2: Recursive Descent + Direct Emission

The generator walks the token stream with a cursor (`pos`).
Because the grammar is LL(1), the parser always knows what comes next:

- A `TK_WHILE` means: consume it, expect `(`, parse expression, expect `)`, parse statement, emit loop instructions.
- A `TK_LBRACE` means: consume it, parse statements until `TK_RBRACE`, consume it.

No node allocation. No pointer chasing. Just **parse, then emit**.
