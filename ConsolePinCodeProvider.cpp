#include <iostream>

#include "meshtastic.h"
#include "ConsolePinCodeProvider.h"

std::string ConsolePinCodeProvider::enterPinCode(
    const std::string &deviceName
) {
    std::string s;
    std::cout << _("Enter PIN code for device ") << deviceName << " ";
    std::cin >> s;
    return s;
}
