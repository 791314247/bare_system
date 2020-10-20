/*
 * COPYRIGHT (C) 2018
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef __BC35CLASS_H
#define __BC35CLASS_H

#include <bare_sys.h>
#include <bsdevice.h>


#define BS_DEVICE_CTRL_BC35_BOOT               (1) /* get timeout(in seconds) */
#define BS_DEVICE_CTRL_BC35_CLOSE              (2) /* set timeout(in seconds) */
#define BS_DEVICE_CTRL_BC35_NET_INIT           (3) /* get the left time before reboot(in seconds) */
#define BS_DEVICE_CTRL_BC35_CHECK_LINK_STATUS  (4) /* refresh bc35 */
#define BS_DEVICE_CTRL_BC35_UPDATA_TIME        (5) /* start bc35 */
#define BS_DEVICE_CTRL_BC35_ENTER_PSM          (6) /* stop bc35 */

/**
 * SIM卡类型
 */
enum sim_type {
    UNKNOWN, /* 未知运行商，不锁频段 */
    TELECOM, /* 电信 */
    MOBILE,  /* 移动 */
    UNICORN  /* 联通 */
};

struct net_informations {
    bs_uint8_t manRevSW[20];      //字符串 大端对齐模式
    bs_uint8_t manRevHW[20];      //字符串 大端对齐模式
    bs_uint8_t NCCID[20];         //字符串 大端对齐模式
    bs_uint8_t IMEI[20];          //字符串 大端对齐模式
    bs_uint8_t IMSI[20];          //字符串 大端对齐模式
    bs_uint32_t earFcn;           //频点
    bs_uint32_t cellId;           //cellId
    short snr;                    //信噪比
    short rsrp;                   //接收电平
    bs_uint8_t CSQ;               //10进制0-31
    bs_uint8_t ecl;               //覆盖等级
    bs_uint16_t pci;              //小区号 0-503
    enum sim_type simType;     //sim卡型号
};


struct bs_bc35_ops;
struct bs_bc35_device
{
    struct bs_device parent;
    const struct bs_bc35_ops *ops;
    struct net_informations net_i;
};
typedef struct bs_bc35_device bs_bc35_t;

struct bs_bc35_ops
{
    void (*gpio_init)(bs_bc35_t *bc35);
    void (*set_vdd)(bs_bc35_t *bc35, int value);
    void (*set_reset)(bs_bc35_t *bc35, int value);
};

bs_err_t bs_hw_bc35_register(bs_bc35_t *bc35,
                                 const char    *name,
                                 bs_uint32_t    flag,
                                 void          *data);









// AtResult bc35_open(void);                                  /* 开机 */
// AtResult bc35_close(void);                                 /* 关机 */
// AtResult bc35_netInit(void);                               /* 联网初始化 */
// AtResult bc35_waitLink(void);                              /* 等待网络连接成功 */
// AtResult bc35_updataTime(char *destin, bs_uint32_t busz);  /* 更新时间 */
// AtResult bc35_enter_PSM(void);


// /* OC平台Socket接口 */
// AtResult bc35_link_OC(const char *ip);
// AtResult bc35_send_OC(const char *source, bs_uint32_t n);
// AtResult bc35_read_OC(char *destin, bs_uint32_t busz);

// /* OneNET平台Socket接口 */
// AtResult bc35_link_OneNET(const char *ip);
// AtResult bc35_send_OneNET(const char *source, bs_uint32_t n);
// AtResult bc35_read_OneNET(char *destin, bs_uint32_t busz);





#endif /*__BC35_H*/
