# AvlTree
Basically just my junkbox presently.
This started off as an experiment with AVL trees because I had in mind to use them in the process of generating records of
duplicated files in a user named directory. Over time sanity prevailed and I decided to use qsort() instead to sort the files
in the user selected dir so that I could remove as many as possible non-duplicated files from consideration because
calculation of __md5sum__ is such a time consumimg operation.

