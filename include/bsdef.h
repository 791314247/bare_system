/*
 * Copyright (c) 2006-2018, bare-system Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 *
 */

#ifndef __BS_DEF_H__
#define __BS_DEF_H__

/* include bsconfig header to import configuration */
#include <bsconfig.h>


/**
 * @addtogroup BasicDef
 */

/**@{*/

/* bare-system version information */
#define BS_VERSION                      1L              /**< major version number */
#define BS_SUBVERSION                   0L              /**< minor version number */
#define BS_REVISION                     1L              /**< revise version number */

/* bare-system version */
#define BSTHREAD_VERSION                ((BS_VERSION * 10000) + \
                                         (BS_SUBVERSION * 100) + BS_REVISION)

/* bare-system basic data type definitions */
#ifndef BS_USING_ARCH_DATA_TYPE
    typedef signed   char                   bs_int8_t;      /**<  8bit integer type */
    typedef signed   short                  bs_int16_t;     /**< 16bit integer type */
    typedef signed   int                    bs_int32_t;     /**< 32bit integer type */
    typedef unsigned char                   bs_uint8_t;     /**<  8bit unsigned integer type */
    typedef unsigned short                  bs_uint16_t;    /**< 16bit unsigned integer type */
    typedef unsigned int                    bs_uint32_t;    /**< 32bit unsigned integer type */

    #ifdef ARCH_CPU_64BIT
        typedef signed long                     bs_int64_t;     /**< 64bit integer type */
        typedef unsigned long                   bs_uint64_t;    /**< 64bit unsigned integer type */
    #else
        typedef signed long long                bs_int64_t;     /**< 64bit integer type */
        typedef unsigned long long              bs_uint64_t;    /**< 64bit unsigned integer type */
    #endif
#endif

typedef int                             bs_bool_t;      /**< boolean type */
typedef long                            bs_base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   bs_ubase_t;     /**< Nbit unsigned CPU related data type */

typedef bs_base_t                       bs_err_t;       /**< Type for error number */
typedef bs_uint32_t                     bs_time_t;      /**< Type for time stamp */
typedef bs_uint32_t                     bs_tick_t;      /**< Type for tick count */
typedef bs_base_t                       bs_flag_t;      /**< Type for flags */
typedef bs_ubase_t                      bs_size_t;      /**< Type for size number */
typedef bs_ubase_t                      bs_dev_t;       /**< Type for device */
typedef bs_base_t                       bs_off_t;       /**< Type for offset */

/* boolean type definitions */
#define BS_TRUE                         1               /**< boolean true  */
#define BS_FALSE                        0               /**< boolean fails */

/**@}*/

/* maximum value of base type */
#define BS_UINT8_MAX                    0xff            /**< Maxium number of UINT8 */
#define BS_UINT16_MAX                   0xffff          /**< Maxium number of UINT16 */
#define BS_UINT32_MAX                   0xffffffff      /**< Maxium number of UINT32 */
#define BS_TICK_MAX                     BS_UINT32_MAX   /**< Maxium number of tick */

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #define __CLANG_ARM
#endif

/* Compiler Related Definitions */
#if defined(__CC_ARM) || defined(__CLANG_ARM)           /* ARM Compiler */
    #include <stdarg.h>
    #define SECTION(x)                  __attribute__((section(x)))
    #define BS_UNUSED                   __attribute__((unused))
    #define BS_USED                     __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))

    #define BS_WEAK                     __attribute__((weak))
    #define bs_inline                   static __inline

#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #include <stdarg.h>
    #define SECTION(x)                  @ x
    #define BS_UNUSED
    #define BS_USED                     __root
    #define PRAGMA(x)                   _Pragma(#x)
    #define ALIGN(n)                    PRAGMA(data_alignment=n)
    #define BS_WEAK                     __weak
    #define bs_inline                   static inline

#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #ifdef BS_USING_NEWLIB
        #include <stdarg.h>
    #else
        /* the version of GNU GCC must be greater than 4.x */
        typedef __builtin_va_list       __gnuc_va_list;
        typedef __gnuc_va_list          va_list;
        #define va_start(v,l)           __builtin_va_start(v,l)
        #define va_end(v)               __builtin_va_end(v)
        #define va_arg(v,l)             __builtin_va_arg(v,l)
    #endif

    #define SECTION(x)                  __attribute__((section(x)))
    #define BS_UNUSED                   __attribute__((unused))
    #define BS_USED                     __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))
    #define BS_WEAK                     __attribute__((weak))
    #define bs_inline                   static __inline
#else
    #error not supported tool chain
#endif

/* initialization export */
typedef int (*init_fn_t)(void);
#define INIT_EXPORT(fn, level)   \
            BS_USED const init_fn_t __bs_init_##fn SECTION(".rti_fn."level) = fn

/* board init routines will be called in board_init() function */
#define INIT_BOARD_EXPORT(fn)           INIT_EXPORT(fn, "1")

/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initilization) */
#define INIT_PREV_EXPORT(fn)            INIT_EXPORT(fn, "2")
/* device initialization */
#define INIT_DEVICE_EXPORT(fn)          INIT_EXPORT(fn, "3")
/* components initialization (dfs, lwip, ...) */
#define INIT_COMPONENT_EXPORT(fn)       INIT_EXPORT(fn, "4")
/* environment initialization (mount disk, ...) */
#define INIT_ENV_EXPORT(fn)             INIT_EXPORT(fn, "5")
/* appliation initialization (rtgui application etc ...) */
#define INIT_APP_EXPORT(fn)             INIT_EXPORT(fn, "6")


/**
 * @addtogroup Error
 */

/**@{*/

/* bare-system error code definitions */
#define BS_EOK                          0               /**< There is no error */
#define BS_ERROR                        1               /**< A generic error happens */
#define BS_ETIMEOUT                     2               /**< Timed out */
#define BS_EFULL                        3               /**< The resource is full */
#define BS_EEMPTY                       4               /**< The resource is empty */
#define BS_ENOMEM                       5               /**< No memory */
#define BS_ENOSYS                       6               /**< No system */
#define BS_EBUSY                        7               /**< Busy */
#define BS_EIO                          8               /**< IO error */
#define BS_EINTR                        9               /**< Interrupted system call */
#define BS_EINVAL                       10              /**< Invalid argument */

/**@}*/

/**
 * @ingroup BasicDef
 *
 * @def BS_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. BS_ALIGN(13, 4)
 * would return 16.
 */
#define BS_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def BS_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. BS_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define BS_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def BS_NULL
 * Similar as the \c NULL in C library.
 */
#define BS_NULL                         (0)



/**
 * Double List structure
 */
struct bs_list_node
{
    struct bs_list_node *next;                          /**< point to next node. */
    struct bs_list_node *prev;                          /**< point to prev node. */
};
typedef struct bs_list_node bs_list_t;                  /**< Type for lists. */

/**
 * Single List structure
 */
struct bs_slist_node
{
    struct bs_slist_node *next;                         /**< point to next node. */
};
typedef struct bs_slist_node bs_slist_t;                /**< Type for single list. */


/**
 * Base structure of Kernel object
 */
struct bs_object
{
    char       name[BS_NAME_MAX];                       /**< name of kernel object */
    bs_uint8_t type;                                    /**< type of kernel object */
    bs_uint8_t flag;                                    /**< flag of kernel object */

#ifdef BS_USING_MODULE
    void      *module_id;                               /**< id of application module */
#endif
    bs_list_t  list;                                    /**< list node of kernel object */
};
typedef struct bs_object *bs_object_t;                  /**< Type for kernel objects. */


/**
 * device (I/O) class type
 */
enum bs_device_class_type
{
    BS_Device_Class_Char = 0,                           /**< character device */
    BS_Device_Class_Block,                              /**< block device */
    BS_Device_Class_NetIf,                              /**< net interface */
    BS_Device_Class_MTD,                                /**< memory device */
    BS_Device_Class_CAN,                                /**< CAN device */
    BS_Device_Class_RTC,                                /**< RTC device */
    BS_Device_Class_Sound,                              /**< Sound device */
    BS_Device_Class_Graphic,                            /**< Graphic device */
    BS_Device_Class_I2CBUS,                             /**< I2C bus device */
    BS_Device_Class_USBDevice,                          /**< USB slave device */
    BS_Device_Class_USBHost,                            /**< USB host bus */
    BS_Device_Class_SPIBUS,                             /**< SPI bus device */
    BS_Device_Class_SPIDevice,                          /**< SPI device */
    BS_Device_Class_SDIO,                               /**< SDIO bus device */
    BS_Device_Class_PM,                                 /**< PM pseudo device */
    BS_Device_Class_Pipe,                               /**< Pipe device */
    BS_Device_Class_Portal,                             /**< Portal device */
    BS_Device_Class_Timer,                              /**< Timer device */
    BS_Device_Class_Miscellaneous,                      /**< Miscellaneous device */
    BS_Device_Class_Sensor,                             /**< Sensor device */
    BS_Device_Class_Touch,                              /**< Touch device */
    BS_Device_Class_Unknown                             /**< unknown device */
};

typedef struct bs_device *bs_device_t;
/**
 * operations set for device object
 */
struct bs_device_ops {
    /* common device interface */
    bs_err_t (*init)(bs_device_t dev);
    bs_err_t (*open)(bs_device_t dev, bs_uint16_t oflag);
    bs_err_t (*close)(bs_device_t dev);
    bs_size_t (*read)(bs_device_t dev, bs_off_t pos, void *buffer, bs_size_t size);
    bs_size_t (*write)(bs_device_t dev, bs_off_t pos, const void *buffer, bs_size_t size);
    bs_err_t (*control)(bs_device_t dev, int cmd, void *args);
};

/**
 * Device structure
 */
struct bs_device {
    struct bs_object          parent;                   /**< inherit from bs_object */

    enum bs_device_class_type type;                     /**< device type */
    bs_uint16_t               flag;                     /**< device flag */
    bs_uint16_t               open_flag;                /**< device open flag */

    bs_uint8_t                ref_count;                /**< reference count */
    bs_uint8_t                device_id;                /**< 0 - 255 */

    /* device call back */
    bs_err_t (*rx_indicate)(bs_device_t dev, bs_size_t size);
    bs_err_t (*tx_complete)(bs_device_t dev, void *buffer);

    const struct bs_device_ops *ops;
#ifdef BS_USING_CLOSE_OPS
    /* common device interface */
    bs_err_t (*init)(bs_device_t dev);
    bs_err_t (*open)(bs_device_t dev, bs_uint16_t oflag);
    bs_err_t (*close)(bs_device_t dev);
    bs_size_t (*read)(bs_device_t dev, bs_off_t pos, void *buffer, bs_size_t size);
    bs_size_t (*write)(bs_device_t dev, bs_off_t pos, const void *buffer, bs_size_t size);
    bs_err_t (*control)(bs_device_t dev, int cmd, void *args);
#endif
    void *user_data;                /**< device private data */
};


#endif /* end */

