#ifndef DEVICELISTVIEW_H
#define DEVICELISTVIEW_H

#include <cinttypes>
#include <string>

#include "MeshtasticEnvironmentEventHandler.h"

class DeviceListView : public MeshtasticEnvironmentEventHandler {
public:
    MeshtasticEnvironment *environment;
    int rows;
    int cols;

    DeviceListView();
    DeviceListView(MeshtasticEnvironment *environment);
    ~DeviceListView();

    virtual void init() = 0;
    virtual void done() = 0;
    virtual void gotoXY(int x, int y) const = 0;
    virtual void textOut(int x, int y, const std::string &s) const = 0;
    virtual void attrOn(uint32_t attr) const = 0;
    virtual void attrOff(uint32_t attr) const = 0;
    virtual void clearScreen() const = 0;
    virtual void flushScreen() const = 0;
    virtual void getScreenSize() = 0;
    virtual int readKey() const = 0;
    virtual void draw() = 0;
    virtual void view() = 0;
};


#endif
