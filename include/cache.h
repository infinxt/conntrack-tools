#ifndef _CACHE_H_
#define _CACHE_H_

#include <stdint.h>
#include <stddef.h>

/* cache features */
enum {
	NO_FEATURES = 0,

	TIMER_FEATURE = 0,
	TIMER = (1 << TIMER_FEATURE),

	LIFETIME_FEATURE = 2,
	LIFETIME = (1 << LIFETIME_FEATURE),

	WRITE_THROUGH_FEATURE = 3,
	WRITE_THROUGH = (1 << WRITE_THROUGH_FEATURE),

	__CACHE_MAX_FEATURE
};
#define CACHE_MAX_FEATURE __CACHE_MAX_FEATURE

struct cache;
struct us_conntrack;

struct cache_feature {
	size_t size;
	void (*add)(struct us_conntrack *u, void *data);
	void (*update)(struct us_conntrack *u, void *data);
	void (*destroy)(struct us_conntrack *u, void *data);
	int  (*dump)(struct us_conntrack *u, void *data, char *buf, int type);
};

extern struct cache_feature lifetime_feature;
extern struct cache_feature timer_feature;
extern struct cache_feature writethrough_feature;

#define CACHE_MAX_NAMELEN 32

struct cache {
	char name[CACHE_MAX_NAMELEN];
	struct hashtable *h;

	unsigned int num_features;
	struct cache_feature **features;
	unsigned int feature_type[CACHE_MAX_FEATURE];
	unsigned int *feature_offset;
	struct cache_extra *extra;
	unsigned int extra_offset;

        /* statistics */
	struct {
		uint32_t	active;
	
		uint32_t	add_ok;
		uint32_t	del_ok;
		uint32_t	upd_ok;
		
		uint32_t	add_fail;
		uint32_t	del_fail;
		uint32_t	upd_fail;

		uint32_t	add_fail_enomem;
		uint32_t	add_fail_enospc;
		uint32_t	del_fail_enoent;
		uint32_t	upd_fail_enoent;

		uint32_t	commit_ok;
		uint32_t	commit_exist;
		uint32_t	commit_fail;

		uint32_t	flush;
	} stats;
};

struct cache_extra {
	unsigned int size;

	void (*add)(struct us_conntrack *u, void *data);
	void (*update)(struct us_conntrack *u, void *data);
	void (*destroy)(struct us_conntrack *u, void *data);
};

struct nf_conntrack;

struct cache *cache_create(const char *name, unsigned int features, struct cache_extra *extra);
void cache_destroy(struct cache *e);

struct us_conntrack *cache_add(struct cache *c, struct nf_conntrack *ct);
struct us_conntrack *cache_update(struct cache *c, struct nf_conntrack *ct);
struct us_conntrack *cache_update_force(struct cache *c, struct nf_conntrack *ct);
int cache_del(struct cache *c, struct nf_conntrack *ct);
int __cache_del_timer(struct cache *c, struct us_conntrack *u, int timeout);
struct us_conntrack *cache_find(struct cache *c, struct nf_conntrack *ct);
int cache_test(struct cache *c, struct nf_conntrack *ct);
void cache_stats(const struct cache *c, int fd);
void cache_stats_extended(const struct cache *c, int fd);
struct us_conntrack *cache_get_conntrack(struct cache *, void *);
void *cache_get_extra(struct cache *, void *);
void cache_iterate(struct cache *c, void *data, int (*iterate)(void *data1, void *data2));

/* iterators */
void cache_dump(struct cache *c, int fd, int type);
void cache_commit(struct cache *c);
void cache_flush(struct cache *c);
void cache_bulk(struct cache *c);
void cache_reset_timers(struct cache *c);

#endif
