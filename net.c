#include "platform.h"

#include "util.h"

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
    infof("startup...");
    if (platform_run() == -1)
    {
        errorf("platform_run() failed");
        return -1;
    }
    infof("success");
    return 0;
}

int net_shutdown(void)
{
    infof("shutting down...");
    if (platform_shutdown() == -1)
    {
        warnf("platform_shutdown() failed");
    }
    infof("success");
    return 0;
}
