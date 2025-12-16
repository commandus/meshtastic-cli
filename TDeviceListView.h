#ifndef TDEVICELISTVIEW_H
#define TDEVICELISTVIEW_H

#include <string>
#include <map>
#include "curses.h"
#include "DeviceListView.h"
#include "MeshtasticOSEnvironment-win.h"

class MeshtasticDevice;

class TDeviceListView : DeviceListView {
private:
    bool viewProcessKey(int key);
protected:
    std::map<MeshtasticTransport*, bool> discoveryOn;
    void showDiscoveryState();
public:
    void init() override;
    void done() override;
    void gotoXY(int x, int y) const override;
    void textOut(int x, int y, const std::string &s) const override;
    void attrOn(uint32_t attr) const override;
    void attrOff(uint32_t attr) const override;
    void clearScreen() const override;
    void flushScreen() const override;
    void getScreenSize() override;
    int readKey() const override;
    void draw() override;

    void view() override;


    TDeviceListView();
    TDeviceListView(MeshtasticOSEnvironment *environment);
    ~TDeviceListView();

    bool processEvent(const MeshtasticEnvironmentEvent &event) override;

    void showDiscoveredDevice(MeshtasticDiscoveryEvent *aDiscoveryEvent);
};

#endif
