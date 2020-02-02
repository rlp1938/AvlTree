#include <stdio.h>
#include <stdlib.h>
#include "avltree.h"

int main() {
  avl_tree *t = new_avl_tree();

  avl_node *a, *b, *c, *d, *e, *f, *g, *h, *i, *j, *k, *l, *m, *n;

  a = new_avl_node(10);
  b = new_avl_node(20);
  c = new_avl_node(30);
  d = new_avl_node(100);
  e = new_avl_node(90);
  f = new_avl_node(40);
  g = new_avl_node(50);
  h = new_avl_node(60);
  i = new_avl_node(70);
  j = new_avl_node(80);
  k = new_avl_node(150);
  l = new_avl_node(110);
  m = new_avl_node(120);
  n = new_avl_node(100);
  insert(t, a);
  insert(t, b);
  insert(t, c);
  insert(t, d);
  insert(t, e);
  insert(t, f);
  insert(t, g);
  insert(t, h);
  insert(t, i);
  insert(t, j);
  insert(t, k);
  insert(t, l);
  insert(t, m);
  insert(t, n);

//  avl_delete(t, a);
//  avl_delete(t, m);

  inorder(t, t->root);

  return 0;
}
