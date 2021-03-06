/*------------------------------------------------------------------------------
 *    Copyright 2016 Chris Rink
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "lexer_test.h"
#include "../src/lexer.h"

typedef struct LexResultTuple {
    const char *val;        /** Value of the input token */
    ms_TokenType type;      /** Type of token value should be lexed into */
} LexResultTuple;

/*
 * TEST DATA AND PARAMETERS
 */

static char* int_num_vals[] = {
    "0", "1", "15", "382932", "791933922",
    NULL
};

static MunitParameterEnum int_num_params[] = {
    { "num", int_num_vals },
    { NULL, NULL }
};

static char* float_num_vals[] = {
    "32223.53", "3.14", "2.7182818284", "1.6180339", "1.414", "2.",
    "2402.", "1332.", "10e4", "1e10", "2.3e8", "8.e4", "7e33", ".2",
    ".0314", "0.02718",
    NULL
};

static MunitParameterEnum float_num_params[] = {
    { "num", float_num_vals },
    { NULL, NULL }
};

static char* hex_num_vals[] = {
    "0x1", "0X0", "0x32f2", "0xf", "0xbeef", "0XDEAD",
    NULL
};

static MunitParameterEnum hex_num_params[] = {
    { "num", hex_num_vals },
    { NULL, NULL }
};

static char* bad_num_vals[] = {
    "0x",
    NULL
};

static MunitParameterEnum bad_num_params[] = {
    { "num", bad_num_vals },
    { NULL, NULL }
};

static char *non_keyword_vals[] = {
    "IF", "ret", "True", "False", "nil", "function", "delete",
    "i", "k", "next", "CONSTANT",
    NULL
};

static MunitParameterEnum non_keyword_params[] = {
    { "kw", non_keyword_vals },
    { NULL, NULL }
};

static char *reserved_keyword_vals[] = {
    "while", "switch", "goto", "error", "class", "private", "public",
    "protected", "yield", "from", "try", "except", "finally", "do", "and",
    "or", "mut", "const", "async", "await", "repeat", "until", "package",
    "num", "str", "bool", "datetime", "obj", "as", "with", "using", "spawn",
    "val", "merge",
    NULL
};

static MunitParameterEnum reserved_keyword_params[] = {
    { "kw", reserved_keyword_vals },
    { NULL, NULL }
};

static char* global_vals[] = {
    "@global", "@var", "@_legal_name", "@________", "@g", "@\xF0\x9F\x86\x92",
    NULL
};

static MunitParameterEnum global_params[] = {
    { "global", global_vals },
    { NULL, NULL }
};

static char* builtin_vals[] = {
    "$begin", "$commit", "$rollback", "$order", "$data",
    "$char", "$ord", "$len", "$type", "$open", "$close",
    NULL
};

static MunitParameterEnum builtin_params[] = {
    { "builtin", builtin_vals },
    { NULL, NULL }
};

static char* invalid_identifier_vals[] = {
    "$", "@", "@-some-name", "$%%%%", "`", "#",
    NULL
};

static MunitParameterEnum invalid_identifier_params[] = {
    { "ident", invalid_identifier_vals },
    { NULL, NULL }
};

static char* string_vals[] = {
    "\"\"", "''", "\"string\"", "\"string with 'single-quoted' sub\"",
    "\"string with \\\"escaped sub\\\" -- very meta\"",
    "\"string containing keywords: if, else, func\"",
    "\"string containing builtin: $begin, $commit\"",
    "\"string containing global: @glo, @people\"",
    NULL
};

static MunitParameterEnum string_params[] = {
    { "str", string_vals },
    { NULL, NULL }
};

static char* invalid_string_vals[] = {
    "\"'", "'\"", "\"\n\"", "\"\r\n\"", "'\n'", "'\r\n'",
    NULL
};

static MunitParameterEnum invalid_string_params[] = {
    { "str", invalid_string_vals },
    { NULL, NULL }
};

/*
 * TEST DEFINITIONS
 */

static MunitResult lex_TestLexIntNumerics(const MunitParameter params[], void *user_data);
static MunitResult lex_TestLexFloatNumerics(const MunitParameter params[], void *user_data);
static MunitResult lex_TestLexHexNumerics(const MunitParameter params[], void *user_data);
static MunitResult lex_TestLexBadNumerics(const MunitParameter params[], void *user_data);
static MunitResult lex_TestLexKeywords(const MunitParameter params[], void *user_data);
static MunitResult lex_TestLexNonKeywords(const MunitParameter *params, void *user_data);
static MunitResult lex_TestLexReservedKeywords(const MunitParameter *params, void *user_data);
static MunitResult lex_TestLexGlobals(const MunitParameter *params, void *user_data);
static MunitResult lex_TestLexBuiltins(const MunitParameter *params, void *user_data);
static MunitResult lex_TestLexInvalidIdentifiers(const MunitParameter *params, void *user_data);
static MunitResult lex_TestLexOperators(const MunitParameter *params, void *user_data);
static MunitResult lex_TestLexPunctuation(const MunitParameter *params, void *user_data);
static MunitResult lex_TestLexStrings(const MunitParameter params[], void *user_data);
static MunitResult lex_TestLexInvalidStrings(const MunitParameter params[], void *user_data);

MunitTest lexer_tests[] = {
    {
        "/IntNumeric",
        lex_TestLexIntNumerics,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        int_num_params
    },
    {
        "/FloatNumeric",
        lex_TestLexFloatNumerics,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        float_num_params
    },
    {
        "/HexNumeric",
        lex_TestLexHexNumerics,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        hex_num_params
    },
    {
        "/BadNumeric",
        lex_TestLexBadNumerics,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        bad_num_params
    },
    {
        "/Keywords",
        lex_TestLexKeywords,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },
    {
        "/NonKeywords",
        lex_TestLexNonKeywords,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        non_keyword_params
    },
    {
        "/ReservedKeywords",
        lex_TestLexReservedKeywords,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        reserved_keyword_params
    },
    {
        "/Globals",
        lex_TestLexGlobals,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        global_params
    },
    {
        "/Builtins",
        lex_TestLexBuiltins,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        builtin_params
    },
    {
        "/InvalidIdentifiers",
        lex_TestLexInvalidIdentifiers,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        invalid_identifier_params
    },
    {
        "/Operators",
        lex_TestLexOperators,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },
    {
        "/Punctuation",
        lex_TestLexPunctuation,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },
    {
        "/Strings",
        lex_TestLexStrings,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        string_params
    },
    {
        "/InvalidStrings",
        lex_TestLexInvalidStrings,
        NULL,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        invalid_string_params
    },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

/*
 * FORWARD DECLARATIONS
 */

static MunitResult LexCompareStrings(const char *param, const char *val);
static MunitResult LexExpect(const char *param, ms_TokenType type);
static MunitResult TestLexResultTuple(LexResultTuple *tokens, size_t len);

/*
 * UNIT TEST FUNCTIONS
 */

static MunitResult lex_TestLexIntNumerics(const MunitParameter params[], void *user_data) {
    const char *num = munit_parameters_get(params, "num");
    return LexExpect(num, INT_NUMBER);
}

static MunitResult lex_TestLexFloatNumerics(const MunitParameter params[], void *user_data) {
    const char *num = munit_parameters_get(params, "num");
    return LexExpect(num, FLOAT_NUMBER);
}

static MunitResult lex_TestLexHexNumerics(const MunitParameter params[], void *user_data) {
    const char *num = munit_parameters_get(params, "num");
    return LexExpect(num, HEX_NUMBER);
}

static MunitResult lex_TestLexBadNumerics(const MunitParameter params[], void *user_data) {
    const char *num = munit_parameters_get(params, "num");
    return LexExpect(num, ERROR);
}

static MunitResult lex_TestLexKeywords(const MunitParameter params[], void *user_data) {
    static LexResultTuple tokens[] = {
        { "if", KW_IF },
        { "else", KW_ELSE },
        { "return", KW_RETURN },
        { "true", KW_TRUE },
        { "false", KW_FALSE },
        { "null", KW_NULL },
        { "func", KW_FUNC },
        { "del", KW_DEL },
        { "continue", KW_CONTINUE },
        { "break", KW_BREAK },
        { "import", KW_IMPORT },
        { "var", KW_VAR },
        { "in", KW_IN },
        { "is", KW_IS },
        { "for", KW_FOR },
        { "select", KW_SELECT },
    };

    size_t len = sizeof(tokens) / sizeof(tokens[0]);
    return TestLexResultTuple(tokens, len);
}

static MunitResult lex_TestLexNonKeywords(const MunitParameter params[], void *user_data) {
    const char *kw = munit_parameters_get(params, "kw");
    return LexExpect(kw, IDENTIFIER);
}

static MunitResult lex_TestLexReservedKeywords(const MunitParameter params[], void *user_data) {
    const char *kw = munit_parameters_get(params, "kw");
    return LexExpect(kw, RESERVED_KW);
}

static MunitResult lex_TestLexGlobals(const MunitParameter params[], void *user_data) {
    const char *global = munit_parameters_get(params, "global");
    return LexExpect(global, GLOBAL);
}

static MunitResult lex_TestLexBuiltins(const MunitParameter params[], void *user_data) {
    const char *builtin = munit_parameters_get(params, "builtin");
    return LexExpect(builtin, BUILTIN_FUNC);
}

static MunitResult lex_TestLexInvalidIdentifiers(const MunitParameter params[], void *user_data) {
    const char *ident = munit_parameters_get(params, "ident");
    return LexExpect(ident, ERROR);
}

static MunitResult lex_TestLexOperators(const MunitParameter params[], void *user_data) {
    static LexResultTuple tokens[] = {
        { "+", OP_PLUS },
        { "+=", OP_PLUS_EQUALS },
        { "-", OP_MINUS },
        { "-=", OP_MINUS_EQUALS },
        { "*", OP_TIMES },
        { "*=", OP_TIMES_EQUALS },
        { "/", OP_DIVIDE },
        { "/=", OP_DIVIDE_EQUALS },
        { "\\", OP_IDIVIDE },
        { "\\=", OP_IDIVIDE_EQUALS },
        { "%", OP_MODULO },
        { "%=", OP_MODULO_EQUALS },
        { "**", OP_EXPONENTIATE },
        { "&&", OP_AND },
        { "||", OP_OR },
        { "!", OP_NOT },
        { "==", OP_DOUBLE_EQ },
        { "!=", OP_NOT_EQ },
        { ">", OP_GT },
        { ">=", OP_GE },
        { "<", OP_LT },
        { "<=", OP_LE },
        { ":=", OP_EQ },
        { "&", OP_BITWISE_AND },
        { "&=", OP_BITWISE_AND_EQUALS },
        { "|", OP_BITWISE_OR },
        { "|=", OP_BITWISE_OR_EQUALS },
        { "~", OP_BITWISE_NOT },
        { "^", OP_BITWISE_XOR },
        { "^=", OP_BITWISE_XOR_EQUALS },
        { ">>", OP_SHIFT_RIGHT },
        { ">>=", OP_SHIFT_RIGHT_EQUALS },
        { "<<", OP_SHIFT_LEFT },
        { "<<=", OP_SHIFT_LEFT_EQUALS },
        { "?.", OP_SAFE_REFERENCE },
        { "?[", OP_SAFE_GETATTR },
    };

    size_t len = sizeof(tokens) / sizeof(tokens[0]);
    return TestLexResultTuple(tokens, len);
}

static MunitResult lex_TestLexPunctuation(const MunitParameter params[], void *user_data) {
    static LexResultTuple tokens[] = {
        { "[", LBRACKET },
        { "]", RBRACKET },
        { "(", LPAREN },
        { ")", RPAREN },
        { "{", LBRACE },
        { "}", RBRACE },
        { ":", COLON },
        { ".", PERIOD },
        { ",", COMMA },
        { ";", SEMICOLON },
        { "?", QUESTION_MARK },
    };

    size_t len = sizeof(tokens) / sizeof(tokens[0]);
    return TestLexResultTuple(tokens, len);
}

static MunitResult lex_TestLexStrings(const MunitParameter params[], void *user_data) {
    const char *str = munit_parameters_get(params, "str");
    return LexExpect(str, STRING);
}

static MunitResult lex_TestLexInvalidStrings(const MunitParameter params[], void *user_data) {
    const char *str = munit_parameters_get(params, "str");
    return LexExpect(str, ERROR);
}

/*
 * PRIVATE FUNCTIONS
 */

static MunitResult LexCompareStrings(const char *param, const char *val) {
    /* `param` is the input string surrounded by single or double quotes, so
     * we need to only compare that string to the lexed value which will
     * exclude the surrounding quotes*/
    size_t len = strlen(param);
    munit_assert_size(len, >, 1);
    int cmp = strncmp(val, &param[1], len-2);
    munit_assert_int(cmp, ==, 0);
    return MUNIT_OK;
}

static MunitResult LexExpect(const char *param, ms_TokenType type) {
    ms_Lexer *lex = ms_LexerNew();
    munit_assert_not_null(lex);
    munit_assert(ms_LexerInitString(lex, param));

    ms_Token *tok = ms_LexerNextToken(lex);
    munit_assert_not_null(tok);
    munit_assert_int(tok->type, ==, type);
    if (type == STRING) {
        LexCompareStrings(param, dsbuf_char_ptr(tok->value));
    } else if (type != ERROR) {
        munit_assert_string_equal(dsbuf_char_ptr(tok->value), param);
    }
    munit_assert_string_equal(ms_TokenName(tok), ms_TokenTypeName(type));

    ms_LexerDestroy(lex);
    ms_TokenDestroy(tok);
    return MUNIT_OK;
}

static MunitResult TestLexResultTuple(LexResultTuple *tokens, size_t len) {
    ms_Lexer *lex = ms_LexerNew();
    munit_assert_not_null(lex);

    for (size_t i = 0; i < len; i++) {
        LexResultTuple *tuple = &tokens[0];
        munit_logf(MUNIT_LOG_INFO, "  val='%s'", tuple->val);
        (void)LexExpect(tuple->val, tuple->type);
    }

    ms_LexerDestroy(lex);
    return MUNIT_OK;
}
