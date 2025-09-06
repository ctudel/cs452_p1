# Project 1 - Simple Linked List

- Name: Chris Tudela
- Email: christudela@u.boisestate.edu
- Class: 452-002

## Known Bugs or Issues

No known bugs, but it is possible that I missed some edge cases I didn't think to test for.

## Experience

This project was a great refresher for pointers and the dynamics of managing memory. Some of my breakthroughs involved re-learning what exactly pointers are (references or copies?) and managing the memory (addresses). This definitely introduced some struggles, especially with dynamic behavior like the list_destroy function. Cleaning all the nodes and the list(s) was challenging to do without memory leaks. It was mostly because I decided to manage a tail, which slightly paid off if the list ever gets large, but will hardly make a difference and should've probably excluded that functionality. Not to say that it works any differently, the implementation still respects the sentinel node -- it just introduced, probably, unnecessary struggle. 

Not necessarily a struggle, but I've been working in Typescript for the past year and it was interesting to see all the cross-over when typing. I noticed that Typescript inherited a lot of principles of C, where interfaces are basically structs, and many more elements. In other words, there were new discoveries for me when dealing with type checks and creations in C. I thought it was a great intro project and is definitely one that got me back up to speed -- hopefully enough to be ready for the next projects.

