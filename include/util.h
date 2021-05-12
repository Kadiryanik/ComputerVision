#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

#define sfree(_p) do {	    \
	if ((_p)) {	    \
	    free((_p));	    \
	    (_p) = NULL;    \
	}		    \
    } while (0)

/* Goto fail in error with expression */
#define util_fiee(f,e) do { \
	if ((f) != 0) {	    \
	    e;		    \
	    goto fail;	    \
	}		    \
    } while (0)

#define util_fie(f) util_fiee(f,{ do { } while (0); })

/* Goto fail/success in condition true with expression */
#define util_xite(c,l,e) do {	\
	if (c) {		\
	    e;			\
	    goto l;		\
	}			\
    } while (0)

/* fail in true w/o expression */
#define util_fit(c) util_xite(c,fail,{ do { } while (0); })
#define util_fite(c,e) util_xite(c,fail,e)
/* success in true w/o expression */
#define util_sit(c) util_xite(c,success,{ do { } while (0); })
#define util_site(c,e) util_xite(c,success,e)

int plot_histogram(const uint32_t* const histogram);

#endif /* UTIL_H_ */
