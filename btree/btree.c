/* radare - LGPL - Copyright 2009-2018 - pancake */

#include <btree.h>

R_API void btree_init(struct btree_node **T) {
	*T = NULL;
}

R_API struct btree_node *btree_remove(struct btree_node *p, BTREE_DEL(del)) {
	struct btree_node *rp = NULL, *f;
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

R_API void *btree_search(struct btree_node *root, void *x, BTREE_CMP(cmp), int parent) {
	struct btree_node *p = NULL;

	if (root) {
		if (cmp (x, root->data) < 0) {
			p = btree_search (root->left, x, cmp, parent);
		} else if (cmp (x, root->data) > 0) {
			p = btree_search (root->right, x, cmp, parent);
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

R_API void btree_traverse(struct btree_node *root, int reverse, void *context, BTREE_TRV(trv)) {
	if (root) {
		if (reverse) {
			btree_traverse (root->right, reverse, context, trv);
			trv (root->data, context);
			btree_traverse (root->left, reverse, context, trv);
		} else {
			btree_traverse (root->left, reverse, context, trv);
			trv (root->data, context);
			btree_traverse (root->right, reverse, context, trv);
		}
	}
}

R_API bool btree_del(struct btree_node *proot, void *x, BTREE_CMP(cmp), BTREE_DEL(del)) {
	struct btree_node *p = btree_search (proot, x, cmp, 1);
	if (p) {
		// p->right =
		btree_remove (p->left, del);
		p->left = NULL;
		return true;
	}
	return false;
}

R_API void *btree_get(struct btree_node *proot, void *x, BTREE_CMP(cmp)) {
	struct btree_node *p = btree_search (proot, x, cmp, 0);
	if (p) {
		p->hits++;
		return p->data;
	}
	return NULL;
}

R_API void btree_cleartree(struct btree_node *proot, BTREE_DEL(del)) {
	if (proot) {
		btree_cleartree (proot->left, del);
		btree_cleartree (proot->right, del);
		if (del) {
			del (proot->data);
		}
		free (proot);
	}
}

R_API void btree_insert(struct btree_node **T, struct btree_node *p, BTREE_CMP(cmp)) {
	int ret = cmp (p->data, (*T)->data);
	if (ret < 0) {
		if ((*T)->left) {
			btree_insert (&(*T)->left, p, cmp);
		} else {
			(*T)->left = p;
		}
	} else if (ret > 0) {
		if ((*T)->right) {
			btree_insert (&(*T)->right, p, cmp);
		} else {
			(*T)->right = p;
		}
	}
}

R_API void btree_add(struct btree_node **T, void *e, BTREE_CMP(cmp)) {
	struct btree_node *p = (struct btree_node *)malloc (sizeof (struct btree_node));
	p->data = e;
	p->hits = 0;
	p->left = p->right = NULL;
	if (!*T) {
		*T = p;
	} else {
		btree_insert (T, p, cmp);
	}
}

/* unused */
R_API int btree_empty(struct btree_node **T) {
	return !((T && (*T) && ((*T)->right || (*T)->left)));
}

R_API struct btree_node *btree_hittest(struct btree_node *root, struct btree_node *hn) {
	struct btree_node *p = root;
	if (root) {
		struct btree_node *ml = btree_hittest (root->left, root);
		struct btree_node *mr = btree_hittest (root->right, root);
		if (ml && ml->hits > p->hits) {
			p = ml;
		}
		if (mr && mr->hits > p->hits) {
			p = mr;
		}
	}
	return p;
}

R_API int btree_optimize(struct btree_node **T, BTREE_CMP(cmp)) {
	struct btree_node *node, *NT = NULL;
	do {
		node = btree_hittest (*T, NULL);
		if (node) {
			btree_add (&NT, node->data, cmp);
			btree_del (*T, node->data, cmp, NULL);
		}
	} while (node);
	*T = NT; /* replace one tree with the other */
	return 0;
}
