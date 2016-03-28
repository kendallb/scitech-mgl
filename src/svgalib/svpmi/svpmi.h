
typdef struct
  {
    int width;
    int height;
    int bitsperpixel;
    int bytesperscanline;
    int windowgranularity;
    void *setmode ();
    void *setwindow (int);
  }
svpmi_modeentry;


static void
wait (int x)
{
  int i;
  for (i = 0; i < 10; i++);
}

static unsigned char r0, r1, r2, r3, r4, r5, r6, r7;
static unsigned char r8, r9, r10, r11, r12, r13, r14, r15;
static unsigned char r16, r17, r18, r19, r20, r21, r22, r23;
static unsigned char r24, r25, r26, r27, r28, r29, r30, r31;

#define boutb(n, p1, p2) __boutb(0, n, p1 p2)
#define __boutp(i, n, p1, p2) \
	#if n != 0 \
	outb(p1, i); outb(p2, r##i); \
	boutb((i + 1), (n - 1), p1, p2); \
	#endif

#define inb(r, p) port_in(p)

#define and(r, v) r &= v;
#define or(r, v) r |= v;
#define xor(r, v) r ^= v;
#define shr(r, v) r >>= v;
#define shl(r, v) r <<= v;
