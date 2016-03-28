/* Set structure packing to 1 byte */

#if defined(__WATCOMC__)
 #pragma pack(__push,1);
#elif defined(__HIGHC__)
 #pragma push_align_members(1);
#elif defined __GNUC__
 #pragma pack(1)
#elif defined __C6X__
#elif defined __MWERKS__
#else
 #error Compiler not supported
#endif
