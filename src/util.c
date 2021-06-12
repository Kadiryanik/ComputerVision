/**
 * \file
 *	Utilities
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "util.h"

#ifndef LOG_LEVEL_CONF_UTIL
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_UTIL */
#define LOG_LEVEL LOG_LEVEL_CONF_UTIL
#endif /* LOG_LEVEL_CONF_UTIL */

/*------------------------------------------------------------------------------*/
#define HISTOGRAM_LENGTH 256
#define HISTOGRAM_FILE_NAME "hist.txt"
#define SYS_CALL_PLOT_HISTOGRAM "python helper/plot.py "HISTOGRAM_FILE_NAME" &"

/*------------------------------------------------------------------------------*/
extern int plot_with_python; /* defined in test.c */

/*------------------------------------------------------------------------------*/
int plot_histogram(const uint32_t* const histogram)
{
    int ret = 0;
    FILE *file = NULL;

    /* Check plot is needed */
    util_sit((plot_with_python == 0));

    util_fite(((file = fopen(HISTOGRAM_FILE_NAME, "w")) == NULL),
	    LOG_ERR("File open failed\n"));

    util_fite((fwrite(histogram, HISTOGRAM_LENGTH, sizeof(int), file) < 1),
	    LOG_ERR("File writing data failed!\n"));

    util_fite((system(SYS_CALL_PLOT_HISTOGRAM) != 0),
	    LOG_ERR("CMD: %s failed!\n", SYS_CALL_PLOT_HISTOGRAM));

    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    if (file) fclose(file);
    return ret;
}

/*------------------------------------------------------------------------------*/
str_node_t* util_sl_insert(str_node_t **head, char *str)
{
    str_node_t *ptr = *head;

    if (*head == NULL) {
	util_fite(((*head = (str_node_t *)calloc(1, sizeof(str_node_t))) == NULL),
		LOG_ERR("String list first allocation failed\n"));
	ptr = *head;
    } else {
	while (ptr->next != NULL) {
	    ptr = ptr->next;
	}
	util_fite(((ptr->next = (str_node_t *)calloc(1, sizeof(str_node_t))) == NULL),
		LOG_ERR("String list new allocation failed\n"));
	ptr = ptr->next;
    }
    util_fite(((ptr->str = strdup(str)) == NULL),
	    LOG_ERR("Duplicating class name failed\n"));
    LOG_DBG("String %p - '%s' added\n", ptr, str);

    goto success;

fail:
    if (ptr) sfree(ptr->str);
    if (ptr == *head) {
	sfree(*head);
	ptr = NULL;
    } else {
	sfree(ptr);
    }

success:
    return ptr;
}

/*------------------------------------------------------------------------------*/
void util_sl_free(str_node_t **head)
{
    str_node_t *ptr = *head, *current = NULL;

    while (ptr != NULL) {
	sfree(ptr->str);

	current = ptr;
	ptr = ptr->next;
	sfree(current);
    }
    *head = NULL;
}

