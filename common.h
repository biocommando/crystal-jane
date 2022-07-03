#ifndef crystal_common_h
#define crystal_common_h

#define verbose_log(...) \
	if (verbose_logging) \
	printf("DEBUG: " __VA_ARGS__)

#endif