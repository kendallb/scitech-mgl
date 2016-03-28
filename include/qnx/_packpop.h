/* Restore structure packing to previous value. */

#if defined(__WATCOMC__)
 #pragma pack(__pop);
#elif defined(__HIGHC__)
 #pragma pop_align_members();
#elif defined __GNUC__
 #pragma pack()
#elif defined __C6X__
#elif defined __MWERKS__
#else
 #error Compiler not supported
#endif
