#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define LINE_LENGTH 200
#define NUMBER_OF_TOKENS 50
#define TOKEN_VALUE_LENGTH 100
#define TOKEN_TYPE_LENGTH 20
#define VARIABLE_NAME_LENGTH 50

#define NUMBER_OF_KEYWORDS 2
#define NUMBER_OF_OPERATORS 5
#define NUMBER_OF_SYMBOLS 3
#define MAX_VARIABLES 100

typedef struct
{
    char type[TOKEN_TYPE_LENGTH];
    char value[TOKEN_VALUE_LENGTH];
} Token;

typedef struct
{
    char name[VARIABLE_NAME_LENGTH];
    int value;
    int isInitialized;
} Variable;

int isKeyword(char *str);
int isOperator(char *str);
int isSymbol(char *str);
int isIdentifier(char *str);
int isNumber(char *str);
void addVariable(char *name, int value, int isInitialized);
void setVariableValue(char *name, int value);
int lookupVariable(char *name);
void tokenize(FILE *file);
void printTokens();
void expect(char *type, char *value);
void parseProgram();
void parseStatementList();
void parseStatement();
void parseDeclaration();
void parseAssign();
void parsePrint();
int parseExpression();

char *keywords[] = {"int", "print"};
char *operators[] = {"+", "=", "-", "*", "/"};
char *symbols[] = {"(", ")", ";"};

Token tokens[NUMBER_OF_TOKENS];
int currentTokenIndex = 0;
int tokenCount = 0;

Variable variables[MAX_VARIABLES];
int variableCount = 0;

int main(int argc, char *argv[])
{
    FILE *file;
    char line[LINE_LENGTH];

    file = fopen(argv[1], "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    tokenize(file);
    printTokens();
    parseProgram();
    fclose(file);
    return 0;
}

int isKeyword(char *str)
{
    for (int i = 0; i < NUMBER_OF_KEYWORDS; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int isOperator(char *str)
{
    for (int i = 0; i < NUMBER_OF_OPERATORS; i++)
    {
        if (strcmp(str, operators[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int isSymbol(char *str)
{
    for (int i = 0; i < NUMBER_OF_SYMBOLS; i++)
    {
        if (strcmp(str, symbols[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int isIdentifier(char *str)
{
    if ((isalpha(str[0]) || str[0] == '_'))
    {
        for (int i = 1; str[i] != '\0'; i++)
        {
            if (!isalnum(str[i]) && str[i] != '_')
            {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int isNumber(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

void addVariable(char *name, int value, int isInitialized)
{
    if (variableCount < MAX_VARIABLES)
    {
        for (int i = 0; i < variableCount; i++)
        {
            if (strcmp(variables[i].name, name) == 0)
            {
                printf("Error: Variable '%s' already declared\n", name);
                exit(1);
            }
        }

        strncpy(variables[variableCount].name, name, VARIABLE_NAME_LENGTH - 1);
        variables[variableCount].name[VARIABLE_NAME_LENGTH - 1] = '\0';
        variables[variableCount].value = value;
        variables[variableCount].isInitialized = isInitialized;
        variableCount++;
    }
    else
    {
        printf("Error: Maximum variable limit reached\n");
        exit(1);
    }
}

void setVariableValue(char *name, int value)
{
    for (int i = 0; i < variableCount; i++)
    {
        if (strcmp(variables[i].name, name) == 0)
        {
            variables[i].value = value;
            variables[i].isInitialized = 1;
            return;
        }
    }
    printf("Semmentic Error: Variable '%s' not found\n", name);
    exit(1);
}

int lookupVariable(char *name)
{
    for (int i = 0; i < variableCount; i++)
    {
        if (strcmp(variables[i].name, name) == 0)
        {
            if (variables[i].isInitialized)
            {
                return variables[i].value;
            }
            else
            {
                printf("Error: Variable '%s' is not initialized\n", name);
                exit(1);
            }
        }
    }
    printf("Error: Variable '%s' not found\n", name);
    exit(1);
}

void tokenize(FILE *file)
{
    char ch;
    char buffer[TOKEN_VALUE_LENGTH];
    int bufferIndex = 0;

    while ((ch = fgetc(file)) != EOF && tokenCount < NUMBER_OF_TOKENS)
    {

        if (isspace(ch))
            continue;

        // check for keywords and identifiers
        if (isalpha(ch) || ch == '_')
        {
            bufferIndex = 0;
            buffer[bufferIndex++] = ch;

            while (isalpha((ch = fgetc(file))) || isdigit(ch) || ch == '_')
                buffer[bufferIndex++] = ch;

            buffer[bufferIndex] = '\0';
            ungetc(ch, file);

            if (isKeyword(buffer))
                strcpy(tokens[tokenCount].type, "KEYWORD");
            else
                strcpy(tokens[tokenCount].type, "IDENTIFIER");

            strcpy(tokens[tokenCount].value, buffer);
            tokenCount++;
        }

        // check for numbers
        else if (isdigit(ch))
        {
            bufferIndex = 0;
            buffer[bufferIndex++] = ch;

            while (isdigit((ch = fgetc(file))))
                buffer[bufferIndex++] = ch;

            buffer[bufferIndex] = '\0';
            ungetc(ch, file);

            strcpy(tokens[tokenCount].type, "NUMBER");
            strcpy(tokens[tokenCount].value, buffer);
            tokenCount++;
        }

        else
        {
            buffer[0] = ch;
            buffer[1] = '\0';

            if (isOperator(buffer))
                strcpy(tokens[tokenCount].type, "OPERATOR");
            else if (isSymbol(buffer))
                strcpy(tokens[tokenCount].type, "SYMBOL");
            else
            {
                printf("Error: Unrecognized token '%s'\n", buffer);
                exit(1);
            }

            strcpy(tokens[tokenCount].value, buffer);
            tokenCount++;
        }
    }
}

void printTokens()
{
    for (int i = 0; i < tokenCount; i++)
    {
        printf("Token %d: Type = %s, Value = %s\n", i, tokens[i].type, tokens[i].value);
    }
}

void expect(char *type, char *value)
{
    if (currentTokenIndex < tokenCount)
    {
        if (strcmp(tokens[currentTokenIndex].type, type) != 0)
        {
            printf("Syntax Error: Unexpected token %s\n", tokens[currentTokenIndex].value);
            exit(1);
        }
        if (strcmp(tokens[currentTokenIndex].value, value) != 0)
        {
            printf("Syntax Error: Expected '%s' but found '%s'\n",
                   value, tokens[currentTokenIndex].value);
            exit(1);
        }
        currentTokenIndex++;
    }
}

void parseProgram()
{
    parseStatementList();

    if (currentTokenIndex < tokenCount)
    {
        printf("Syntax Error: Unexpected token '%s'\n", tokens[currentTokenIndex].value);
        exit(1);
    }
}

void parseStatementList()
{
    while (currentTokenIndex < tokenCount)
    {
        parseStatement();
    }
}

void parseStatement()
{
    if (strcmp(tokens[currentTokenIndex].value, "int") == 0)
    {
        parseDeclaration();
    }
    else if (strcmp(tokens[currentTokenIndex].type, "IDENTIFIER") == 0)
    {
        parseAssign();
    }
    else if (strcmp(tokens[currentTokenIndex].value, "print") == 0)
    {
        parsePrint();
    }
    else
    {
        printf("Syntax Error: Invalid statement\n");
        exit(1);
    }
}

void parsePrint()
{
    expect("KEYWORD", "print");
    expect("SYMBOL", "(");

    char varName[VARIABLE_NAME_LENGTH];
    strncpy(varName, tokens[currentTokenIndex].value, VARIABLE_NAME_LENGTH - 1);

    expect("IDENTIFIER", tokens[currentTokenIndex].value);
    expect("SYMBOL", ")");
    expect("SYMBOL", ";");

    printf("%d\n", lookupVariable(varName));
}

void parseDeclaration()
{
    expect("KEYWORD", "int");

    char varName[VARIABLE_NAME_LENGTH];
    strncpy(varName, tokens[currentTokenIndex].value, VARIABLE_NAME_LENGTH - 1);
    expect("IDENTIFIER", tokens[currentTokenIndex].value);
    if (currentTokenIndex < tokenCount && strcmp(tokens[currentTokenIndex].value, "=") == 0)
    {
        expect("OPERATOR", "=");
        int value = parseExpression();
        addVariable(varName, value, 1);
    }
    else
    {
        addVariable(varName, 0, 0);
    }
    expect("SYMBOL", ";");
}

void parseAssign()
{

    char varName[VARIABLE_NAME_LENGTH];
    strncpy(varName, tokens[currentTokenIndex].value, VARIABLE_NAME_LENGTH - 1);

    expect("IDENTIFIER", tokens[currentTokenIndex].value);
    expect("OPERATOR", "=");
    int value = parseExpression();
    setVariableValue(varName, value);
    expect("SYMBOL", ";");
}

int parseExpression()
{
    int leftValue;

    if (strcmp(tokens[currentTokenIndex].type, "NUMBER") == 0)
    {
        leftValue = atoi(tokens[currentTokenIndex].value);
        expect("NUMBER", tokens[currentTokenIndex].value);
    }
    else if (strcmp(tokens[currentTokenIndex].type, "IDENTIFIER") == 0)
    {
        leftValue = lookupVariable(tokens[currentTokenIndex].value);
        expect("IDENTIFIER", tokens[currentTokenIndex].value);
    }
    else
    {
        printf("Syntax Error: Expected number or identifier\n");
        exit(1);
    }

    if (currentTokenIndex < tokenCount && strcmp(tokens[currentTokenIndex].value, "+") == 0)
    {
        expect("OPERATOR", "+");
        int rightValue = parseExpression();
        return leftValue + rightValue;
    }

    return leftValue;
}
