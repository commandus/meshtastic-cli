#include <sstream>
#include <iostream>

#include "MeshtasticString.h"
#include "TDeviceListView.h"

void TDeviceListView::init()
{
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_WHITE, COLOR_RED);
    keypad(stdscr, TRUE);

    if (environment)
        environment->addEventHandler(this);
}

void TDeviceListView::done()
{
    endwin();
    if (environment)
        environment->rmEventHandler(this);
}

void TDeviceListView::gotoXY(int x, int y) const
{
    move(y, x);
}

void TDeviceListView::textOut(
    int x,
    int y,
    const std::string &s
) const
{
    move(y, x);
    // 012345 cols = 6
    //   xxxxx  x = 2 s.len = 5 extra = (2 + 5) - 6 = 1
    //
    int extraLen = x + s.length() - cols;
    if (extraLen > 0) {
        std::string ss(s.substr(0, s.length() - extraLen));
        printw(ss.c_str());
        return;
    }
    printw(s.c_str());
}

void TDeviceListView::attrOn(uint32_t attr) const
{
    attron(COLOR_PAIR(attr));
}

void TDeviceListView::attrOff(uint32_t attr) const
{
    attroff(COLOR_PAIR(attr));
}

void TDeviceListView::clearScreen() const
{
    clear();
}

void TDeviceListView::flushScreen() const
{
    refresh();
}

void TDeviceListView::getScreenSize() {
    resize_term(0, 0);
    getmaxyx(stdscr, rows, cols);
}

int TDeviceListView::readKey() const
{
    return getch();
}

TDeviceListView::TDeviceListView()
    : DeviceListView(nullptr)
{
    init();
}

TDeviceListView::TDeviceListView(
    MeshtasticOSEnvironment *environment
)
    : DeviceListView(environment)
{
    init();
}


TDeviceListView::~TDeviceListView()
{
    done();
}

bool TDeviceListView::viewProcessKey(
    int key
) {
    bool r = true;
    switch (key) {
        case KEY_BTAB:
        case KEY_LEFT:
            break;
        case 9:
        case 10:
            break;
        case KEY_RIGHT:
            break;
        case KEY_HOME:
            break;
        case KEY_END:
            break;
        case KEY_UP:
            break;
        case KEY_DOWN:
            break;
        case KEY_PPAGE:
            break;
        case KEY_NPAGE:
            break;
        case KEY_F(5):
            break;
        case KEY_RESIZE:
            break;
        default:
            r = false;
    }
    return r;
}

void TDeviceListView::view() {
    draw();
    bool stopRequest = false;
    while (!stopRequest) {
        auto ch = readKey();
        viewProcessKey(ch);
        switch (ch) {
            case 27:
                stopRequest = true;
                break;
        }
        draw();
    }
}

void TDeviceListView::draw()
{
    getScreenSize();
    clearScreen();
    textOut(0, 0, "Press <Esc> to quit.");
    showDiscoveryState();
    flushScreen();
}

bool TDeviceListView::processEvent(
    const MeshtasticEnvironmentEvent &event
) {
    switch (event.eventType) {
        case ET_DISCOVERY_OFF:
            discoveryOn[event.transport] = false;
            showDiscoveryState();
            flushScreen();
            break;
        case ET_DISCOVERY_ON:
            discoveryOn[event.transport] = true;
            showDiscoveryState();
            flushScreen();
            break;
        case ET_DEVICE_DISCOVERED:
            showDiscoveredDevice(event.de);
            flushScreen();
            break;

    }
    return false;
}

void TDeviceListView::showDiscoveryState() {
    std::stringstream ss;
    size_t sz = 0;
    for (auto &t : discoveryOn) {
        std::string s = ' ' + MeshtasticString::transportType2String(t.first->transportType) + ' ';
        if (t.second)
            attrOn(3);
        else
            attrOn(4);
        textOut(sz, 1, s);
        sz += s.length() + 1;
    }
    attrOn(1);
}

void TDeviceListView::showDiscoveredDevice(
    MeshtasticDiscoveryEvent *aDiscoveryEvent
) {
    std::stringstream ss;
    ss << "Discovery " << aDiscoveryEvent->deviceAddressString() << ' ' << aDiscoveryEvent->deviceName
        << ' ' << aDiscoveryEvent->signalStrength;
    textOut(0, 2, ss.str());
}
