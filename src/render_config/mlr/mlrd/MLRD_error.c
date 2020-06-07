/* vi:set ts=8 sw=4 ai:
 *[
 *$     MLRD_error.c
 *
 *      Steve Gray - Wed Jul 19 12:29:23 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	Media Logic error handler
 *
 *      Usage:
 *		ERROR()
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *]
 */


ERROR( func, msg)
char	*func;
char	*msg;
{
    printf( "ERROR: %s\n", msg);
    printf( "(detected by %s)\n", func);

    exit(1);
}


