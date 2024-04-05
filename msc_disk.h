#ifndef _MSC_DISK_H
#define _MSC_DISK_H

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
