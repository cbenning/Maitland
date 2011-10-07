/* lutil.h -- utilities

   This file is part of the UCL data compression library.

   Copyright (C) 1996-2003 Markus Franz Xaver Johannes Oberhumer
   All Rights Reserved.

   The UCL library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The UCL library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the UCL library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   <markus@oberhumer.com>
 */


#if defined(UCL_HAVE_CONFIG_H)
#  define ACC_CONFIG_NO_HEADER 1
#endif
#include "acc/acc.h"
#include "acc/acc_incd.h"
#include "acc/acc_ince.h"

#undef NDEBUG
#include <assert.h>

#if (ACC_CC_MSC && (_MSC_VER >= 1000 && _MSC_VER < 1200))
   /* avoid -W4 warnings in <windows.h> */
#  pragma warning(disable: 4201 4214 4514)
#endif

/* some systems have a xmalloc in their C library... */
#undef xmalloc
#undef xfree
#undef xread
#undef xwrite
#undef xputc
#undef xgetc
#undef xread32
#undef xwrite32


/*
vi:ts=4:et
*/

