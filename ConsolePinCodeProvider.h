#ifndef CONSOLEPINCODEPROVIDER_H
#define CONSOLEPINCODEPROVIDER_H

#include "PinCodeProvider.h"

class ConsolePinCodeProvider : public PinCodeProvider {
public:
    std::string enterPinCode(const std::string &deviceName) override;
};

#endif
