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

#ifndef _TINY_REGEX_C
#define _TINY_REGEX_C

#include <sbgCommon.h>

//----------------------------------------------------------------------//
//- Header (open extern C block)                                       -//
//----------------------------------------------------------------------//
#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------//
//- Public definitions                                                 -//
//----------------------------------------------------------------------//

/*!
 * Define to 0 if you DON'T want '.' to match '\r' + '\n'
 */
#ifndef RE_DOT_MATCHES_NEWLINE
    #define RE_DOT_MATCHES_NEWLINE  1
#endif

/*!
 * Max number of regex symbols in expression.
 */
#define MAX_REGEXP_OBJECTS          30

/*!
 * Max length of character-class buffer in.
 */
#define MAX_CHAR_CLASS_LEN          40

/*!
 * Compiled regex entry.
 */
typedef struct _sbgRegexItem
{
    unsigned char  type;   /* CHAR, STAR, etc.                      */
    union
    {
        unsigned char  ch;   /*      the character itself             */
        unsigned char* ccl;  /*  OR  a pointer to characters in class */
    } u;
} sbgRegexItem;

/*!
 * Compiled regex object
 */
typedef struct _SbgRegex
{
    sbgRegexItem            re_compiled[MAX_REGEXP_OBJECTS];    /*!< MAX_REGEXP_OBJECTS is the max number of symbols in the expression. */
    unsigned char           ccl_buf[MAX_CHAR_CLASS_LEN];        /*!< MAX_CHAR_CLASS_LEN determines the size of buffer for chars in all char-classes in the expression. */
} SbgRegex;

//----------------------------------------------------------------------//
//- Public methods                                                     -//
//----------------------------------------------------------------------//

/*!
 * Compile regex string pattern to a regex_t - array.
 * 
 * Example valid regex for this library: "[Hh]ello [Ww]orld\\s*[!]?"
 * 
 * \param[in]   pRegex                  Regex instance.
 * \param[in]   pattern                 The regex pattern to compile.
 * \return                              true if the regex has been compiled successfully.
 */
SBG_COMMON_LIB_API bool sbgRegexCompile(SbgRegex *pRegex, const char *pPattern);

/*!
 * Print the compiled regex
 * 
 * \param[in]   pRegex                  Regex instance.
 */
SBG_COMMON_LIB_API void sbgRegexPrint(const SbgRegex *pRegex);

/*!
 * Find matches of the compiled pattern inside text.
 * 
 * \param[in]   pRegex                  Regex instance.
 * \param[in]   pText                   The text to check against the regex expression.
 * \param[out]  pMatchLength            Number of chars that have matched.
 * \return                              0 if the regex matches
 *                                      -1 in case of error or if the regex doesn't match.
 */
SBG_COMMON_LIB_API int32_t sbgRegexMatchp(const SbgRegex *pRegex, const char *pText, size_t *pMatchLength);

/*!
 * Find matches of the txt pattern inside text (will compile automatically first).
 * 
 * \param[in]   pPattern                The regex expression.
 * \param[in]   pText                   The text to check against the regex expression.
 * \param[out]  pMatchLength            Number of chars that have matched.
 * \return                              0 if the regex matches
 *                                      -1 in case of error or if the regex doesn't match.
 */
SBG_COMMON_LIB_API int32_t sbgRegexMatch(const char *pPattern, const char *pText, size_t *pMatchLength);

//----------------------------------------------------------------------//
//- Footer (close extern C block)                                      -//
//----------------------------------------------------------------------//
#ifdef __cplusplus
}
#endif

#endif // _TINY_REGEX_C
