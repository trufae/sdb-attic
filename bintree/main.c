#include "bintree.h"

struct mydata {
	unsigned long long addr;
	char *str;
};

static int shownode(char *str, struct mydata *m) {
	if (m) {
		printf ("==> %s: %s, %" PFMT64d "\n", str, m->str, m->addr);
	} else {
		printf ("==> not found\n");
	}
	return 0;
}

static int mycmp(const void *a, const void *b) {
	struct mydata *ma = (struct mydata *)a;
	struct mydata *mb = (struct mydata *)b;
	if (!a || !b)
		return 0;
	return (int)(ma->addr - mb->addr);
}

int main () {
	struct bintree_node *n, *bt = NULL;
	//bintree_init(&bt);

	struct mydata foo = { 10, "hello" };
	struct mydata bar = { 20, "world" };

	printf ("EMPTY TREE: %d\n", bintree_empty (&bt));
	bintree_add (&bt, &foo, mycmp);
	bintree_add (&bt, &bar, mycmp);
	printf ("EMPTY TREE: %d\n", bintree_empty (&bt));

	printf ("==== go search ====\n");
	/* find existent data */
	struct mydata *p = bintree_get (bt, &bar, mycmp);
	shownode ("result for 20: ", p);

	printf ("==== go search ====\n");
	/* find unexistent data */
	struct mydata nop = { 15, NULL };
	p = bintree_get (bt, &nop, mycmp);
	shownode ("result for 15: ", p);

	printf ("==== go get hittest ====\n");
	n = bintree_hittest (bt, NULL);
	shownode ("hitest is: ", p);

	printf ("==== go remove 20 ====\n");
	if (bintree_del (bt, &bar, mycmp, NULL))
		printf ("node found and removed\n");
	else printf ("oops\n");

	printf ("==== go search ====\n");
	/* find existent data */
	p = bintree_get (bt, &bar, mycmp);
	shownode ("result for 20: ", p);

	printf ("==== go search ====\n");
	/* find existent data */
	p = bintree_get (bt, &foo, mycmp);
	shownode ("result for 10: ", p);

	bintree_cleartree (bt, NULL);
	return 0;
}
