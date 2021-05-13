#ifndef LOG_H_
#define LOG_H_

/* Log Levels */
#define LOG_LEVEL_PRINT		0 /* Force to print */
#define LOG_LEVEL_ERR		1 /* Errors */
#define LOG_LEVEL_WARN		2 /* Warnings */
#define LOG_LEVEL_INFO		3 /* Basic info */
#define LOG_LEVEL_DBG		4 /* Detailled debug */

/*------------------------------------------------------------------------------*/
#include "project-conf.h"	    /* make sure the LOG_LEVELS defined */

/*------------------------------------------------------------------------------*/
extern int print_with_func_line;    /* defined in test.c */
extern int verbose_output_enabled;  /* defined in test.c */

#ifndef LOG_FEATURE_ENABLED
#define LOG_FEATURE_ENABLED 0
#endif

#if LOG_FEATURE_ENABLED

#define LOG(with_level_str, level, level_str, ...) do {		\
	if (verbose_output_enabled || (level <= (LOG_LEVEL))) {	\
	    if (with_level_str) {				\
		if (print_with_func_line) {			\
		    printf("%s[%s %d]: ", level_str,		\
			    __FUNCTION__, __LINE__);		\
		} else {					\
		    printf("%s", level_str);			\
		}						\
	    }							\
	    printf(__VA_ARGS__);				\
	}							\
    } while (0)

#else /* LOG_FEATURE_ENABLED */

#define LOG(...) do {} while(0)

#endif /* LOG_FEATURE_ENABLED */

/*------------------------------------------------------------------------------*/
#define LOG_PRINT(...)		LOG(1, LOG_LEVEL_PRINT, "PRI : ", __VA_ARGS__)
#define LOG_ERR(...)		LOG(1, LOG_LEVEL_ERR, "ERR : ", __VA_ARGS__)
#define LOG_WARN(...)		LOG(1, LOG_LEVEL_WARN, "WARN: ", __VA_ARGS__)
#define LOG_INFO(...)		LOG(1, LOG_LEVEL_INFO, "INFO: ", __VA_ARGS__)
#define LOG_DBG(...)		LOG(1, LOG_LEVEL_DBG, "DBG : ", __VA_ARGS__)

/*------------------------------------------------------------------------------*/
#define LOG_PRINT_(...)		LOG(0, LOG_LEVEL_PRINT, "PRI : ", __VA_ARGS__)
#define LOG_ERR_(...)		LOG(0, LOG_LEVEL_ERR, "ERR : ", __VA_ARGS__)
#define LOG_WARN_(...)		LOG(0, LOG_LEVEL_WARN, "WARN: ", __VA_ARGS__)
#define LOG_INFO_(...)		LOG(0, LOG_LEVEL_INFO, "INFO: ", __VA_ARGS__)
#define LOG_DBG_(...)		LOG(0, LOG_LEVEL_DBG, "DBG : ", __VA_ARGS__)

#endif /* LOG_H_ */
