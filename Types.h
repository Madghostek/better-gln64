#ifndef TYPES_H
#define TYPES_H

using u8 = unsigned char; /* unsigned  8-bit */
using u16 = unsigned short; /* unsigned 16-bit */
using u32 = unsigned long; /* unsigned 32-bit */
using u64 = unsigned __int64; /* unsigned 64-bit */

using s8 = signed char; /* signed  8-bit */
using s16 = short; /* signed 16-bit */
using s32 = long; /* signed 32-bit */
using s64 = __int64; /* signed 64-bit */

using vu8 = volatile unsigned char; /* unsigned  8-bit */
using vu16 = volatile unsigned short; /* unsigned 16-bit */
using vu32 = volatile unsigned long; /* unsigned 32-bit */
using vu64 = volatile unsigned __int64; /* unsigned 64-bit */

using vs8 = volatile signed char; /* signed  8-bit */
using vs16 = volatile short; /* signed 16-bit */
using vs32 = volatile long; /* signed 32-bit */
using vs64 = volatile __int64; /* signed 64-bit */

using f32 = float; /* single prec floating point */
using f64 = double; /* double prec floating point */

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#define NULL    0
#endif

#endif  TYPES_H
