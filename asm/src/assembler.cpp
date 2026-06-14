#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <list>

#include "assembler.h"

struct TokenState
{
    const char *content;
    int length;
    int pointer;
    char *token;
};

static char is_white_space(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static char is_alphabetic(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static char is_numeric(char c)
{
    return c >= '0' && c <= '9';
}

static void init_token(struct TokenState *token_state, const char *content, int length)
{
    memset(token_state, 0, sizeof(struct TokenState));
    token_state->content = content;
    token_state->length = length;

    // avança o ponteiro para o primeiro token
    while (token_state->pointer < token_state->length)
    {
        if (!is_white_space(token_state->content[token_state->pointer]))
            break;
        token_state->pointer++;
    }
}

static void release_token(struct TokenState *token_state)
{
    if (token_state->token)
    {
        free(token_state->token);
        token_state->token = NULL;
    }
}

static const char *next_token(struct TokenState *token_state)
{
    // libera a memória usada para manter o token anterior (se houver)
    if (token_state->token)
    {
        free(token_state->token);
        token_state->token = NULL;
    }

    // retornar imediatamente caso todo o conteúdo já senha sido analisado
    if (token_state->pointer >= token_state->length)
        return NULL;

    if (is_white_space(token_state->content[token_state->pointer]))
    {
        // retorna uma string vazia, mas antes avança o ponteiro para o próximo token
        while (token_state->pointer < token_state->length)
        {
            if (!is_white_space(token_state->content[token_state->pointer]))
                break;
            token_state->pointer++;
        }

        return "";
    }

    // ponto de início do novo token
    int start = token_state->pointer;

    while (true)
    {
        char c = token_state->content[token_state->pointer];

        // o token encerra quando entra um espaço em branco ou um caractere
        // não alfa e não numérico
        // isso significa que números com ponto decimal serão tratados como três
        // token distintos, cabendo ao analisador sintático tratá-los adequadamente
        if (is_white_space(c))
        {
            break;
        }

        else if (!is_alphabetic(c) && !is_numeric(c))
        {
            if (start == token_state->pointer)
                token_state->pointer++;
            break;
        }

        token_state->pointer++;
        if (token_state->pointer >= token_state->length)
            break;
    }

    int token_length = token_state->pointer - start;
    token_state->token = (char *)malloc(token_length + 1);
    strncpy(token_state->token, &token_state->content[start], token_length);
    token_state->token[token_length] = '\0';

    return token_state->token;
}

#define SYNTAX_IDLE                 0
#define SYNTAX_DIRECTIVE            1
#define SYNTAX_LABEL_OR_STATEMENT   2
#define SYNTAX_LABEL                3
#define SYNTAX_STATEMENT            4
#define SYNTAX_DIRECTIVE_ARGS       5

#define NODE_UNKNOWN        0
#define NODE_DIRECTIVE      1
#define NODE_LABEL          2
#define NODE_STATEMENT      3
#define NODE_ARGUMENT       4


struct SyntaticNode
{
    int type;
    std::list<const char *> *symbols;
    std::list<struct SyntaticNode *> *args;
};

static struct SyntaticNode *new_syntatic_node(int type, const char *symbol)
{
    struct SyntaticNode *node = (struct SyntaticNode *)malloc(sizeof(struct SyntaticNode));
    memset(node, 0, sizeof(struct SyntaticNode));
    node->type = type;
    node->symbols = new std::list<const char *>();
    node->args = new std::list<struct SyntaticNode *>();

    if (symbol)
        node->symbols->push_back(symbol);

    return node;
}

static std::list<struct SyntaticNode *> *build_syntatic_tree(FILE *fp, FILE *err)
{
    std::list<struct SyntaticNode *> *syntatic_tree = new std::list<struct SyntaticNode *>();

    int line_number = 0;

    while (!feof(fp))
    {
        char line[1024];
        fgets(line, sizeof(line), fp);
        line_number++;

        struct TokenState token_state;
        init_token(&token_state, line, strlen(line));

        int state = SYNTAX_IDLE;

        while (true)
        {
            int column_number = token_state.pointer;

            const char *token = next_token(&token_state);
            if (token == NULL)
                release_token(&token_state);

printf("%d:%d\n", line_number, column_number);

            switch (state)
            {
                case SYNTAX_IDLE:
                {
                    // comentários encerram a análise da linha
                    if (token == NULL || strcmp(token, ";") == 0)
                    {
                        return syntatic_tree;
                    }

                    // ponto indica que o que segue será uma diretiva
                    else if (strcmp(token, ".") == 0)
                    {
                        syntatic_tree->push_back(new_syntatic_node(NODE_DIRECTIVE, token));
                        state = SYNTAX_DIRECTIVE;
                    }

                    // labels podem começar com sublinhado ou letras
                    else if (strcmp(token, "_") == 0)
                    {
                        syntatic_tree->push_back(new_syntatic_node(NODE_LABEL, token));
                        state = SYNTAX_LABEL;
                    }

                    else if (strlen(token) > 0)
                    {
                        if (is_alphabetic(token[0]))
                        {
                            syntatic_tree->push_back(new_syntatic_node(NODE_UNKNOWN, token));
                            state = SYNTAX_LABEL_OR_STATEMENT;
                        }

                        else
                        {
                            fprintf(err, "unexpected char at %d:%d\n", line_number, column_number);
                            // TODO
                            // goto error
                            return NULL;
                        }
                    }

                    else if (strlen(token) != 0)
                    {
                        fprintf(err, "unexpected char at %d:%d\n", line_number, column_number);
                        // TODO
                        // goto error
                        return NULL;
                    }

                    break;
                }

                case SYNTAX_DIRECTIVE:
                {
                    if (token == NULL || strlen(token) == 0 || !is_alphabetic(token[0]))
                    {
                        fprintf(err, "invalid directive name at %d:%d\n", line_number, column_number);
                        // TODO
                        // goto error
                        return NULL;
                    }

                    syntatic_tree->back()->symbols->push_back(token);
                    state = SYNTAX_DIRECTIVE_ARGS;
                    break;
                }

                case SYNTAX_DIRECTIVE_ARGS:
                {
                    if (token == NULL)
                        return syntatic_tree;

                    struct SyntaticNode *directive = syntatic_tree->back();

                    if (strlen(token) == 0)
                    {
                        directive->args->push_back(new_syntatic_node(NODE_ARGUMENT, NULL));
                    }

                    else
                    {
                        if (directive->args->empty())
                            directive->args->push_back(new_syntatic_node(NODE_ARGUMENT, token));
                        else
                            directive->args->back()->symbols->push_back(token);
                    }

                    break;
                }
            }
        }

break;
    }

    return syntatic_tree;
}

char assembly_to_object(const char *filename, FILE *err)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("can't open %s: %d\n", filename, errno);
        return false;
    }

    build_syntatic_tree(fp, err);


    return true;
}

