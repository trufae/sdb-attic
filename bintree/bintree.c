/* radare - LGPL - Copyright 2009-2018 - pancake */

#include "bintree.h"

R_API void bintree_init(struct bintree_node **T) {
	*T = NULL;
}

R_API struct bintree_node *bintree_remove(struct bintree_node *p, BTREE_DEL(del)) {
	struct bintree_node *rp = NULL, *f;
	if (!p) {
		return p;
	}
	if (p->right) {
		if (p->left) {
			f = p;
			rp = p->right;
			while (rp->left) {
				f = rp;
				rp = rp->left;
			}
			if (f != p) {
				f->left = rp->right;
				rp->right = p->right;
				rp->left = p->left;
			} else {
				rp->left = p->left;
			}
		} else {
			rp = p->right;
		}
	} else {
		rp = p->left;
	}
	if (del) {
		del (p->data);
	}
	free (p);
	return (rp);
}

R_API void *bintree_search(struct bintree_node *root, void *x, BTREE_CMP(cmp), int parent) {
	struct bintree_node *p = NULL;

	if (root) {
		if (cmp (x, root->data) < 0) {
			p = bintree_search (root->left, x, cmp, parent);
		} else if (cmp (x, root->data) > 0) {
			p = bintree_search (root->right, x, cmp, parent);
		} else {
			p = root;
		}
	}
	/* node found */
	if (p) {
		if (parent) {
			return root;
		}
		return p;
	}
	return NULL;
}

R_API void bintree_traverse(struct bintree_node *root, int reverse, void *context, BTREE_TRV(trv)) {
	if (root) {
		if (reverse) {
			bintree_traverse (root->right, reverse, context, trv);
			trv (root->data, context);
			bintree_traverse (root->left, reverse, context, trv);
		} else {
			bintree_traverse (root->left, reverse, context, trv);
			trv (root->data, context);
			bintree_traverse (root->right, reverse, context, trv);
		}
	}
}

R_API bool bintree_del(struct bintree_node *proot, void *x, BTREE_CMP(cmp), BTREE_DEL(del)) {
	struct bintree_node *p = bintree_search (proot, x, cmp, 1);
	if (p) {
		// p->right =
		bintree_remove (p->left, del);
		p->left = NULL;
		return true;
	}
	return false;
}

R_API void *bintree_get(struct bintree_node *proot, void *x, BTREE_CMP(cmp)) {
	struct bintree_node *p = bintree_search (proot, x, cmp, 0);
	if (p) {
		p->hits++;
		return p->data;
	}
	return NULL;
}

R_API void bintree_cleartree(struct bintree_node *proot, BTREE_DEL(del)) {
	if (proot) {
		bintree_cleartree (proot->left, del);
		bintree_cleartree (proot->right, del);
		if (del) {
			del (proot->data);
		}
		free (proot);
	}
}

R_API void bintree_insert(struct bintree_node **T, struct bintree_node *p, BTREE_CMP(cmp)) {
	int ret = cmp (p->data, (*T)->data);
	if (ret < 0) {
		if ((*T)->left) {
			bintree_insert (&(*T)->left, p, cmp);
		} else {
			(*T)->left = p;
		}
	} else if (ret > 0) {
		if ((*T)->right) {
			bintree_insert (&(*T)->right, p, cmp);
		} else {
			(*T)->right = p;
		}
	}
}

R_API void bintree_add(struct bintree_node **T, void *e, BTREE_CMP(cmp)) {
	struct bintree_node *p = (struct bintree_node *)malloc (sizeof (struct bintree_node));
	p->data = e;
	p->hits = 0;
	p->left = p->right = NULL;
	if (!*T) {
		*T = p;
	} else {
		bintree_insert (T, p, cmp);
	}
}

/* unused */
R_API int bintree_empty(struct bintree_node **T) {
	return !((T && (*T) && ((*T)->right || (*T)->left)));
}

R_API struct bintree_node *bintree_hittest(struct bintree_node *root, struct bintree_node *hn) {
	struct bintree_node *p = root;
	if (root) {
		struct bintree_node *ml = bintree_hittest (root->left, root);
		struct bintree_node *mr = bintree_hittest (root->right, root);
		if (ml && ml->hits > p->hits) {
			p = ml;
		}
		if (mr && mr->hits > p->hits) {
			p = mr;
		}
	}
	return p;
}

R_API int bintree_optimize(struct bintree_node **T, BTREE_CMP(cmp)) {
	struct bintree_node *node, *NT = NULL;
	do {
		node = bintree_hittest (*T, NULL);
		if (node) {
			bintree_add (&NT, node->data, cmp);
			bintree_del (*T, node->data, cmp, NULL);
		}
	} while (node);
	*T = NT; /* replace one tree with the other */
	return 0;
}
