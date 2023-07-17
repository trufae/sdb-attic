#ifndef R2_BTREE_H
#define R2_BTREE_H

#include "r_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bintree_node {
	void *data;
	int hits; // profiling
	struct bintree_node *left;
	struct bintree_node *right;
};

#define BTREE_CMP(x) int (* x )(const void *, const void *)
#define BTREE_DEL(x) int (* x )(void *)
#define BTREE_TRV(x) void (* x )(const void *, const void *)

#ifdef R_API
R_API int bintree_empty(struct bintree_node **T);
R_API struct bintree_node *bintree_hittest(struct bintree_node *root, struct bintree_node *hn);
R_API void bintree_init(struct bintree_node **T);
R_API struct bintree_node *bintree_remove(struct bintree_node *p, BTREE_DEL(del));
R_API void *bintree_search(struct bintree_node *proot, void *x, BTREE_CMP(cmp), int parent);
R_API void bintree_traverse(struct bintree_node *proot, int reverse, void *context, BTREE_TRV(trv));
R_API bool bintree_del(struct bintree_node *proot, void *x, BTREE_CMP(cmp), BTREE_DEL(del));
R_API void *bintree_get(struct bintree_node *proot, void *x, BTREE_CMP(cmp));
R_API void bintree_insert(struct bintree_node **T, struct bintree_node *p, BTREE_CMP(cmp));
R_API void bintree_add(struct bintree_node **T, void *e, BTREE_CMP(cmp));
R_API void bintree_cleartree(struct bintree_node *proot, BTREE_DEL(del));
#endif

#ifdef __cplusplus
}
#endif

#endif
