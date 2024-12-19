#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_LENGTH 1024
#define MAX_NUM_STACK 1024
#define MAX_OP_STACK 256

typedef struct {
    double *data;
    int top;
} NumStack;

typedef struct {
    char *data;
    int top;
} OpStack;

void initNumStack(NumStack *s) {
    s->data = (double *)malloc(MAX_NUM_STACK * sizeof(double));
    s->top = -1;
}

void initOpStack(OpStack *s) {
    s->data = (char *)malloc(MAX_OP_STACK * sizeof(char));
    s->top = -1;
}

void pushNum(NumStack *s, double num) {
    if (s->top < MAX_NUM_STACK - 1) {
        s->data[++(s->top)] = num;
    } else {
        fprintf(stderr, "Error: Number stack overflow\n");
        exit(1);
    }
}

double popNum(NumStack *s) {
    if (s->top > -1) {
        return s->data[(s->top)--];
    } else {
        fprintf(stderr, "Error: Number stack underflow\n");
        exit(1);
    }
}

void pushOp(OpStack *s, char op) {
    if (s->top < MAX_OP_STACK - 1) {
        s->data[++(s->top)] = op;
    } else {
        fprintf(stderr, "Error: Operator stack overflow\n");
        exit(1);
    }
}

char popOp(OpStack *s) {
    if (s->top > -1) {
        return s->data[(s->top)--];
    } else {
        fprintf(stderr, "Error: Operator stack underflow\n");
        exit(1);
    }
}

double applyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) {
                fprintf(stderr, "Error: Division by zero\n");
                exit(1);
            }
            return a / b;
        case '^': return pow(a, b);
        case 's': return sqrt(b);
    }
    return 0;
}

int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == 's';
}

int precedence(char op) {
    if (op == '+' || op == '-') {
        return 1;
    } else if (op == '*' || op == '/' || op == '^' || op == 's') {
        return 2;
    }
    return 0;
}

void evaluate(char expr[], NumStack *nums, OpStack *ops) {
    int i = 0;
    double num;
    char op;
    int len = strlen(expr);

    while (i < len) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        if (isdigit(expr[i]) || (expr[i] == '.' && i + 1 < len && isdigit(expr[i + 1]))) {
            double value = 0;
            int decimal = 0;
            if (expr[i] == '.') {
                decimal = 1;
                i++;
            }
            while (i < len && (isdigit(expr[i]) || expr[i] == '.')) {
                if (expr[i] == '.') {
                    decimal = 1;
                    i++;
                }
                if (!decimal) {
                    value = value * 10 + (expr[i] - '0');
                } else {
                    value = value + (expr[i] - '0') / (10.0);
                    decimal = 0;
                }
                i++;
            }
            pushNum(nums, value);
            continue;
        }

        if (expr[i] == '(') {
            pushOp(ops, expr[i]);
        } else if (expr[i] == ')') {
            while (ops->top != -1 && ops->data[ops->top] != '(') {
                double b = popNum(nums);
                double a = popNum(nums);
                op = popOp(ops);
                pushNum(nums, applyOp(a, b, op));
            }
            if (ops->top == -1 || ops->data[ops->top] != '(') {
                fprintf(stderr, "Error: Mismatched parentheses\n");
                exit(1);
            }
            popOp(ops); // Pop the '(' from the stack
        } else if (is_operator(expr[i])) {
            while (ops->top != -1 && precedence(ops->data[ops->top]) >= precedence(expr[i])) {
                double b = popNum(nums);
                double a = popNum(nums);
                op = popOp(ops);
                pushNum(nums, applyOp(a, b, op));
            }
            pushOp(ops, expr[i]);
        }

        i++;
    }

    while (ops->top != -1) {
        double b = popNum(nums);
        double a = popNum(nums);
        op = popOp(ops);
        pushNum(nums, applyOp(a, b, op));
    }
}

void printHelp() {
    printf("Available operations:\n");
    printf("  +   Add two numbers\n");
    printf("  -   Subtract two numbers\n");
    printf("  *   Multiply two numbers\n");
    printf("  /   Divide two numbers (note: division by zero is not allowed)\n");
    printf("  ^   Raise a number to the power of another\n");
    printf("  s   Square root of a number\n");
    printf("\nUsage:\n");
    printf("  Enter an expression using the above operations.\n");
    printf("  Example: 3 + 4 * 2\n");
    printf("  To exit, type 'END'\n");
}

int main() {
    char expr[MAX_EXPR_LENGTH];
    NumStack nums;
    OpStack ops;

    initNumStack(&nums);
    initOpStack(&ops);

    printHelp(); // Print help information

    while (1) { // 使用无限循环来不断接收用户输入
        printf("\nEnter an expression (or 'END' to exit): ");
        if (fgets(expr, MAX_EXPR_LENGTH, stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            return 1;
        }

        // Remove newline character if present
        size_t len = strlen(expr);
        if (len > 0 && expr[len - 1] == '\n') {
            expr[len - 1] = '\0';
        }

        // Check if the user wants to exit
        if (strcmp(expr, "END") == 0) {
            break; // 如果输入"END"，则退出循环和程序
        }

        evaluate(expr, &nums, &ops);

        printf("Result: %lf\n", popNum(&nums));

        free(nums.data);
        free(ops.data);
        // 重新初始化栈，以便下一次计算
        initNumStack(&nums);
        initOpStack(&ops);
    }

    return 0;
}