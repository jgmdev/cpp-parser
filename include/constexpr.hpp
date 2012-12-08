#ifndef CONSTEXPR_HPP
#define CONSTEXPR_HPP

#include <string>

enum TokenType
{
    ttWhiteSpace,
    ttQuestion, // ?
    ttColon, // :
    ttOr, // ||
    ttAnd, // &&
    ttBitOr, // |
    ttBitXOr, // ^
    ttBitAnd, // &
    ttEqual, // ==
    ttNotEqual, // !=
    ttLess, // <
    ttGreater, // >
    ttLessEqual, // <=
    ttGreaterEqual, // >=
    ttLShift, // <<
    ttRShift, // >>
    ttPlus, // +
    ttMinus, // -
    ttTimes, // *
    ttDivide, // /
    ttModulo, // %
    ttNot, // !
    ttBitNeg, // ~
    ttLParen, // (
    ttRParen, // )
    ttNumber,
    ttCharLiteral,
    ttTrue, // true
    ttFalse, // false
    ttComma, // ,
    ttEndOfTokens
};

struct Token
{
    TokenType   type;
    std::string value;
};

typedef const Token *PCToken;

class PreprocessorError
{
public:
    /* ctor */          PreprocessorError(const std::string &message, const Token &token);

    std::string         getMessage() const;
    const Token &       getToken() const;

private:
    std::string         mMessage;
    Token               mToken;
};

class ConstExprEvaluator
{
    public:
        /**
            \brief The function that evaluates the constant expression.
            \return The value of truth of the constant expression.

            eval: \n
                 constant-expression \n

            constant-expression: \n
                 conditional-expression
        */
        static int eval(PCToken *tokenIter);

    private:
        /**
            \brief Evaluates a conditional expression.

            conditional-expression: \n
                 logical-or-expression \n
                 logical-or-expression ? expression : assignment-expression
        */
        static int conditional_expression(PCToken *tokenIter);

        /**
            \brief Evaluates a logical-or-expression.

            logical-or-expression: \n
                 logical-and-expression \n
                 logical-or-expression || logical-and-expression \n
        */
        static int logical_or_expression(PCToken *tokenIter);

        /**
            \brief Evaluates a logical-and-expression.

            logical-and-expression: \n
                 inclusive-or-expression \n
                 logical-and-expression && inclusive-or-expression
        */
        static int logical_and_expression(PCToken *tokenIter);

        /**
            \brief Evaluates an inclusive-or-expression.

            inclusive-or-expression: \n
                 exclusive-or-expression \n
                 inclusive-or-expression | exclusive-or-expression
        */
        static int inclusive_or_expression(PCToken *tokenIter);

        /**
            \brief Evaluates an exclusive-or-expression.

            exclusive-or-expression: \n
                 and-expression \n
                 exclusive-or-expression ^ and-expression
        */
        static int exclusive_or_expression(PCToken *tokenIter);

        /**
            \brief Evaluates an and-expression.

            and-expression: \n
                 equality-expression \n
                 and-expression & equality-expression \n
        */
        static int and_expression(PCToken *tokenIter);

        /**
            \brief Evaluates an equality-expression.

            equality-expression: \n
                 relational-expression \n
                 equality-expression == relational-expression \n
                 equality-expression != relational-expression
        */
        static int equality_expression(PCToken *tokenIter);

        /**
            \brief Evaluates a relational-expression.

            relational-expression: \n
                 shift-expression \n
                 relational-expression <  shift-expression \n
                 relational-expression >  shift-expression \n
                 relational-expression <= shift-expression \n
                 relational-expression >= shift-expression
        */
        static int relational_expression(PCToken *tokenIter);

        /**
            \brief Evaluates a shift-expression.

            shift-expression: \n
                 additive-expression \n
                 shift-expression >> additive-expression \n
                 shift-expression << additive-expression
        */
        static int shift_expression(PCToken *tokenIter);

        /**
            \brief Evaluates an additive-expression.

            additive-expression: \n
                 multiplicative-expression \n
                 additive-expression + multiplicative-expression \n
                 additive-expression - multiplicative-expression
        */
        static int additive_expression(PCToken *tokenIter);

        /**
            \brief Evaluates a multiplicative-expression.

            multiplicative-expression:
                 unary-expression \n
                 multiplicative-expression * unary-expression \n
                 multiplicative-expression / unary-expression \n
                 multiplicative-expression % unary-expression \n
        */
        static int multiplicative_expression(PCToken *tokenIter);

        /**
            \brief Evaluates an unary-expression.

            unary-expression: \n
                 primary-expression \n
                 unary-operator unary-expression
        */
        static int unary_expression(PCToken *tokenIter);

        /**
            \brief Evaluates a primary-expression.

            primary-expression: \n
                 literal \n
                 ( expression )
        */
        static int primary_expression(PCToken *tokenIter);

        /**
            \brief Evaluates a literal.

            literal: \n
                 pp-number \n
                 character-literal \n
                 boolean-literal
        */
        static int literal(PCToken *tokenIter);

        /**
            \brief Evaluates an expression.

            expression: \n
                 conditional-expression \n
                 expression , conditional-expression
        */
        static int expression(PCToken *tokenIter);
};

/**Example USAGE

void eval(PCToken tokens)
{
    try
    {
        PCToken pcToken = &tokens[0];
        std::cout << "Expression: " << ConstExprEvaluator::eval(&pcToken) << "\n";
    }
    catch (const PreprocessorError &prepError)
    {
        std::cerr << "Exception: " << prepError.getMessage() << "\n";
    }
}

int main()
{
    // 3 * 2 + 5 > 5*(12+8)
    const Token preprocessedInput1[] =
    {
        { ttNumber, "3" },
        { ttWhiteSpace, " " },
        { ttTimes, "*" },
        { ttWhiteSpace, " " },
        { ttNumber, "2" },
        { ttWhiteSpace, " " },
        { ttPlus, "+" },
        { ttNumber, "5" },
        { ttWhiteSpace, " " },
        { ttGreater, ">" },
        { ttNumber, "5" },
        { ttTimes, "*" },
        { ttLParen, "(" },
        { ttNumber, "12" },
        { ttPlus, "+" },
        { ttNumber, "8" },
        { ttRParen, ")" },
        { ttEndOfTokens, "" }
    };

    eval(&preprocessedInput1[0]); // Result must be false (0)

    // 3 * 2 + 5 < 5*(12+8)
    const Token preprocessedInput2[] =
    {
        { ttNumber, "3" },
        { ttWhiteSpace, " " },
        { ttTimes, "*" },
        { ttWhiteSpace, " " },
        { ttNumber, "2" },
        { ttWhiteSpace, " " },
        { ttPlus, "+" },
        { ttNumber, "5" },
        { ttWhiteSpace, " " },
        { ttLess, "<" },
        { ttNumber, "5" },
        { ttTimes, "*" },
        { ttLParen, "(" },
        { ttNumber, "12" },
        { ttPlus, "+" },
        { ttNumber, "8" },
        { ttRParen, ")" },
        { ttEndOfTokens, "" }
    };

    eval(&preprocessedInput2[0]); // Result must be true (1)

    // 3 * 2 + 5 < 5*(12+8
    const Token preprocessedInput3[] =
    {
        { ttNumber, "3" },
        { ttWhiteSpace, " " },
        { ttTimes, "*" },
        { ttWhiteSpace, " " },
        { ttNumber, "2" },
        { ttWhiteSpace, " " },
        { ttPlus, "+" },
        { ttNumber, "5" },
        { ttWhiteSpace, " " },
        { ttLess, "<" },
        { ttNumber, "5" },
        { ttTimes, "*" },
        { ttLParen, "(" },
        { ttNumber, "12" },
        { ttPlus, "+" },
        { ttNumber, "8" },
        { ttEndOfTokens, "" }
    };

    eval(&preprocessedInput3[0]); // Exception due to missing ')'

    // !3 & 2 % 5 + 5|(12+~8)
    const Token preprocessedInput4[] =
    {
        { ttNot, "!" },
        { ttNumber, "3" },
        { ttWhiteSpace, " " },
        { ttBitAnd, "&" },
        { ttWhiteSpace, " " },
        { ttNumber, "2" },
        { ttWhiteSpace, " " },
        { ttModulo, "%" },
        { ttNumber, "5" },
        { ttWhiteSpace, " " },
        { ttPlus, "+" },
        { ttNumber, "5" },
        { ttBitOr, "|" },
        { ttLParen, "(" },
        { ttNumber, "12" },
        { ttPlus, "+" },
        { ttBitNeg, "~" },
        { ttNumber, "8" },
        { ttRParen, ")" },
        { ttEndOfTokens, "" }
    };

    eval(&preprocessedInput4[0]); // Result must be 3
}

*/

#endif
