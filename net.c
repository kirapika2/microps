#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "platform.h"

#include "util.h"
#include "net.h"

struct net_protocol
{
    struct net_protocol *next;
    uint16_t type;
    net_protocol_handler_t handler;
};

/*
 * NOTE: if you want to add/delete the entries after net_run(),
 *       you need to protect these lists with a lock.
 */
static struct net_device *devices; // 最初のデバイスのポインタ
static struct net_protocol *protocols;

// ネットワークデバイスのオブジェクト割り当て
struct net_device *
net_device_alloc(void)
{
    struct net_device *dev;

    dev = memory_alloc(sizeof(*dev)); // 直接malloc()を呼び出すのではなく、memory_alloc()を呼び出す
    if (!dev)
    {
        errorf("memory_alloc() failed");
        return NULL;
    }
    return dev;
}

// ネットワークデバイスを登録
/*
 * NOTE: must not be call after net_run()
 */
int net_device_register(struct net_device *dev)
{
    static unsigned int index = 0;

    dev->index = index++;
    snprintf(dev->name, sizeof(dev->name), "net%d", dev->index);
    dev->next = devices;
    devices = dev;
    infof("success, dev=%s, type=0x%04x", dev->name, dev->type);
    return 0;
}

// ネットワークデバイスを起動
static int
net_device_open(struct net_device *dev)
{
    infof("dev=%s", dev->name);
    if (NET_DEVICE_IS_UP(dev))
    {
        errorf("already opened, dev=%s", dev->name);
        return -1;
    }
    if (dev->ops->open)
    {
        if (dev->ops->open(dev) == -1)
        {
            errorf("failure, dev=%s", dev->name);
            return -1;
        }
    }
    dev->flags |= NET_DEVICE_FLAG_UP; // フラグを立てる
    return 0;
}

// ネットワークデバイスを停止
static int
net_device_close(struct net_device *dev)
{
    infof("dev=%s", dev->name);
    if (!NET_DEVICE_IS_UP(dev))
    {
        errorf("not opened, dev=%s", dev->name);
        return -1;
    }
    if (dev->ops->close)
    {
        if (dev->ops->close(dev) == -1)
        {
            errorf("failure, dev=%s", dev->name);
            return -1;
        }
    }
    dev->flags &= ~NET_DEVICE_FLAG_UP; // フラグを下ろす
    return 0;
}

// ネットワークデバイスへデータ出力
int net_device_output(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst)
{
    debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
    debugdump(data, len);
    if (!NET_DEVICE_IS_UP(dev))
    {
        errorf("not opened, dev=%s", dev->name);
        return -1;
    }
    if (dev->mtu < len)
    {
        errorf("too long, dev=%s, mtu=%u, len=%zu", dev->name, dev->mtu, len);
        return -1;
    }
    if (!dev->ops->output)
    {
        errorf("ouput callback function is not set, dev=%s", dev->name);
        return -1;
    }
    if (dev->ops->output(dev, type, data, len, dst) == -1)
    {
        errorf("failure, dev=%s, len=%zu", dev->name, len);
        return -1;
    }
    return 0;
}

/*
 * NOTE: must not be call after net_run()
 */
int net_protocol_register(uint16_t type, net_protocol_handler_t handler)
{
}

// ネットワークデバイスからのデータ入力
/*
type: 入力パケットのプロトコル種別
data: 入力パケットのバイト列
len: 入力パケットのバイト数
dev: 入力パケットを受信したネットワークデバイスのポインタ
*/
int net_input(uint16_t type, const uint8_t *data, size_t len, struct net_device *dev)
{
    debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
    debugdump(data, len);
    return 0;
}

int net_init(void)
{
    infof("initialize...");
    if (platform_init() == -1)
    {
        errorf("platform_init() failed");
        return -1;
    }
    infof("success");
    return 0;
}

int net_run(void)
{
    struct net_device *dev;

    infof("startup...");
    if (platform_run() == -1)
    {
        errorf("platform_run() failed");
        return -1;
    }
    // 登録されているネットワークデバイスを順に起動
    for (dev = devices; dev; dev = dev->next)
    {
        net_device_open(dev);
    }
    infof("success");
    return 0;
}

int net_shutdown(void)
{
    struct net_device *dev;

    infof("shutting down...");
    if (platform_shutdown() == -1)
    {
        warnf("platform_shutdown() failed");
    }
    // 登録されているネットワークデバイスを順に停止
    for (dev = devices; dev; dev = dev->next)
    {
        net_device_close(dev);
    }
    infof("success");
    return 0;
}
