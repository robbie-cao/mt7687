
/*****************************************************
  Usage:
   1. function: (a or b)
      a. ATTR_TEXT_IN_TCM int func(int par);
      b. ATTR_TEXT_IN_TCM int func(int par)
        {
        }
   2. RO data:
      a. ATTR_RODATA_IN_TCM const int b = 8;
   3. RW data:
      a. ATTR_RWDATA_IN_TCM int b = 8;
   4. ZI data:
      a. ATTR_ZIDATA_IN_TCM int b;
      
  Note: must put these attributes at HEAD of declaration.
*****************************************************/

#ifndef MEMORY_ATTRIBUTE_H_
#define MEMORY_ATTRIBUTE_H_

#ifndef __ICCARM__

#define ATTR_TEXT_IN_TCM              __attribute__ ((__section__(".tcm_code")))
#define ATTR_RODATA_IN_TCM            __attribute__ ((__section__(".tcm_rodata")))
#define ATTR_RWDATA_IN_TCM            __attribute__ ((__section__(".tcm_rwdata")))
#define ATTR_ZIDATA_IN_TCM            __attribute__ ((__section__(".tcm_zidata")))


#define ATTR_TEXT_IN_RAM              __attribute__ ((__section__(".ram_code")))
#define ATTR_RODATA_IN_RAM            __attribute__ ((__section__(".ram_rodata")))
#define ATTR_RWDATA_IN_NONCACHED_RAM  __attribute__ ((__section__(".noncached_rwdata")))
#define ATTR_ZIDATA_IN_NONCACHED_RAM  __attribute__ ((__section__(".noncached_zidata")))

#define ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  __attribute__ ((__section__(".noncached_rwdata"),__aligned__(4)))
#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  __attribute__ ((__section__(".noncached_zidata"),__aligned__(4)))

#else

#define ATTR_TEXT_IN_TCM              _Pragma("location=\".tcm_code\"")
#define ATTR_RODATA_IN_TCM            _Pragma("location=\".tcm_rodata\"")
#define ATTR_RWDATA_IN_TCM            _Pragma("location=\".tcm_rwdata\"")
#define ATTR_ZIDATA_IN_TCM            _Pragma("location=\".tcm_zidata\"")


#define ATTR_TEXT_IN_RAM              _Pragma("location=\".ram_code\"")
#define ATTR_RODATA_IN_RAM            _Pragma("location=\".ram_rodata\"")
#define ATTR_RWDATA_IN_NONCACHED_RAM  _Pragma("location=\".noncached_rwdata\"")
#define ATTR_ZIDATA_IN_NONCACHED_RAM  _Pragma("location=\".noncached_zidata\"")

#define ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  _Pragma("location=\".noncached_rwdata\"") \
                                                  _Pragma("data_alignment=4")
#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  _Pragma("location=\".noncached_zidata\"") \
                                                  _Pragma("data_alignment=4")

#endif

#endif

