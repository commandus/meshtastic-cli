#ifndef PINCODEPROVIDER_H
#define PINCODEPROVIDER_H

#include <string>

class PinCodeProvider {
public:
    virtual std::string enterPinCode(const std::string &deviceName) = 0;
};

#endif
