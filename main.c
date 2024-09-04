#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "errors.h"

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32

static char buffer[2048];

/* Fake readline function */
char *readline(char *prompt)
{
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  size_t len = strlen(buffer);
  char *cpy = malloc(len + 1);
  strcpy(cpy, buffer);
  if (len > 0 && cpy[len - 1] == '\n')
  {
    cpy[len - 1] = '\0'; // Remove newline character if present
  }
  return cpy;
}

/* Fake add_history function */
void add_history(char *unused) {}

#else
#include <editline/readline.h>
#include <histedit.h>
#endif

mpc_parser_t *Number;
mpc_parser_t *Operator;
mpc_parser_t *Expr;
mpc_parser_t *maki;

int main(int argc, char *argv[])
{
  // Create parsers
  Number = mpc_new("number");
  Operator = mpc_new("operator");
  Expr = mpc_new("expr");
  maki = mpc_new("maki");

  // Define them with the following Language
  mpca_lang(MPCA_LANG_DEFAULT,
            "                                                     \
    number   : /-?[0-9]+(\\.[0-9]+)?/ ;                             \
    operator : '+' | '-' | '*' | '/' | '^' | \"max\" | \"min\";  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    maki    : /^/  <operator><expr>+ /$/ ;             \
  ",
            Number, Operator, Expr, maki);

  puts("makima version 0.0.0.2");
  puts("Press Ctrl + C to exit ");

  while (1)
  {
    char *input = readline("makima> ");
    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, maki, &r))
    {
      /* On Success Print the AST */
      mpc_ast_print(r.output);

    devilval result = eval(r.output);
    devilval_println(result);
      mpc_ast_delete(r.output);
    }
    else
    {
      /* Otherwise Print the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  /* Cleanup */
  mpc_cleanup(4, Number, Operator, Expr, maki);
  return 0;
}
