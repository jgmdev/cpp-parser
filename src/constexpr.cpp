#include <string>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include "constexpr.hpp"

PCToken skipWhiteSpace(PCToken *pTokenIter)
{
    while ((*pTokenIter)->type == ttWhiteSpace)
    {
        ++*pTokenIter;
    }

    return *pTokenIter;
}

PCToken peekNextToken(PCToken *pTokenIter)
{
    return skipWhiteSpace(pTokenIter);
}

PreprocessorError::PreprocessorError(const std::string &message, const Token &token)
:   mMessage(message),
    mToken(token)
{
}

std::string PreprocessorError::getMessage() const
{
    return mMessage;
}

const Token &PreprocessorError::getToken() const
{
    return mToken;
}

int ConstExprEvaluator::eval(PCToken *tokenIter)
{
    int r = conditional_expression(tokenIter);

    skipWhiteSpace(tokenIter);

    if (peekNextToken(tokenIter)->type != ttEndOfTokens)
    {
        throw PreprocessorError("Error parsing constant-expression at token " + (*tokenIter)->value, **tokenIter);
    }

    return r;
}

int ConstExprEvaluator::conditional_expression(PCToken *tokenIter)
{
    int loe = logical_or_expression(tokenIter);

    if (peekNextToken(tokenIter)->type == ttQuestion)
    {
        ++*tokenIter;
        int expr = expression(tokenIter);

        if (peekNextToken(tokenIter)->type == ttColon)
        {
            ++*tokenIter;
            int cexpr = conditional_expression(tokenIter);

            return loe ? expr : cexpr;
        }

        throw PreprocessorError("Missing : in ?: operator", **tokenIter);
    }

    return loe;
}

int ConstExprEvaluator::logical_or_expression(PCToken *tokenIter)
{
    int result = logical_and_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttOr)
    {
        ++*tokenIter;
        int tmp = logical_and_expression(tokenIter);
        result = result || tmp;
    }

    return result;
}

int ConstExprEvaluator::logical_and_expression(PCToken *tokenIter)
{
    int result = inclusive_or_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttAnd)
    {
        ++*tokenIter;
        int tmp = inclusive_or_expression(tokenIter);
        result = result && tmp;
    }

    return result;
}

int ConstExprEvaluator::inclusive_or_expression(PCToken *tokenIter)
{
    int result = exclusive_or_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttBitOr)
    {
        ++*tokenIter;
        result |= exclusive_or_expression(tokenIter);
    }

    return result;
}

int ConstExprEvaluator::exclusive_or_expression(PCToken *tokenIter)
{
    int result = and_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttBitXOr)
    {
        ++*tokenIter;
        result ^= and_expression(tokenIter);
    }

    return result;
}

int ConstExprEvaluator::and_expression(PCToken *tokenIter)
{
    int result = equality_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttBitAnd)
    {
        ++*tokenIter;
        result &= equality_expression(tokenIter);
    }

    return result;
}

int ConstExprEvaluator::equality_expression(PCToken *tokenIter)
{
    int result = relational_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttEqual || peekNextToken(tokenIter)->type == ttNotEqual)
    {
        if (peekNextToken(tokenIter)->type == ttEqual)
        {
            ++*tokenIter;
            result = result == relational_expression(tokenIter);
        }
        else // ttNotEqual
        {
            ++*tokenIter;
            result =  result != relational_expression(tokenIter);
        }
    }

    return result;
}

int ConstExprEvaluator::relational_expression(PCToken *tokenIter)
{
    int result = shift_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttLess || peekNextToken(tokenIter)->type == ttGreater || peekNextToken(tokenIter)->type == ttLessEqual || peekNextToken(tokenIter)->type == ttGreaterEqual)
    {
        if (peekNextToken(tokenIter)->type == ttLess)
        {
            ++*tokenIter;
            result = result < shift_expression(tokenIter);
        }
        else if (peekNextToken(tokenIter)->type == ttGreater)
        {
            ++*tokenIter;
            result = result > shift_expression(tokenIter);
        }
        else if (peekNextToken(tokenIter)->type == ttLessEqual)
        {
            ++*tokenIter;
            result = result <= shift_expression(tokenIter);
        }
        else // ttGreaterEqual
        {
            ++*tokenIter;
            result = result >= shift_expression(tokenIter);
        }
    }

    return result;
}

int ConstExprEvaluator::shift_expression(PCToken *tokenIter)
{
    int result = additive_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttLShift || peekNextToken(tokenIter)->type == ttRShift)
    {
        if (peekNextToken(tokenIter)->type == ttLShift)
        {
            ++*tokenIter;
            result <<= additive_expression(tokenIter);
        }
        else // ttRShift
        {
            ++*tokenIter;
            result >>= additive_expression(tokenIter);
        }
    }

    return result;
}

int ConstExprEvaluator::additive_expression(PCToken *tokenIter)
{
    int result = multiplicative_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttPlus || peekNextToken(tokenIter)->type == ttMinus)
    {
        if (peekNextToken(tokenIter)->type == ttPlus)
        {
            ++*tokenIter;
            result += multiplicative_expression(tokenIter);
        }
        else // ttMINUS
        {
            ++*tokenIter;
            result -= multiplicative_expression(tokenIter);
        }
    }

    return result;
}

int ConstExprEvaluator::multiplicative_expression(PCToken *tokenIter)
{
    int result = unary_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttTimes || peekNextToken(tokenIter)->type == ttDivide || peekNextToken(tokenIter)->type == ttModulo)
    {
        if (peekNextToken(tokenIter)->type == ttTimes)
        {
            ++*tokenIter;
            result *= unary_expression(tokenIter);
        }
        else if (peekNextToken(tokenIter)->type == ttDivide)
        {
            ++*tokenIter;
            result /= unary_expression(tokenIter);
        }
        else // ttModulo
        {
            ++*tokenIter;
            result %= unary_expression(tokenIter);
        }
    }

    return result;
}

int ConstExprEvaluator::unary_expression(PCToken *tokenIter)
{
    if (peekNextToken(tokenIter)->type == ttPlus)
    {
        ++*tokenIter;
        return unary_expression(tokenIter);
    }

    if (peekNextToken(tokenIter)->type == ttMinus)
    {
        ++*tokenIter;
        return -unary_expression(tokenIter);
    }

    if (peekNextToken(tokenIter)->type == ttNot)
    {
        ++*tokenIter;
        return !unary_expression(tokenIter);
    }

    if (peekNextToken(tokenIter)->type == ttBitNeg)
    {
        ++*tokenIter;
        return ~unary_expression(tokenIter);
    }

    return primary_expression(tokenIter);
}

int ConstExprEvaluator::primary_expression(PCToken *tokenIter)
{
    if (peekNextToken(tokenIter)->type == ttLParen)
    {
        ++*tokenIter;
        int result = expression(tokenIter);

        if (peekNextToken(tokenIter)->type == ttRParen)
        {
            ++*tokenIter;
            return result;
        }

        throw PreprocessorError("Expected ')', but found " + peekNextToken(tokenIter)->value, **tokenIter);
    }

    return literal(tokenIter);
}

int ConstExprEvaluator::literal(PCToken *tokenIter)
{
    switch (peekNextToken(tokenIter)->type)
    {
        case ttNumber:
        {
            int result;
            char dummy1 = 0;
            char dummy2 = 0;
            char dummy3 = 0;
            int count = std::sscanf(peekNextToken(tokenIter)->value.c_str(), "%i%c%c%c", &result, &dummy1, &dummy2, &dummy3);

            switch (count)
            {
                case 1:
                    ++*tokenIter;
                    return result;

                case 2:
                    if (dummy1 == 'u' || dummy1 == 'U' || dummy1 == 'l' || dummy1 == 'L')
                    {
                        ++*tokenIter;
                        return result;
                    }
                    break;

                case 3:
                    if (dummy1 == 'u' || dummy1 == 'U')
                    {
                        if (dummy2 == 'l' || dummy2 == 'L')
                        {
                            ++*tokenIter;
                            return result;
                        }
                    }

                    if (dummy1 == 'l' || dummy1 == 'L')
                    {
                        if (dummy2 == 'u' || dummy2 == 'U')
                        {
                            ++*tokenIter;
                            return result;
                        }
                    }
                    break;

                default:
                    break;
            }

            double fresult;

            count = std::sscanf(peekNextToken(tokenIter)->value.c_str(), "%lg%c%c%c", &fresult, &dummy1, &dummy2, &dummy3);

            switch (count)
            {
                case 1:
                    ++*tokenIter;
                    return static_cast<int>(fresult);

                case 2:
                    if (dummy1 == 'f' || dummy1 == 'F' || dummy1 == 'l' || dummy1 == 'L')
                    {
                        ++*tokenIter;
                        return static_cast<int>(fresult);
                    }
                    break;

                default:
                    throw PreprocessorError(peekNextToken(tokenIter)->value + " is not a valid number", **tokenIter);
            }
        }

        case ttCharLiteral:
        {
            size_t pos = 0;

            if (peekNextToken(tokenIter)->value[pos] == 'L')
            {
                ++pos;
            }

            if (peekNextToken(tokenIter)->value.length() == pos + 3) // 'c' or L'c' (pos points to the first ')
            {
                int ret = peekNextToken(tokenIter)->value[++pos];
                ++*tokenIter;
                return ret;
            }

            ++pos;

            size_t end = peekNextToken(tokenIter)->value.length() - 1;

            if (end == pos || peekNextToken(tokenIter)->value[pos] != '\\')
            {
                throw PreprocessorError(peekNextToken(tokenIter)->value + " is not a valid character", **tokenIter);
            }

            ++pos;

            if (peekNextToken(tokenIter)->value.length() == pos + 2) // '\n', '\r'... or L'\n', L'\r'... (pos points to what's after \)
            {
                switch (peekNextToken(tokenIter)->value[pos])
                {
                    case '\\':
                    case '\'':
                    case '"':
                    case '?':
                    {
                        int ret = peekNextToken(tokenIter)->value[pos];
                        ++*tokenIter;
                        return ret;
                    }

                    case 'a':
                        ++*tokenIter;
                        return '\a';

                    case 'b':
                        ++*tokenIter;
                        return '\b';

                    case 'f':
                        ++*tokenIter;
                        return '\f';

                    case 'n':
                        ++*tokenIter;
                        return '\n';

                    case 'r':
                        ++*tokenIter;
                        return '\r';

                    case 't':
                        ++*tokenIter;
                        return '\t';

                    case 'v':
                        ++*tokenIter;
                        return '\v';

                    default:
                        if (std::isdigit(peekNextToken(tokenIter)->value[pos]))
                        {
                            int ret = peekNextToken(tokenIter)->value[pos] - '0';
                            ++*tokenIter;
                            return ret;
                        }

                        throw PreprocessorError(peekNextToken(tokenIter)->value + " is not a valid character", **tokenIter);
                }
            }

            int result;

            if (peekNextToken(tokenIter)->value[pos] == 'x')
            {
                ++pos;
                std::string tmp = peekNextToken(tokenIter)->value.substr(pos, peekNextToken(tokenIter)->value.length() - pos);
                int ret = static_cast<int>(std::strtol(tmp.c_str(), 0, 16));
                ++*tokenIter;
                return ret;
            }

            std::sscanf(&peekNextToken(tokenIter)->value.c_str()[pos], "%i", &result);
            ++*tokenIter;
            return result;

        }
        break;

        case ttTrue:
            ++*tokenIter;
            return 1;

        case ttFalse:
            ++*tokenIter;
            return 0;

        default:
            throw PreprocessorError(peekNextToken(tokenIter)->value + " is not a valid literal", **tokenIter);
    }
}

int ConstExprEvaluator::expression(PCToken *tokenIter)
{
    int result = conditional_expression(tokenIter);

    while (peekNextToken(tokenIter)->type == ttComma)
    {
        ++*tokenIter;
        result = conditional_expression(tokenIter);
    }

    return result;
}
