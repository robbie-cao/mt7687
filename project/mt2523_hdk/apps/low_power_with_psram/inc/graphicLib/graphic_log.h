
#ifndef __GRAPHIC_LOG_H__
#define __GRAPHIC_LOG_H__

#include "syslog.h"

#define GRAPHICLOG(fmt, args...)       LOG_I(GRAPHIC_TAG, "[GRAPHIC]"fmt, ##args)

#endif //__GRAPHIC_LOG_H__
