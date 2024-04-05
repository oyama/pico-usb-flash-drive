#ifndef _MSC_DISK_H_
#define _MSC_DISK_H_

#include <ctype.h>
#include <bsp/board.h>
#include <tusb.h>

#include "flash.h"

enum
{
  DISK_BLOCK_NUM  = 128,
  DISK_BLOCK_SIZE = 512,
};


#endif
