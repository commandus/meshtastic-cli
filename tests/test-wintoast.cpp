#include "wintoastlib.h"
#include <string>
#include <windows.h>
#include <locale>
#include <codecvt>

using namespace WinToastLib;

// Source - https://stackoverflow.com/a/18374698
// Posted by dk123, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-12, License - CC BY-SA 4.0
static std::string wstring2utf8(
    const std::wstring &src
) {
    //setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(src);
}

static std::wstring utf82wstring(
    const std::string &src
) {
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(src);
}

class CustomHandler : public IWinToastHandler {
public:
    void toastActivated() const {
        std::cout << "The user clicked in this toast" << std::endl;
        exit(0);
    }

    void toastActivated(int actionIndex) const {
        std::cout << "The user clicked on action #" << actionIndex << std::endl;
        exit(16 + actionIndex);
    }

    void toastActivated(std::wstring response) const {
        std::cout << "The user replied with: " << wstring2utf8(response) << std::endl;
        exit(0);
    }

    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
            case UserCanceled:
                std::cout << "The user dismissed this toast" << std::endl;
                exit(1);
                break;
            case TimedOut:
                std::cout << "The toast has timed out" << std::endl;
                exit(2);
                break;
            case ApplicationHidden:
                std::wcout << L"The application hid the toast using ToastNotifier.hide()" << std::endl;
                exit(3);
                break;
            default:
                std::cout << "Toast not activated" << std::endl;
                exit(4);
                break;
        }
    }

    void toastFailed() const {
        std::wcout << L"Error showing current toast" << std::endl;
        exit(5);
    }
};

enum Results {
    ToastClicked,             // user clicked on the toast
    ToastDismissed,           // user dismissed the toast
    ToastTimeOut,             // toast timed out
    ToastHided,               // application hid the toast
    ToastNotActivated,        // toast was not activated
    ToastFailed,              // toast failed
    SystemNotSupported,       // system does not support toasts
    UnhandledOption,          // unhandled option
    MultipleTextNotSupported, // multiple texts were provided
    InitializationFailure,    // toast notification manager initialization failure
    ToastNotLaunched          // toast could not be launched
};

#define COMMAND_ACTION     "--action"
#define COMMAND_AUMI       "--aumi"
#define COMMAND_APPNAME    "--appname"
#define COMMAND_APPID      "--appid"
#define COMMAND_EXPIREMS   "--expirems"
#define COMMAND_TEXT       "--text"
#define COMMAND_HELP       "--help"
#define COMMAND_IMAGE      "--image"
#define COMMAND_SHORTCUT   "--only-create-shortcut"
#define COMMAND_AUDIOSTATE "--audio-state"
#define COMMAND_ATTRIBUTE  "--attribute"
#define COMMAND_INPUT      "--input"

void print_help() {
    std::wcout << "WinToast Console Example [OPTIONS]" << std::endl;
    std::wcout << "\t" << COMMAND_ACTION << L" : Set the actions in buttons" << std::endl;
    std::wcout << "\t" << COMMAND_AUMI << L" : Set the App User Model Id" << std::endl;
    std::wcout << "\t" << COMMAND_APPNAME << L" : Set the default appname" << std::endl;
    std::wcout << "\t" << COMMAND_APPID << L" : Set the App Id" << std::endl;
    std::wcout << "\t" << COMMAND_EXPIREMS << L" : Set the default expiration time" << std::endl;
    std::wcout << "\t" << COMMAND_TEXT << L" : Set the text for the notifications" << std::endl;
    std::wcout << "\t" << COMMAND_IMAGE << L" : set the image path" << std::endl;
    std::wcout << "\t" << COMMAND_ATTRIBUTE << L" : set the attribute for the notification" << std::endl;
    std::wcout << "\t" << COMMAND_SHORTCUT << L" : create the shortcut for the app" << std::endl;
    std::wcout << "\t" << COMMAND_AUDIOSTATE << L" : set the audio state: Default = 0, Silent = 1, Loop = 2" << std::endl;
    std::wcout << "\t" << COMMAND_INPUT << L" : add an input to the toast" << std::endl;
    std::wcout << "\t" << COMMAND_HELP << L" : Print the help description" << std::endl;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        print_help();
        return 0;
    }

    if (!WinToast::isCompatible()) {
        std::cerr << "Error, your system in not supported!" << std::endl;
        return Results::SystemNotSupported;
    }

    std::string appName        = "Console WinToast Example";
    std::string appUserModelID = "WinToast Console Example";
    std::string text           = "";
    std::string imagePath      = "";
    std::string attribute      = "default";
    std::vector<std::string> actions;
    INT64 expiration = 0;
    bool input       = false;

    bool onlyCreateShortcut                   = false;
    WinToastTemplate::AudioOption audioOption = WinToastTemplate::AudioOption::Default;

    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(COMMAND_IMAGE, argv[i])) {
            imagePath = argv[++i];
        } else if (!strcmp(COMMAND_ACTION, argv[i])) {
            actions.push_back(argv[++i]);
        } else if (!strcmp(COMMAND_EXPIREMS, argv[i])) {
            expiration = strtol(argv[++i], NULL, 10);
        } else if (!strcmp(COMMAND_APPNAME, argv[i])) {
            appName = argv[++i];
        } else if (!strcmp(COMMAND_AUMI, argv[i]) || !strcmp(COMMAND_APPID, argv[i])) {
            appUserModelID = argv[++i];
        } else if (!strcmp(COMMAND_TEXT, argv[i])) {
            text = argv[++i];
        } else if (!strcmp(COMMAND_ATTRIBUTE, argv[i])) {
            attribute = argv[++i];
        } else if (!strcmp(COMMAND_SHORTCUT, argv[i])) {
            onlyCreateShortcut = true;
        } else if (!strcmp(COMMAND_AUDIOSTATE, argv[i])) {
            audioOption = static_cast<WinToastTemplate::AudioOption>(std::stoi(argv[++i]));
        } else if (!strcmp(COMMAND_INPUT, argv[i])) {
            input = true;
        } else if (!strcmp(COMMAND_HELP, argv[i])) {
            print_help();
            return 0;
        } else {
            std::wcerr << L"Option not recognized: " << argv[i] << std::endl;
            return Results::UnhandledOption;
        }
    }

    WinToast::instance()->setAppName(utf82wstring(appName));
    WinToast::instance()->setAppUserModelId(utf82wstring(appUserModelID));

    if (onlyCreateShortcut) {
        if (!imagePath.empty() || !text.empty() || actions.size() > 0 || expiration) {
            std::wcerr << L"--only-create-shortcut does not accept images/text/actions/expiration" << std::endl;
            return 9;
        }
        enum WinToast::ShortcutResult result = WinToast::instance()->createShortcut();
        return result ? 16 + result : 0;
    }

    if (text.empty()) {
        text = "Hello, world!";
    }

    if (!WinToast::instance()->initialize()) {
        std::wcerr << L"Error, your system in not compatible!" << std::endl;
        return Results::InitializationFailure;
    }

    WinToastTemplate templ(!imagePath.empty() ? WinToastTemplate::ImageAndText02 : WinToastTemplate::Text02);
    templ.setTextField(utf82wstring(text), WinToastTemplate::FirstLine);
    templ.setAudioOption(audioOption);
    templ.setAttributionText(utf82wstring(attribute));
    templ.setImagePath(utf82wstring(imagePath));
    if (input) {
        templ.addInput();
    }

    for (auto const& action : actions) {
        templ.addAction(utf82wstring(action));
    }

    if (expiration) {
        templ.setExpiration(expiration);
    }

    if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        std::wcerr << L"Could not launch your toast notification!";
        return Results::ToastFailed;
    }

    // Give the handler a chance for 15 seconds (or the expiration plus 1 second)
    Sleep(expiration ? (DWORD) expiration + 1000 : 15000);

    exit(2);
}
