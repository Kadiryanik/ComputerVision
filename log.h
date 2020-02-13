#ifndef LOG_H_
#define LOG_H_

/* Log Levels */
#define LOG_LEVEL_NONE         0 /* No log */
#define LOG_LEVEL_ERR          1 /* Errors */
#define LOG_LEVEL_WARN         2 /* Warnings */
#define LOG_LEVEL_INFO         3 /* Basic info */
#define LOG_LEVEL_DBG          4 /* Detailled debug */

#define LOG(level, ...) do {  \
                            if(level <= (LOG_LEVEL)) { \
                              printf(__VA_ARGS__); \
                            } \
                          } while (0)

#define LOG_PRINT(...)          LOG(LOG_LEVEL_NONE, __VA_ARGS__)
#define LOG_ERR(...)            LOG(LOG_LEVEL_ERR, __VA_ARGS__)
#define LOG_WARN(...)           LOG(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_INFO(...)           LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_DBG(...)            LOG(LOG_LEVEL_DBG, __VA_ARGS__)

#endif /* LOG_H_ */