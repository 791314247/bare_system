/*
 * COPYRIGHT (C) 2018
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include "bc35.h"
#include "at_base.h"

static SoftTimer st_bc35;
static bs_bool_t timeoutFlag;

static void st_bc35_timeout_cb(void *args)
{
    timeoutFlag = BS_TRUE;
}

/*
 * This function initializes serial
 */
static bs_err_t bs_bc35_init(struct bs_device *dev)
{
    bs_bc35_t *bc35;

    BS_ASSERT(dev != BS_NULL);
    bc35 = (bs_bc35_t *)dev;

    bc35->ops->gpio_init(bc35);
    return (BS_EOK);
}

static bs_err_t bs_bc35_open(struct bs_device *dev, bs_uint16_t oflag)
{
    at_base_clean_recv();
    return (BS_EOK);
}

static bs_err_t bs_bc35_close(struct bs_device *dev)
{
    bs_bc35_t *bc35;

    BS_ASSERT(dev != BS_NULL);
    bc35 = (bs_bc35_t *)dev;

    bc35->ops->set_vdd(bc35, 0);
    return (BS_EOK);
}

static bs_err_t bc35_open(bs_bc35_t *dev) /* 开机 */
{
    static bs_bool_t at_wait_reset = BS_FALSE;
    static bs_bool_t at_wait_recv = BS_FALSE;

    if (at_wait_reset == BS_FALSE) {
        at_wait_reset = BS_TRUE;
        dev->ops->set_vdd(dev, 1);  /* 上电 */
        dev->ops->set_reset(dev, 0);  /* 拉低复位脚 */
        timeoutFlag = BS_FALSE;
        creat_single_soft_timer(&st_bc35,
                                RUN_IN_LOOP_MODE,
                                TIMER_SEC_DELAY * 1U,
                                st_bc35_timeout_cb, BS_NULL);
        bs_kprintf("Bc35 power on and press reset pin!");
    } else if (timeoutFlag && at_wait_reset == BS_TRUE && at_wait_recv == BS_FALSE) {
        dev->ops->set_reset(dev, 1);  /* BC35复位引脚拉低1秒后恢复，硬复位 */
        at_wait_recv = BS_TRUE;
        timeoutFlag = BS_FALSE;
        creat_single_soft_timer(&st_bc35,
                                RUN_IN_LOOP_MODE,
                                TIMER_SEC_DELAY * 10U,
                                st_bc35_timeout_cb, BS_NULL);
        bs_kprintf("release reset pin!");
    }

    if (at_wait_recv) { /* 打开电源并复位后等待收到ok */
        if (timeoutFlag || (at_base_read_recv("OK\r\n") != BS_NULL)) {
            stop_timer(&st_bc35);
            at_wait_recv = BS_FALSE;
            at_wait_reset = BS_FALSE;
            bs_kprintf("Bc35 boot complete!");
            return (BS_EOK);
        }
    }
    return (-BS_EBUSY);
}

static bs_err_t bc35_close(bs_bc35_t *dev) /* 关机 */
{
    dev->ops->set_vdd(dev, 0);
    return BS_EOK;
}

static bs_err_t bs_bc35_control(struct bs_device *dev,
                                int              cmd,
                                void             *args)
{
    bs_bc35_t *bc35;

    BS_ASSERT(dev != BS_NULL);
    bc35 = (bs_bc35_t *)dev;

    switch (cmd) {
    case BS_DEVICE_CTRL_BC35_BOOT:
        return bc35_open(dev);
    case BS_DEVICE_CTRL_BC35_CLOSE:
        return bc35_close(dev);
    case BS_DEVICE_CTRL_BC35_NET_INIT:
        break;
    case BS_DEVICE_CTRL_BC35_WAIT_LINK:
        break;
    case BS_DEVICE_CTRL_BC35_UPDATA_TIME:
        break;
    default:
        bs_kprintf("This command is not supported.");
        return -BS_ERROR;
    }
}


const static struct bs_device_ops bc35_ops = {
    bs_bc35_init,
    bs_bc35_open,
    bs_bc35_close,
    BS_NULL,
    BS_NULL,
    bs_bc35_control,
};


/**
 * This function register a bc35 device
 */
bs_err_t bs_hw_bc35_register(struct bs_bc35_device *bc35,
                             const char                *name,
                             bs_uint32_t                flag,
                             void                      *data)
{
    struct bs_device *device;
    BS_ASSERT(bc35 != BS_NULL);

    device = &(bc35->parent);

    device->type        = BS_Device_Class_Block;
    device->rx_indicate = BS_NULL;
    device->tx_complete = BS_NULL;
    device->ops         = &bc35_ops;
    device->user_data   = data;

    /* register a character device */
    return bs_device_register(device, name, flag);
}























#define WAIT_1S                     1     /* 发送命令后的最小等待时间 */
#define RESEND_ONCE                 1     /* 命令的重发次数 */
#define at_base_read_recv(str) (strstr(serial_readALL(BC35_SERIAL, BS_NULL), (str)))
#define CLEAN_SERIAL_BUF() (serial_clean_buf(BC35_SERIAL))

typedef enum {
    _AT_READY,    /* 可以开始通讯 */
    _AT_BUSY,     /* 通讯中 */
    _AT_OK,       /* 成功收到期望值 */
    _AT_ERROR     /* 未收到期望值，AT交互错误 */
} _AtCommStatus;  /* AT命令通讯状态 */

static bs_soft_timer_t st_bc35;
static bs_bool_t timeoutFlag = BS_FALSE;
static bs_bool_t onenetRegisterFlag = BS_FALSE;
static bs_uint32_t oneNETServerMsgId = 0;  /* OneNet平台msg ID，用于发送数据 */
static char sendCache[600];



/* 此函数将从模组读到的时间转换为北京时间 */
static bs_uint8_t GBStoLocalTime(bs_uint8_t *tbuf)
{
    unsigned int buf[8];
    unsigned int i = 0;
    unsigned char monthlengths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#define ADJFACTOR 1
    BS_ASSERT(tbuf);

    for (i = 0; i < 8; i++) {
        if (i == 0) {
            buf[i] = 2000 + tbuf[i];
            if (((buf[i] % 4 == 0) && (buf[i] % 100 != 0)) || (buf[i] % 400 == 0)) {
                monthlengths[2 - ADJFACTOR] = 29;
            }
        }
        buf[i] = tbuf[i];
    }
    buf[7] = (unsigned int)buf[7] * 15 / 60; //15min
    if (buf[6] == '+') {
        buf[3] += buf[7]; //timeZone
        if (buf[3] < 24) {
        } else {
            buf[3] %= 24; //h
            buf[2]++;     //dd
            if (buf[2] > monthlengths[buf[1] - ADJFACTOR]) {
                buf[2] %= monthlengths[buf[1] - ADJFACTOR];
                buf[1]++; //mm
                if (buf[1] > 12) {
                    buf[1] %= 12;
                    buf[0]++; //yy++
                }
            }
        }
    } else if (tbuf[6] == '-') {
        if (buf[3] >= buf[7]) {
            buf[3] -= buf[7];
        } else {
            buf[3] = buf[3] + 24 - buf[7]; //h
            buf[2]--;                      //dd
            if (!buf[2]) {
                buf[1]--; //mm
                buf[2] = monthlengths[buf[1] - ADJFACTOR];

                if (!buf[1]) {
                    buf[0]--; //yy--
                    buf[1] = 1;
                }
            }
        }
    }
    for (i = 0; i < 6; i++) {
        tbuf[i] = (bs_uint8_t)(((buf[i] % 100 / 10) << 4) + (buf[i] % 10));
    }
    return 1;
}





AtResult bc35_netInit(void) /* 联网初始化 */
{
#define NET_INIT_START     0
#define NET_INIT_END       17

    static bs_uint8_t step = NET_INIT_START;
    _AtCommStatus status = _AT_READY;

    switch (step) {
    case NET_INIT_START: /* read soft version and test module */
        status = at_send_command("AT+NCONFIG=AUTOCONNECT,FALSE\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        break;
    case 1:
        status = at_send_command("AT+QREGSWT=0\r\n", "OK\r\n", WAIT_1S * 5, RESEND_ONCE * 3); /* 启用平台自注册 */
        break;
    case 2:
        status = at_send_command("AT+NRB\r\n", "OK\r\n", WAIT_1S * 10, RESEND_ONCE * 1);
        if (status == _AT_ERROR) {
            status = _AT_OK;
        }
        break;
    case 3:
        status = at_send_command("AT+CMEE=1\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        break;
    case 4:
        status = at_send_command("AT+CFUN=0\r\n", "OK\r\n", WAIT_1S * 10, RESEND_ONCE * 5);
        break;
    case 5:
        status = at_send_command("AT+NCSEARFCN\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        break;
    case 6: {
        char *atCommandTelecom = "AT+NBAND=5\r\n";
        char *atCommandMobile = "AT+NBAND=8\r\n";
        char *atCommandUnicorn = "AT+NBAND=3\r\n";
        if (sibsype == TELECOM)
            status = at_send_command(atCommandTelecom, "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        else if (sibsype == MOBILE)
            status = at_send_command(atCommandMobile, "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        else if (sibsype == UNICORN)
            status = at_send_command(atCommandUnicorn, "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        else /* default */
            status = _AT_OK;

        /* The following is for BC95 compatibility */

        if (status == _AT_ERROR) {
            status = _AT_OK;
        }
        break;
    }
    case 7:
        status = at_send_command("AT+CFUN=1\r\n", "OK\r\n", WAIT_1S * 10, RESEND_ONCE * 5);
        break;
    case 8:
        status = at_send_command("AT+CIMI\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            char *p = BS_NULL;
            char mnc = 0;
            if ((p = at_base_read_recv("\r\n")) != BS_NULL) {
                p += 2;
                for (int i = 0; ((*(p + i) != '\r') && (i < sizeof(netInfo.IMSI))); i++)
                    netInfo.IMSI[i] = *(p + i);
                p += 3;
                mnc = ((*p - 0X30) * 10) + (*(p + 1) - 0x30);
                switch (mnc) {
                case 11:
                    if (sibsype != TELECOM) {
                        sibsype = TELECOM;
                        step = 1;
                    }
                    break;
                case 4:
                    if (sibsype != MOBILE) {
                        sibsype = MOBILE;
                        step = 1;
                    }
                    break;
                case 6:
                    if (sibsype != UNICORN) {
                        sibsype = UNICORN;
                        step = 1;
                    }
                    break;
                default: {
                    log_e("Sim mnc error !");
                    sibsype = UNKNOWN;
                    step = NET_INIT_START;
                    return AT_SIBSYPE_ERROR;
                }
                }
            }
        }
        break;
    case 9:
        status = at_send_command("AT+CEDRXS=0,5\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        // status = at_send_command("AT+NPSMR=1\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        break;
    case 10: /* read soft version */
        status = at_send_command("ATI\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            char *p = BS_NULL;
            if ((p = at_base_read_recv("Revision:")) != BS_NULL) {
                p += 9;
                for (int i = 0; ((*(p + i) != '\r') && (i < sizeof(netInfo.manRevSW))); i++)
                    netInfo.manRevSW[i] = *(p + i);
            }
        }
        break;
    case 11:
        status = at_send_command("AT+CGMR\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            char *p = BS_NULL;
            if ((p = at_base_read_recv("RADIO,")) != BS_NULL) {
                p += 6;
                for (int i = 0; ((*(p + i) != '\r') && (i < sizeof(netInfo.manRevHW))); i++)
                    netInfo.manRevHW[i] = *(p + i);
            }
        }
        break;
    case 12: /* check SIM card */
        status = at_send_command("AT+NCCID\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            char *p = BS_NULL;
            if ((p = at_base_read_recv("NCCID:")) != BS_NULL) {
                p += 6;
                for (int i = 0; ((*(p + i) != '\r') && (i < sizeof(netInfo.NCCID))); i++)
                    netInfo.NCCID[i] = *(p + i);
            }
        }
        break;
    case 13:
        status = at_send_command("AT+CGSN=1\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            char *p = at_base_read_recv("CGSN:");
            if ((p = at_base_read_recv("CGSN:")) != BS_NULL) {
                p += 5;
                for (int i = 0; ((*(p + i) != '\r') && (i < sizeof(netInfo.IMEI))); i++)
                    netInfo.IMEI[i] = *(p + i);
            }
        }
        break;
    case 14:
        status = at_send_command("AT+CGATT=1\r\n", "OK\r\n", WAIT_1S * 5, RESEND_ONCE * 3);
        break;
    case 15: /* 当平台确认收到消息时模组不发送URC */
        status = at_send_command("AT+NSMI=0\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        break;
    case 16: /* 当平台确认收到消息时模组不发送URC */
        status = at_send_command("AT+NNMI=0\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        break;
    case NET_INIT_END: /* 平台有消息时模组不提示 */
        status = at_send_command("AT+CPSMS=1,,,01000011,00000001\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            step = NET_INIT_START;
            return AT_SUCCESS_END;
        }
        break;
    default:
        step = NET_INIT_START;
        log_e("bc35 net init step error!");
        break;
    }

    if (status == _AT_OK) {
        step++;
    } else if (status == _AT_ERROR) { /* 代表重发3次后依然失败 */
        log_e("net init fail !");
        step = NET_INIT_START;
        return AT_FAIL_END;
    }
    return AT_WAIT;
}


AtResult bc35_waitLink(void) /* 等待网络连接成功 */
{
#define WAIT_NET_START 1
#define WAIT_NET_END   3
    static bs_uint8_t step = 0;
    _AtCommStatus status = _AT_READY;
    bs_int_32_t temp = 0;
    char *p = BS_NULL;

    switch (step) {
    case WAIT_NET_START: /* get csq */
        status = at_send_command("AT+CSQ\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            if ((p = at_base_read_recv("CSQ:")) != BS_NULL) {
                temp = atol(p + 4);
                if (temp >= 0 && temp <= 31)
                    netInfo.CSQ = (bs_uint8_t)temp;
            }
        }
        break;

    case 2: /* get power para */
        status = at_send_command("AT+NUESTATS\r\n", "OK\r\n", WAIT_1S * 5, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            if ((p = at_base_read_recv("ECL:")) != BS_NULL) {
                temp = atol(p + 4);
                if (temp != 255)
                    netInfo.ecl = (bs_uint8_t)temp;
            }
            if ((p = at_base_read_recv("SNR:")) != BS_NULL) {
                temp = atol(p + 4);
                if (temp != -32768)
                    netInfo.snr = (short)temp;
            }
            if ((p = at_base_read_recv("Signal power:")) != BS_NULL) {
                temp = atol(p + 13);
                if (temp != -32768)
                    netInfo.rsrp = (short)temp;
            }
            if ((p = at_base_read_recv("PCI:")) != BS_NULL) {
                temp = atol(p + 4);
                if (temp != 65535)
                    netInfo.pci = (bs_uint16_t)temp;
            }
            if ((p = at_base_read_recv("ID:")) != BS_NULL) {
                temp = atol(p + 3);
                if (temp != -1)
                    netInfo.cellId = (bs_uint32_t)temp;
            }
            if ((p = at_base_read_recv("EARFCN:")) != BS_NULL) {
                temp = atol(p + 7);
                if (temp != -1)
                    netInfo.earFcn = (bs_uint32_t)temp;
            }
        }
        break;

    case WAIT_NET_END: /* check register status, 这里是无限等待注册网络，调用时必须做超时处理 */
        status = at_send_command("AT+CEREG?\r\n", "+CEREG:0,1", WAIT_1S * 2U, RESEND_ONCE * 1);
        if (status == _AT_OK) {
            step = 0;              /* 结束后初始化步数，为下次进入做准备 */
            return AT_SUCCESS_END;
        } else if (status == _AT_ERROR) {
            step = 0;
            if ((p = at_base_read_recv("+CEREG:")) != BS_NULL) {
                *p = *(p + 9) - 0X30;
                switch (*p) {
                case 5:                     /* 正常漫游 */
                    return AT_SUCCESS_END;
                case 0:                     /* 未注册，UE 目前未搜索运营商进行注册 */
                    fault_write(ERROR_NET_REGISTER_NO_SEARCH);
                    return AT_FAIL_END;
                case 1:                     /* 注册被拒绝 */
                    fault_write(ERROR_NET_REGISTER_DENIED);
                    return AT_FAIL_END;
                case 4:                     /* 未知（例如不在 E-UTRAN 范围内）*/
                    fault_write(ERROR_NET_REGISTER_UNKNOW);
                    return AT_FAIL_END;
                default:
                    break;
                }
            }
            status = _AT_READY;
        }
        break;

    default:
        step = WAIT_NET_START;
        log_e("Network register step error !");
        break;
    }

    if (status == _AT_OK) {
        step++;
    } else if (status == _AT_ERROR) {
        /* 结束处理 */
        log_e("Network register is fail !");
        step = WAIT_NET_START; /* 结束后初始化步数，为下次进入做准备 */
        return AT_FAIL_END;
    }
    return AT_WAIT;
}

AtResult bc35_updataTime(char *destin, bs_uint32_t busz) /* 更新时间 */
{
    _AtCommStatus status = _AT_READY;
    char *p1 = BS_NULL;
    char timeTemp[8];
    char i;
    BS_ASSERT(destin);

    status = at_send_command("AT+CCLK?\r\n", "+CCLK:", WAIT_1S * 2, RESEND_ONCE * 30);

    if (status == _AT_OK) {
        if ((p1 = at_base_read_recv("+CCLK:")) != BS_NULL) {
            p1 += 6;
            for (i = 0; i < 6; i++) {
                timeTemp[i] = ((*(p1 + 3 * i) - 0x30) * 10) + (*(p1 + 3 * i + 1) - 0x30);
            }
            timeTemp[6] = *(p1 + 3 * i - 1); //+-
            timeTemp[7] = ((*(p1 + 3 * i) - 0x30) * 10) + (*(p1 + 3 * i + 1) - 0x30);
            if (GBStoLocalTime((unsigned char *)timeTemp)) {
                if ((timeTemp[0] > 0x16) && (timeTemp[1] < 0x13) && (timeTemp[1] > 0x00) && (timeTemp[2] < 0x32) &&
                    (timeTemp[2] > 0x00) && (timeTemp[3] < 0x24) && (timeTemp[4] < 0x60) && (timeTemp[5] < 0x60)) {
                    if (busz >= 6U) {
                        memcpy(destin, timeTemp, 6U); /* year_l -> sec */
                    }
                }
            }
            return AT_SUCCESS_END;
        }
    } else if (status == _AT_ERROR) {
        return AT_FAIL_END;
    }

    return AT_WAIT;
}

/**
 * 连接到云平台
 */
AtResult bc35_link_OC(const char *ip)
{
    _AtCommStatus status = _AT_READY;
    static bs_uint8_t step = 0;
    static bs_uint8_t queries = 0;  /* 等待注册到平台的时间，单位是秒 */
    char *comand_head = "AT+NCDP=";
    char comand[40];
    BS_ASSERT(ip);

    switch (step) {
    case 0: /* set ip and port */
        memset(comand, 0, sizeof(comand));
        strncat(comand, comand_head, strlen(comand_head));
        strncat(comand, ip, strlen(ip));
        strncat(comand, "\r\n", 2U);
        status = at_send_command(comand, "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 20);
        break;
    case 1: /* link telecom platform */
        status = at_send_command("AT+QLWSREGIND=0\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3); /* 手动注册到电信云平台 */
        break;
    case 2:  /* check link status */
        status = at_send_command("AT+NMSTATUS?\r\n", "MO_DATA_ENABLED", WAIT_1S * 2, RESEND_ONCE * 1); /* check socket */
        if (status == _AT_OK) {
            queries = 0;
            step = 0;
            return AT_SUCCESS_END;
        } else if (status == _AT_ERROR) {
            if ((at_base_read_recv("REJECTED_BY_SERVER") != BS_NULL) ||  /* 平台未注册 */
                (at_base_read_recv("REG_FAILED") != BS_NULL)) { /* IP地址错误 */
                step = 0;
                return AT_REGISTER_FAIL;
            } else {
                status = _AT_READY;
                queries++;
                if (queries >= 100) { /* 最长等待160s */
                    queries = 0;
                    step = 0;
                    return AT_REGISTER_FAIL;
                }
            }
        }
        break;

    default:
        step = 0;
        break;
    }

    if (status == _AT_OK) {
        step++;
    } else if (status == _AT_ERROR) {
        step = 0;
        return AT_FAIL_END;
    }
    return AT_WAIT;
}

AtResult bc35_send_OC(const char *source, bs_uint32_t n)
{
    static _AtCommStatus status = _AT_READY;
    char *sendHead = "AT+NMGS=";
    BS_ASSERT(source);
    BS_ASSERT(n < 548);

    if (status == _AT_READY) {
        memset(sendCache, 0, sizeof(sendCache));
        strncat(sendCache, sendHead, strlen(sendHead));
        my_itoa((bs_uint8_t *)(sendCache + strlen(sendCache)), (n / 2)); //填数据长度,如: "AT+NMGS=XXX"
        strncat(sendCache, ",", 1U);
        strncat(sendCache, source, n);
        strncat(sendCache, "\r\n", 2);
    }

    status = at_send_command(sendCache, "OK\r\n", WAIT_1S * 5, RESEND_ONCE * 10);
    if (status == _AT_OK) { /* 因为只有一条命令，所以直接转义结果 */
        status = _AT_READY;
        return AT_SUCCESS_END;
    } else if (status == _AT_ERROR) {
        status = _AT_READY;
        return AT_FAIL_END;
    }
    return AT_WAIT;
}

AtResult bc35_read_OC(char *destin, bs_uint32_t busz)
{
#define READ_SOCKET_NUM               20
    static bs_uint8_t readCount = 0;
    static bs_bool_t response = BS_FALSE;
    _AtCommStatus status = _AT_READY;

    char *p1 = BS_NULL;
    char i;
    BS_ASSERT(destin);

    status = at_send_command("AT+NMGR\r\n", ",", WAIT_1S * 1, RESEND_ONCE * 1); /* check socket */
    if (status == _AT_ERROR) {
        if (++readCount >= (READ_SOCKET_NUM - 1)) {
            readCount = 0;
            if (response) {
                response = BS_FALSE;
                return AT_SUCCESS_END;
            } else {
                return AT_REPORT_NO_RESPONSE;
            }
        }
    } else if (status == _AT_OK) { /* 收到数据 */
        if ((p1 = at_base_read_recv(",")) != BS_NULL) {
            if (at_base_read_recv("AAAA0000") != BS_NULL) { /* 当收到4,AAAA0000时代表收到平台回复，不重发 */
                response = BS_TRUE;
                return AT_WAIT;
            } else {
                p1++; //略过','
                for (i = 0; ((*(p1 + i) != '\r') && (i < busz)); i++)
                    destin[i] = *(p1 + i);
                readCount = 0;
                return AT_RECIEVED_DATA;
            }
        }
    }
    return AT_WAIT;
}

AtResult bc35_enter_PSM(void)
{
    _AtCommStatus status = _AT_READY;

    /* 指示电信coap快速进入低功耗状态 */
    status = at_send_command("AT+QLWULDATAEX=3,AA34BB,0x0001\r\n", "OK\r\n", 2, 3);
    if (status == _AT_OK) /* 因为只有一条命令，所以直接转义结果 */
        return AT_SUCCESS_END;
    else if (status == _AT_ERROR)
        return AT_FAIL_END;
    return AT_WAIT;
}

/**
 * 连接到云平台
 */
AtResult bc35_link_OneNET(const char *ip)
{
    _AtCommStatus status = _AT_READY;
    static bs_uint8_t step = 0;
    char *comand_head = "AT+MIPLCONFIG=1,";
    char comand[40];

    switch (step) {
    case 0: /* Creat socket */
        memset(comand, 0, sizeof(comand));
        strncat(comand, comand_head, strlen(comand_head));
        strncat(comand, ip, strlen(ip));
        strncat(comand, "\r\n", 2U);
        status = at_send_command(comand, "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 20);
        if (status == _AT_ERROR) {
            /* IP错误处理 */
            step = 0;
            return AT_REGISTER_FAIL;
        }
        break;
    case 1: /* Creat socket */
        status = at_send_command("AT+MIPLCREATE\r\n", "+MIPLCREATE:0", WAIT_1S * 2, RESEND_ONCE * 3);
        break;
    case 2: /* creat object */
        status = at_send_command("AT+MIPLADDOBJ=0,3200,1,\"1\",1,0\r\n", "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        break;
    case 3:
        status = at_send_command("AT+MIPLOPEN=0,90000,60\r\n", "+MIPLEVENT: 0,6", WAIT_1S * 160, RESEND_ONCE * 1);
        if (status == _AT_OK) {
            /* 结束处理 */
            onenetRegisterFlag = BS_TRUE;
            step = 0; /* 结束后初始化步数，为下次进入做准备 */
            return AT_SUCCESS_END;
        } else if (status == _AT_ERROR) {
            /* IP错误处理 */
            step = 0;
            return AT_REGISTER_FAIL;
        }
        break;
    default:
        step = 0;
        break;
    }

    if (status == _AT_OK) {
        step++;
    } else if (status == _AT_ERROR) {
        step = 0; /* 结束后初始化步数，为下次进入做准备 */
        return AT_FAIL_END;
    }
    return AT_WAIT;
}

AtResult bc35_send_OneNET(const char *source, bs_uint32_t n)
{
    _AtCommStatus status = _AT_READY;
    char *sendHead = "AT+MIPLNOTIFY=0,";
    char *section1 = ",3200,0,5505,1,";
    char *section2 = ",0,0,1,0X400\r\n";
    static bs_bool_t sendingFlag = BS_FALSE;
    BS_ASSERT(source);
    BS_ASSERT(n < 548);

    if (!sendingFlag) {
        sendingFlag = BS_TRUE;
        memset(sendCache, 0, sizeof(sendCache));
        strncat(sendCache, sendHead, strlen(sendHead));
        my_itoa((bs_uint8_t *)(sendCache + strlen(sendCache)), oneNETServerMsgId); //填ID
        strncat(sendCache + strlen(sendCache), section1, strlen(section1));
        my_itoa((bs_uint8_t *)(sendCache + strlen(sendCache)), n); //填数据长度,如: "AT+NMGS=XXX"
        strncat(sendCache, ",", 1U);
        strncat(sendCache, source, n);
        strncat(sendCache + strlen(sendCache), section2, strlen(section2));
    }

    status = at_send_command(sendCache, "+MIPLEVENT: 0,26", WAIT_1S * 60, RESEND_ONCE * 3);
    if (status == _AT_OK) { /* 因为只有一条命令，所以直接转义结果 */
        sendingFlag = BS_FALSE;
        return AT_SUCCESS_END;
    } else if (status == _AT_ERROR) {
        sendingFlag = BS_FALSE;
        return AT_FAIL_END;
    }
    return AT_WAIT;
}


AtResult bc35_read_OneNET(char *destin, bs_uint32_t busz)
{
#define ONENET_UPDATA                   0
#define ONENET_CREAT_WAIT_TIME          1
#define ONENET_WAIT_URC                 2
#define ONENET_RESPONSE_DISCOVER        3
#define ONENET_RESPONSE_EXECUTE         4
#define ONENET_REPORT_DATA              5
    _AtCommStatus status = _AT_READY;
    static bs_uint8_t step = 0;
    static bs_uint32_t coverMsgId = 0;
    static bs_uint32_t executeMsgId = 0;
    char comand[50] = {0};

    switch (step) {
    case ONENET_UPDATA:
        if (!onenetRegisterFlag) {
            status = at_send_command("AT+MIPLUPDATE=0,86400,0\r\n", "+MIPLEVENT: 0,11", WAIT_1S * 200, RESEND_ONCE * 1);
            if ((status == _AT_OK) || (at_base_read_recv("+MIPLEVENT: 0,6") != BS_NULL)) {
                step = ONENET_CREAT_WAIT_TIME;
            }
        } else {
            step = ONENET_CREAT_WAIT_TIME;
        }
        break;
    case ONENET_CREAT_WAIT_TIME:
        timeoutFlag = BS_FALSE;
        restart_single_soft_timer(&st_bc35,
                                  RUN_IN_LOOP_MODE,
                                  TIMER_SEC_DELAY * 15,
                                  st_bc35_timeout_cb, BS_NULL);
        step = ONENET_WAIT_URC;
        break;
    case ONENET_WAIT_URC: { /* wait urc */
        char *p = BS_NULL;
        if ((p = at_base_read_recv("+MIPLOBSERVE: ")) != BS_NULL) {
            if (strstr(p, "\r\n") != BS_NULL) {
                p += strlen("+MIPLOBSERVE: ");
                p += 2; /* 0, */
                oneNETServerMsgId = atol(p);
                CLEAN_SERIAL_BUF();
            }
        }
        if ((p = at_base_read_recv("+MIPLEXECUTE: ")) != BS_NULL) {
            if (strstr(p, "\r\n") != BS_NULL) {
                p += strlen("+MIPLEXECUTE: ");
                p += 2; /* 0, */
                executeMsgId = atol(p);
                if ((p = strstr(p, "\"")) != BS_NULL) {
                    p++;
                    for (int i = 0; ((*(p + i) != '\"') && i < busz); i++)
                        destin[i] = *(p + i);
                }
                CLEAN_SERIAL_BUF();
                step = ONENET_RESPONSE_EXECUTE;
                return AT_RECIEVED_DATA;
            }
        }
        if ((p = at_base_read_recv("+MIPLDISCOVER: ")) != BS_NULL) {
            if (strstr(p, "\r\n") != BS_NULL) {
                p += strlen("+MIPLDISCOVER: ");
                p += 2; /* 0, */
                coverMsgId = atol(p);
                CLEAN_SERIAL_BUF();
                step = ONENET_RESPONSE_DISCOVER;
            }
        }
        if (timeoutFlag) { /* 限制URC等待时间 */
            onenetRegisterFlag = BS_FALSE;
            step = ONENET_UPDATA;
            return AT_SUCCESS_END;  /* 结束业务 */
        }
        break;
    }
    case ONENET_RESPONSE_DISCOVER:  {/* response */
        char *pTemp1 = "AT+MIPLDISCOVERRSP=0,";
        char *pTemp2 = ",1,4,\"5505\"\r\n";

        memset(comand, 0, sizeof(comand));
        strncat(comand, pTemp1, strlen(pTemp1));
        my_itoa((bs_uint8_t *)(comand + strlen(comand)), coverMsgId);
        strncat(comand + strlen(comand), pTemp2, strlen(pTemp2));
        status = at_send_command(comand, "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            step = ONENET_CREAT_WAIT_TIME;
        }
        break;
    }
    case ONENET_RESPONSE_EXECUTE:  { /* response */
        char *pTemp1 = "AT+MIPLEXECUTERSP=0,";
        char *pTemp2 = ",2,0\r\n";

        memset(comand, 0, sizeof(comand));
        strncat(comand, pTemp1, strlen(pTemp1));
        my_itoa((bs_uint8_t *)(comand + strlen(comand)), executeMsgId);
        strncat(comand + strlen(comand), pTemp2, strlen(pTemp2));
        status = at_send_command(comand, "OK\r\n", WAIT_1S * 2, RESEND_ONCE * 3);
        if (status == _AT_OK) {
            step = ONENET_CREAT_WAIT_TIME;
        }
        break;
    }
    default:
        step = ONENET_UPDATA;
        break;
    }

    if (status == _AT_ERROR) {
        step = ONENET_UPDATA;
        return AT_FAIL_END;
    }
    return AT_WAIT;
}


int bc35_hw_init()
{
    bs_pin_mode(NB_VDD_PIN, PIN_MODE_OUTPUT);
    bs_pin_mode(NB_RESET_PIN, PIN_MODE_OUTPUT);
    bs_pin_write(NB_VDD_PIN, PIN_HIGH);
    bs_pin_write(NB_RESET_PIN, PIN_LOW);
    return 0;
}

INIT_BOARD_EXPORT(bc35_hw_init);
