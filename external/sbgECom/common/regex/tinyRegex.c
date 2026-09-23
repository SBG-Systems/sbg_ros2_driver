/*
 *
 * Mini regex-module inspired by Rob Pike's regex code described in:
 *
 * http://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
 *
 *
 *
 * Supports:
 * ---------
 *   '.'        Dot, matches any character
 *   '^'        Start anchor, matches beginning of string
 *   '$'        End anchor, matches end of string
 *   '*'        Asterisk, match zero or more (greedy)
 *   '+'        Plus, match one or more (greedy)
 *   '?'        Question, match zero or one (non-greedy)
 *   '[abc]'    Character class, match if one of {'a', 'b', 'c'}
 *   '[^abc]'   Inverted class, match if NOT one of {'a', 'b', 'c'} -- NOTE: feature is currently broken!
 *   '[a-zA-Z]' Character ranges, the character set of the ranges { a-z | A-Z }
 *   '\s'       Whitespace, \t \f \r \n \v and spaces
 *   '\S'       Non-whitespace
 *   '\w'       Alphanumeric, [a-zA-Z0-9_]
 *   '\W'       Non-alphanumeric
 *   '\d'       Digits, [0-9]
 *   '\D'       Non-digits
 *
 *
 */



#include "tinyRegex.h"
#include <stdio.h>
#include <ctype.h>


enum { UNUSED, DOT, BEGIN, END, QUESTIONMARK, STAR, PLUS, CHAR, CHAR_CLASS, INV_CHAR_CLASS, DIGIT, NOT_DIGIT, ALPHA, NOT_ALPHA, WHITESPACE, NOT_WHITESPACE, /* BRANCH */ };



//----------------------------------------------------------------------//
//- Private methods                                                    -//
//----------------------------------------------------------------------//

static int matchpattern(const sbgRegexItem *pattern, const char* text, size_t* matchlength);
static int matchstar(const sbgRegexItem *p, const sbgRegexItem *pattern, const char* text, size_t* matchlength);
static int matchplus(const sbgRegexItem *p, const sbgRegexItem *pattern, const char* text, size_t* matchlength);

static int matchdigit(char c)
{
    return isdigit(c);
}
static int matchalpha(char c)
{
    return isalpha(c);
}
static int matchwhitespace(char c)
{
    return isspace(c);
}
static int matchalphanum(char c)
{
    return ((c == '_') || matchalpha(c) || matchdigit(c));
}
static int matchrange(char c, const char* str)
{
    return ((c != '-')
        && (str[0] != '\0')
        && (str[0] != '-')
        && (str[1] == '-')
        && (str[2] != '\0')
        && ((c >= str[0])
            && (c <= str[2])));
}
static int matchdot(char c)
{
#if defined(RE_DOT_MATCHES_NEWLINE) && (RE_DOT_MATCHES_NEWLINE == 1)
    (void)c;
    return 1;
#else
    return c != '\n' && c != '\r';
#endif
}
static int ismetachar(char c)
{
    return ((c == 's') || (c == 'S') || (c == 'w') || (c == 'W') || (c == 'd') || (c == 'D'));
}

static int matchmetachar(char c, const char* str)
{
    switch (str[0])
    {
    case 'd': return  matchdigit(c);
    case 'D': return !matchdigit(c);
    case 'w': return  matchalphanum(c);
    case 'W': return !matchalphanum(c);
    case 's': return  matchwhitespace(c);
    case 'S': return !matchwhitespace(c);
    default:  return (c == str[0]);
    }
}

static int matchcharclass(char c, const char* str)
{
    do
    {
        if (matchrange(c, str))
        {
            return 1;
        }
        else if (str[0] == '\\')
        {
            /* Escape-char: increment str-ptr and match on next char */
            str += 1;
            if (matchmetachar(c, str))
            {
                return 1;
            }
            else if ((c == str[0]) && !ismetachar(c))
            {
                return 1;
            }
        }
        else if (c == str[0])
        {
            if (c == '-')
            {
                return ((str[-1] == '\0') || (str[1] == '\0'));
            }
            else
            {
                return 1;
            }
        }
    } while (*str++ != '\0');

    return 0;
}

static int matchone(const sbgRegexItem *p, char c)
{
    switch (p->type)
    {
    case DOT:            return matchdot(c);
    case CHAR_CLASS:     return  matchcharclass(c, (const char*)p->u.ccl);
    case INV_CHAR_CLASS: return !matchcharclass(c, (const char*)p->u.ccl);
    case DIGIT:          return  matchdigit(c);
    case NOT_DIGIT:      return !matchdigit(c);
    case ALPHA:          return  matchalphanum(c);
    case NOT_ALPHA:      return !matchalphanum(c);
    case WHITESPACE:     return  matchwhitespace(c);
    case NOT_WHITESPACE: return !matchwhitespace(c);
    default:             return  (p->u.ch == c);
    }
}

static int matchstar(const sbgRegexItem *p, const sbgRegexItem* pattern, const char* text, size_t *matchlength)
{
    size_t prelen = *matchlength;
    const char* prepoint = text;
    while ((text[0] != '\0') && matchone(p, *text))
    {
        text++;
        (*matchlength)++;
    }
    while (text >= prepoint)
    {
        if (matchpattern(pattern, text--, matchlength))
            return 1;
        (*matchlength)--;
    }

    *matchlength = prelen;
    return 0;
}

static int matchplus(const sbgRegexItem *p, const sbgRegexItem *pattern, const char* text, size_t *matchlength)
{
    const char* prepoint = text;
    while ((text[0] != '\0') && matchone(p, *text))
    {
        text++;
        (*matchlength)++;
    }
    while (text > prepoint)
    {
        if (matchpattern(pattern, text--, matchlength))
            return 1;
        (*matchlength)--;
    }

    return 0;
}

static int matchquestion(const sbgRegexItem *p, const sbgRegexItem* pattern, const char* text, size_t *matchlength)
{
    if (p->type == UNUSED)
        return 1;
    if (matchpattern(pattern, text, matchlength))
        return 1;
    if (*text && matchone(p, *text++))
    {
        if (matchpattern(pattern, text, matchlength))
        {
            (*matchlength)++;
            return 1;
        }
    }
    return 0;
}


#if 0

/* Recursive matching */
static int matchpattern(sbgRegexItem* pattern, const char* text, size_t *matchlength)
{
    size_t pre = *matchlength;
    if ((pattern[0].type == UNUSED) || (pattern[1].type == QUESTIONMARK))
    {
        return matchquestion(pattern[1], &pattern[2], text, matchlength);
    }
    else if (pattern[1].type == STAR)
    {
        return matchstar(pattern[0], &pattern[2], text, matchlength);
    }
    else if (pattern[1].type == PLUS)
    {
        return matchplus(pattern[0], &pattern[2], text, matchlength);
    }
    else if ((pattern[0].type == END) && pattern[1].type == UNUSED)
    {
        return text[0] == '\0';
    }
    else if ((text[0] != '\0') && matchone(pattern[0], text[0]))
    {
        (*matchlength)++;
        return matchpattern(&pattern[1], text + 1);
    }
    else
    {
        *matchlength = pre;
        return 0;
    }
}

#else

/* Iterative matching */
static int matchpattern(const sbgRegexItem* pattern, const char* text, size_t *matchlength)
{
    size_t pre = *matchlength;

    do
    {
        if ((pattern[0].type == UNUSED) || (pattern[1].type == QUESTIONMARK))
        {
            return matchquestion(&pattern[0], &pattern[2], text, matchlength);
        }
        else if (pattern[1].type == STAR)
        {
            return matchstar(&pattern[0], &pattern[2], text, matchlength);
        }
        else if (pattern[1].type == PLUS)
        {
            return matchplus(&pattern[0], &pattern[2], text, matchlength);
        }
        else if ((pattern[0].type == END) && pattern[1].type == UNUSED)
        {
            return (text[0] == '\0');
        }
        /*  Branching is not working properly
            else if (pattern[1].type == BRANCH)
            {
              return (matchpattern(pattern, text) || matchpattern(&pattern[2], text));
            }
        */
        (*matchlength)++;
    } while ((text[0] != '\0') && matchone(pattern++, *text++));

    *matchlength = pre;
    return 0;
}

#endif

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

SBG_COMMON_LIB_API bool sbgRegexCompile(SbgRegex *pRegex, const char *pPattern)
{
    int ccl_bufidx = 1;

    char c;     /* current char in pattern   */
    int i = 0;  /* index into pattern        */
    int j = 0;  /* index into re_compiled    */

    assert(pRegex);
    assert(pPattern);

    while (pPattern[i] != '\0' && (j + 1 < MAX_REGEXP_OBJECTS))
    {
        c = pPattern[i];

        switch (c)
        {
            /* Meta-characters: */
        case '^': {    pRegex->re_compiled[j].type = BEGIN;           } break;
        case '$': {    pRegex->re_compiled[j].type = END;             } break;
        case '.': {    pRegex->re_compiled[j].type = DOT;             } break;
        case '*': {    pRegex->re_compiled[j].type = STAR;            } break;
        case '+': {    pRegex->re_compiled[j].type = PLUS;            } break;
        case '?': {    pRegex->re_compiled[j].type = QUESTIONMARK;    } break;
            /*    case '|': {    pRegex->re_compiled[j].type = BRANCH;          } break; <-- not working properly */

                  /* Escaped character-classes (\s \w ...): */
        case '\\':
        {
            if (pPattern[i + 1] != '\0')
            {
                /* Skip the escape-char '\\' */
                i += 1;
                /* ... and check the next */
                switch (pPattern[i])
                {
                    /* Meta-character: */
                case 'd': {    pRegex->re_compiled[j].type = DIGIT;            } break;
                case 'D': {    pRegex->re_compiled[j].type = NOT_DIGIT;        } break;
                case 'w': {    pRegex->re_compiled[j].type = ALPHA;            } break;
                case 'W': {    pRegex->re_compiled[j].type = NOT_ALPHA;        } break;
                case 's': {    pRegex->re_compiled[j].type = WHITESPACE;       } break;
                case 'S': {    pRegex->re_compiled[j].type = NOT_WHITESPACE;   } break;

                    /* Escaped character, e.g. '.' or '$' */
                default:
                {
                    pRegex->re_compiled[j].type = CHAR;
                    pRegex->re_compiled[j].u.ch = pPattern[i];
                } break;
                }
            }
            /* '\\' as last char in pattern -> invalid regular expression. */
    /*
            else
            {
              re_compiled[j].type = CHAR;
              re_compiled[j].ch = pattern[i];
            }
    */
        } break;

        /* Character class: */
        case '[':
        {
            /* Remember where the char-buffer starts. */
            int buf_begin = ccl_bufidx;

            /* Look-ahead to determine if negated */
            if (pPattern[i + 1] == '^')
            {
                pRegex->re_compiled[j].type = INV_CHAR_CLASS;
                i += 1; /* Increment i to avoid including '^' in the char-buffer */
                if (pPattern[i + 1] == 0) /* incomplete pattern, missing non-zero char after '^' */
                {
                    return false;
                }
            }
            else
            {
                pRegex->re_compiled[j].type = CHAR_CLASS;
            }

            /* Copy characters inside [..] to buffer */
            while ((pPattern[++i] != ']')
                && (pPattern[i] != '\0')) /* Missing ] */
            {
                if (pPattern[i] == '\\')
                {
                    if (ccl_bufidx >= MAX_CHAR_CLASS_LEN - 1)
                    {
                        //fputs("exceeded internal buffer!\n", stderr);
                        return false;
                    }
                    if (pPattern[i + 1] == 0) /* incomplete pattern, missing non-zero char after '\\' */
                    {
                        return false;
                    }
                    pRegex->ccl_buf[ccl_bufidx++] = pPattern[i++];
                }
                else if (ccl_bufidx >= MAX_CHAR_CLASS_LEN)
                {
                    //fputs("exceeded internal buffer!\n", stderr);
                    return false;
                }
                pRegex->ccl_buf[ccl_bufidx++] = pPattern[i];
            }
            if (ccl_bufidx >= MAX_CHAR_CLASS_LEN)
            {
                /* Catches cases such as [00000000000000000000000000000000000000][ */
                //fputs("exceeded internal buffer!\n", stderr);
                return false;
            }
            /* Null-terminate string end */
            pRegex->ccl_buf[ccl_bufidx++] = 0;
            pRegex->re_compiled[j].u.ccl = &pRegex->ccl_buf[buf_begin];
        } break;

        /* Other characters: */
        default:
        {
            pRegex->re_compiled[j].type = CHAR;
            pRegex->re_compiled[j].u.ch = c;
        } break;
        }
        /* no buffer-out-of-bounds access on invalid patterns - see https://github.com/kokke/tiny-regex-c/commit/1a279e04014b70b0695fba559a7c05d55e6ee90b */
        if (pPattern[i] == 0)
        {
            return false;
        }

        i += 1;
        j += 1;
    }
    /* 'UNUSED' is a sentinel used to indicate end-of-pattern */
    pRegex->re_compiled[j].type = UNUSED;

    return true;
}

SBG_COMMON_LIB_API void sbgRegexPrint(const SbgRegex *pRegex)
{
    const char* types[] = { "UNUSED", "DOT", "BEGIN", "END", "QUESTIONMARK", "STAR", "PLUS", "CHAR", "CHAR_CLASS", "INV_CHAR_CLASS", "DIGIT", "NOT_DIGIT", "ALPHA", "NOT_ALPHA", "WHITESPACE", "NOT_WHITESPACE", "BRANCH" };

    int i;
    int j;
    char c;

    assert(pRegex);

    for (i = 0; i < MAX_REGEXP_OBJECTS; ++i)
    {
        if (pRegex->re_compiled[i].type == UNUSED)
        {
            break;
        }

        printf("type: %s", types[pRegex->re_compiled[i].type]);
        if (pRegex->re_compiled[i].type == CHAR_CLASS || pRegex->re_compiled[i].type == INV_CHAR_CLASS)
        {
            printf(" [");
            for (j = 0; j < MAX_CHAR_CLASS_LEN; ++j)
            {
                c = pRegex->re_compiled[i].u.ccl[j];
                if ((c == '\0') || (c == ']'))
                {
                    break;
                }
                printf("%c", c);
            }
            printf("]");
        }
        else if (pRegex->re_compiled[i].type == CHAR)
        {
            printf(" '%c'", pRegex->re_compiled[i].u.ch);
        }
        printf("\n");
    }
}

SBG_COMMON_LIB_API int32_t sbgRegexMatchp(const SbgRegex *pRegex, const char *pText, size_t *pMatchLength)
{
    assert(pRegex);
    assert(pText);
    assert(pMatchLength);

    *pMatchLength = 0;

    if (pRegex->re_compiled[0].type == BEGIN)
    {
        return ((matchpattern(&pRegex->re_compiled[1], pText, pMatchLength)) ? 0 : -1);
    }
    else
    {
        int idx = -1;

        do
        {
            idx += 1;

            if (matchpattern(pRegex->re_compiled, pText, pMatchLength))
            {
                if (pText[0] == '\0')
                    return -1;

                return idx;
            }
        } while (*pText++ != '\0');
    }

    return -1;
}

SBG_COMMON_LIB_API int32_t sbgRegexMatch(const char *pPattern, const char *pText, size_t *pMatchLength)
{
    SbgRegex    regex;

    assert(pPattern);
    assert(pText);
    assert(pMatchLength);

    if (sbgRegexCompile(&regex, pPattern))
    {
        return sbgRegexMatchp(&regex, pText, pMatchLength);
    }
    else
    {
        return -1;
    }
}
