/* Prints the provided format string */
void log(const char* fmt, ...);

/* Prints the provided format string if DEBUG is enabled (i.e. 0) */
void debug(const char* fmt, ...);

/* Prints the provided format string then kills the program. */
void fatal(const char* fmt, ...);
