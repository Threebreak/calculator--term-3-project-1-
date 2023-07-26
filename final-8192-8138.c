#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define INVALID 0
#define NUMBER 1
#define OPERATOR 2

typedef struct node
{
    float data;
    struct node *next;
} node;

typedef struct
{
    node *top;
} stack;

// Stack Functions:
stack *init();
node *newnode(float x);
float pop(stack *s);
void push(stack *s, float x);
float peek(stack *s);
int isEmpty(stack *s);

// Conversion Functions:
char *infixTopostfix(char *infix);
int checkType(char *str);
int priority(char operator);
void charcat(char *dest, char source);

// Evaluation Functions:
float evaluatePostfix(char *postfix);
float calculate(float x, float y, char *op);

int main()
{
    char infix[200];
    printf("Insert Expression: ");
    gets(infix);
    char *postfix = infixTopostfix(infix);
    printf(postfix);
    printf("\n");
    printf("%.2f", evaluatePostfix(postfix));
    free(postfix);
    return 0;
}

// Stack Functions:
stack *init()
{
    stack *s = malloc(sizeof(stack));
    if (s == NULL)
    {
        printf("Memory Error");
        exit(-1);
    }

    s->top = NULL;
    return s;
}

node *newnode(float x)
{
    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        printf("Memory Error");
        exit(-1);
    }

    n->data = x;
    n->next = NULL;
    return n;
}

float pop(stack *s)
{
    if (!isEmpty(s))
    {
        node *temp = s->top;
        float y = s->top->data;
        s->top = s->top->next;
        free(temp);
        return y;
    }
    else
    {
        printf("Error, can't pop stack is empty");
        exit(-1);
    }
}

void push(stack *s, float x)
{
    node *n = newnode(x);

    if (s->top == NULL)
        s->top = n;
    else
    {
        n->next = s->top;
        s->top = n;
    }
}

float peek(stack *s)
{
    if (!isEmpty(s))
    {
        return s->top->data;
    }
    else
    {
        printf("Error, can't peek stack is Empty");
        exit(-1);
    }
}

int isEmpty(stack *s)
{
    return (s->top == NULL);
}

// Converting Infix to Postfix

char *infixTopostfix(char *infix)
{
    char *postfix = malloc(strlen(infix) * sizeof(char));
    if (postfix == NULL)
    {
        printf("Memory error");
        exit(-1);
    }

    postfix[0] = '\0';
    char *token = strtok(infix, " ");
    char operator, lastoperator = '\0';
    int type, cur = 0, lastType = 0, bracketsopened = 0;
    stack *s = init();

    if (checkType(token) == OPERATOR && token[0] != '(')
    {
        printf("Error starting with a operator");
        exit(-1);
    }

    while (token != NULL)
    {
        type = checkType(token);
        switch (type)
        {
        case INVALID:
            printf("%s is invalid\n", token);
            exit(-1);
            break;

        case NUMBER:
            if (lastType == NUMBER)
            {
                printf("Error two consecutive numbers. ");
                exit(-1);
            }
            strcat(postfix, token);
            strcat(postfix, " ");
            break;

        case OPERATOR:
            if (lastType == OPERATOR && token[0] != '(' && lastoperator != ')')
            {
                printf("Error two consecutive operands. ");
                exit(-1);
            }
            operator= token[0];
            if (operator== '(')
            {
                bracketsopened++;
            }
            if (isEmpty(s))
            {
                push(s, operator);
            }

            else if (operator== ')')
            {
                bracketsopened--;
                while (peek(s) != '(')
                {
                    charcat(postfix, pop(s));
                    if (isEmpty(s))
                    {
                        printf("ERROR ')' without '('\n");
                        exit(-1);
                    }
                }
                pop(s);
            }

            else
            {
                while (!isEmpty(s) && peek(s) != '(' && priority(peek(s)) >= priority(operator))
                {
                    charcat(postfix, pop(s));
                }
                push(s, operator);
            }

            lastoperator = operator;

            break;
        }
        token = strtok(NULL, " ");
        lastType = type;
    }
    if (bracketsopened != 0)
    {
        printf("Error , '(' without ')' . ");
        exit(-1);
    }
    if (lastType == OPERATOR && operator!= ')')
    {
        printf("Error, Ending with an operator. ");
        exit(-1);
    }
    while (!isEmpty(s))
    {
        charcat(postfix, pop(s));
    }
    free(s);
    return postfix;
}

int checkType(char *str) // Returns: 1 if number, 2 if operator, 0 if invalid
{
    int type = 0, i = 0, decimal = 0;
    switch (str[0])
    {
    case '+':
    case '*':
    case '/':
    case '^':
    case '(':
    case ')':
        if (str[1] == '\0')
            return OPERATOR;
        else
            return INVALID;
        break;

    case '-':
        if (str[1] == '\0')
            return OPERATOR;
        else
            i = 1;
        break;
    }
    while (str[i] != '\0')
    {
        if (str[i] == '.' && !decimal)
        {
            decimal = 1;
        }
        else if (!isdigit(str[i]))
        {
            return INVALID;
        }
        i++;
    }
    return NUMBER;
}

int priority(char operator)
{
    switch (operator)
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '^':
        return 3;
    case '(':
        return 4;
    }
}

void charcat(char *dest, char source)
{
    char str[2];
    str[0] = source;
    str[1] = '\0';
    strcat(dest, str);
    strcat(dest, " ");
}

// Evaluating postfix Functions:

float calculate(float x, float y, char *op)

{
    if (*op == '+')
        return (x + y);
    else if (*op == '-')
        return (x - y);
    else if (*op == '*')
        return (x * y);
    else if (*op == '/')
        return (x / y);
    else if (*op == '^')
        return (pow(x, y));
}

float evaluatePostfix(char *postfix)
{
    stack *s = init();
    char *token = strtok(postfix, " ");
    int type, value;
    float x, y, result, calc;
    while (token != NULL)
    {
        type = checkType(token);
        switch (type)
        {
        case NUMBER:
            push(s, atof(token));
            break;
        case OPERATOR:
            y = pop(s);
            x = pop(s);
            if (*token == '/' && y == 0)
            {
                printf("No division by 0");
                exit(-1);
            }

            calc = calculate(x, y, token);
            push(s, calc);

            break;
        }
        token = strtok(NULL, " ");
    }
    result = pop(s);
    free(s);
    return result;
}
