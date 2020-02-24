# AvlTree
Basically just my junkbox presently.
This started off as an experiment with AVL trees because I had in mind to use them in the process of generating records of
duplicated files in a user named directory. Over time sanity prevailed and I decided to use _qsort()_ instead to sort the files
in the user selected dir so that I could remove as many as possible non-duplicated files from consideration because
calculation of __md5sum__ is such a time consumimg operation.

At the time of creation of this, the program _newdups.c_ functions rather well in generating a list of duplictated files
to deal with. The interactive program _procdups.c_ is however only about half done.

My intention is to do a major rewrite of _duplicates.c_ by merging in the knowledge gained here. Options and man page is to
be added to suit. The target program resides in the __Duplicates__ repository. When this process is complete this repo will
be deleted.
