/**
 * \file
 *        K-Means implementation
 *
 * \author
 *        Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "log.h"
#include "util.h"
#include "k-means.h"

#ifndef LOG_LEVEL_CONF_KMEANS
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_KMEANS */
#define LOG_LEVEL LOG_LEVEL_CONF_KMEANS
#endif /* LOG_LEVEL_CONF_KMEANS */
/*------------------------------------------------------------------------------*/
#define HISTOGRAM_LENGTH 256

/*------------------------------------------------------------------------------*/
typedef struct cluster {
    uint8_t c;	    /* centroid */
    uint8_t c_u;    /* centroid^ */
    float sum;
    float content_sum;
} cluster_t;

/*------------------------------------------------------------------------------*/
static cluster_t *clusters = NULL;
static uint8_t cluster_num = 0;

/*------------------------------------------------------------------------------*/
/*
 * _print_clusters
 */
static void _print_clusters()
{
    uint8_t i = 0;
    for (i = 0; i < cluster_num; i++) {
	LOG_DBG("clusters[%u].\n"
		"\tc=%u\n"
		"\tc_u=%u\n"
		"\tsum=%f\n"
		"\tcontent_sum=%f\n",
		i, clusters[i].c, clusters[i].c_u,
		clusters[i].sum, clusters[i].content_sum);
    }
}

/*------------------------------------------------------------------------------*/
/*
 * _initialize_clusters
 */
static void _initialize_clusters()
{
    uint8_t i = 0;
    for (i = 0; i < cluster_num; i++) {
	clusters[i].c_u = rand() % HISTOGRAM_LENGTH;
    }
    _print_clusters();
}

/*------------------------------------------------------------------------------*/
/*
 * _reset_clusters
 */
static void _reset_clusters()
{
    uint8_t i = 0;
    for (i = 0; i < cluster_num; i++) {
	clusters[i].sum = 0;
	clusters[i].content_sum = 0;
	clusters[i].c = clusters[i].c_u;
    }
}

/*------------------------------------------------------------------------------*/
/*
 * _add_point_to_cluster adds point into cluster which has minimum distance.
 */
static void _add_point_to_cluster(int index, int histogram_val)
{
    uint8_t i = 0, min = 0, min_index = 0;

    min = abs(clusters[0].c - index);
    for (i = 1; i < cluster_num; i++) {
	uint8_t current_distance = abs(clusters[i].c - index);
	if (current_distance < min) {
	    min = current_distance;
	    min_index = i;
	}
    }
    clusters[min_index].sum += index * histogram_val;
    clusters[min_index].content_sum += histogram_val;
}

/*------------------------------------------------------------------------------*/
/*
 * _calc_new_centroids calculates new centroids and stores in c_u.
 */
static void _calc_new_centroids()
{
    uint8_t i = 0;

    for (i = 0; i < cluster_num; i++) {
	if (clusters[i].content_sum == 0) clusters[i].content_sum = 1;
	clusters[i].c_u = clusters[i].sum / clusters[i].content_sum;
    }
}

/*------------------------------------------------------------------------------*/
/*
 * _check_centroids checks clusters are already orginized or not.
 */
static uint8_t _check_centroids()
{
    uint8_t ret = 0, i = 0;
    for (i = 0; i < cluster_num; i++) {
	util_fit((fabs(clusters[i].c - clusters[i].c_u) > 2));
    }

    /* For debugging move this call into fail case */
    _print_clusters();
    goto success;

fail:
    ret = 1;

success:
    return ret;
}

/*------------------------------------------------------------------------------*/
/* TODO: seperate this function for (n != 2) cases.
 * kmeans_get_thold return threshold value for converting image into binary.
 */
int kmeans_get_thold(uint8_t n, image_t image)
{
    int ret = 0;
    uint32_t *histogram = NULL, i = 0;

    util_fit(((histogram = (uint32_t *)calloc(1, HISTOGRAM_LENGTH * sizeof(uint32_t))) == NULL));

    util_fit(((clusters = (cluster_t *)calloc(n, sizeof(cluster_t))) == NULL));
    cluster_num = n;

    for (i = 0; i < image.size; i++) histogram[image.buf[i]]++;

    util_fite((plot_histogram(histogram) != 0),
	    LOG_ERR("Threshold plotting failed!\n"));

    _initialize_clusters();
    do {
	/* loop reset, check function for more detail */
	_reset_clusters();

	/* new clustering */
	for (i = 0; i < HISTOGRAM_LENGTH; i++) _add_point_to_cluster(i, histogram[i]);

	/* calculate new cluster centroid */
	_calc_new_centroids();
    } while (_check_centroids());

    ret = (clusters[0].c + clusters[1].c) / 2;
    LOG_DBG("c1: %u, c2: %u -> threshold = %d\n", clusters[0].c, clusters[1].c, ret);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree(histogram);
    sfree(clusters);
    return ret;
}

