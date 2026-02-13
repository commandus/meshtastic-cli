/**
 *  ./mtcli -vvv --ble
 *  ./mtcli -vvv -c COM4  --mode nodes
 *  ./mtcli -vvv -c COM4  --mode sendtext --node !9aa9259d --value test1
 */

#include <iostream>
#include "argtable3/argtable3.h"

#include "daemonize.h"
#include "file-helper.h"
#include "TDeviceListView.h"
#include "ConsolePinCodeProvider.h"

#include "MeshtasticString.h"

#include "MeshtasticOSEnvironment.h"
#include "MeshtasticBLETransport.h"
#include "MeshtasticSerialTransport.h"

#ifdef _MSC_VER
#else
#include <csignal>
#endif

static const char *programName = "mtcli";

enum ClientRunningState {
    CRS_STOPPED = 0,
    CRS_RUNNING,
    CRS_STOP_REQUEST
};

enum ClientMode {
    CM_LISTEN = 0,  // default
    CM_LIST_NODES,
    CM_LIST_CHANNELS,
    CM_SEND_TEXT
};

class MyEventHandler : public MeshtasticEnvironmentEventHandler {
public:
    bool processEvent(
        const MeshtasticEnvironmentEvent &event
    ) override
    {
        switch (event.eventType) {
            case ET_DISCOVERY_OFF:
                std::cout << _("Discovery off") << std::endl;
                break;
            case ET_DISCOVERY_ON:
                std::cout << _("Discovery on") << std::endl;
                break;
            case ET_DEVICE_DISCOVERED: {
                std::string d = event.de->isMeshtasticDevice ? _("Meshtastic ") : "";
                std::cout << d << _("Device discovered ") << event.de->deviceAddressString() << ' ' << event.de->deviceName
                          << ' ' << event.de->signalStrength << "dBm" << std::endl;
                if (event.de->isMeshtasticDevice) {
                }
            }
                break;
            case ET_DEVICE_FROM_RADIO:
                std::cout << _("From radio  ") << event.de->deviceAddressString() << ' ' << event.de->deviceName
                          << ' ' << event.de->signalStrength << std::endl;
                break;
            case ET_DEVICE_TO_RADIO:
                std::cout << _("To radio  ") << event.de->deviceAddressString() << ' ' << event.de->deviceName
                          << ' ' << event.de->signalStrength << std::endl;
                break;
            case ET_DEVICE_ADD:
                {
                    std::string d = event.de->isMeshtasticDevice ? _("Meshtastic ") : "";
                    std::cout << d << _("Device configured ") << event.de->deviceAddressString() << ' ' << event.de->deviceName
                              << ' ' << event.de->signalStrength << "dBm" << std::endl;
                    // stop discovery if necessary
                    // env.stopDiscovery(MTT_BLE, "");
                }
                break;
            case ET_DEVICE_RM:
                break;
            case ET_DEVICE_LOG:
                break;
        }
        return false;
    }
};

class MeshtasticEnvNParams {
public:
    // OS specific initialization/finalization
    MeshtasticOSEnvironment env;

    // running state
    std::mutex mutexState;
    std::condition_variable cvState;

    ClientRunningState state;
    ClientMode mode;
    bool showActivity;
    int verbose;
    bool daemonize;
    bool enableBLE;
    bool enableSerial;
    std::vector<uint32_t> nodeNums;
    std::string value;

    std::string pidFile;

    MeshtasticBLETransport bleTransport = MeshtasticBLETransport(&env);
    MeshtasticSerialTransport serialTransport = MeshtasticSerialTransport(&env);

    MeshtasticEnvNParams()
        : state(CRS_STOPPED), mode(CM_LISTEN), showActivity(false), verbose(0), daemonize(false),
            enableBLE(false), enableSerial(false)
    {
    }

    MeshtasticEnvNParams(int argc, char **argv)
        : MeshtasticEnvNParams()
    {
        parseArgs(argc, argv);
    }

    bool parseArgs(int argc, char **argv) {
        struct arg_lit *a_show_activity = arg_lit0("a", "activity", _("show device activity"));
        struct arg_lit *a_daemonize = arg_lit0("d", "daemonize", _("Run as daemon"));
        struct arg_str *a_pid_file = arg_str0("p", "pidfile", _("<file>"), _("Check whether a process has created the file pidfile"));
        struct arg_lit *a_enable_ble = arg_lit0("b", "ble", _("enable BLE devices"));
        struct arg_str *a_ble_macs = arg_strn("m", "mac", _("<Bluetooth MAC address>"), 0, 100, _("Enable specified BLE device by address e.g. e1:ce:9a:a9:25:9d"));
        struct arg_str *a_serial_ports = arg_strn("c", "port", _("<Serial port device>"), 0, 100, _("Enable specified serial port e.g. COM4"));

        struct arg_str *a_mode = arg_str0(nullptr, "mode", _("listen|nodes"), _("Select client mode"));

        struct arg_str *a_node_id = arg_strn("n", "node", _("<node num>"), 0, 100, _("node hex number"));
        struct arg_str *a_value = arg_str0(nullptr, "value", _("<text>"), _("value"));

        struct arg_lit *a_verbose = arg_litn("v", "verbose", 0, 3, _("-v print errors -vv warnings -vvv info"));
        struct arg_lit *a_help = arg_lit0("h", "help", _("Show this help"));
        struct arg_end *a_end = arg_end(20);

        void* argtable[] = {
            a_show_activity, a_daemonize, a_pid_file,
            a_enable_ble, a_ble_macs,
            a_serial_ports,
            a_mode, a_node_id, a_value,
            a_verbose,
            a_help, a_end
        };

        // verify the argtable[] entries were allocated successfully
        if (arg_nullcheck(argtable) != 0) {
            arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
            return false;
        }
        // Parse the command line as defined by argtable[]
        int errorCount = arg_parse(argc, argv, argtable);

        daemonize = a_daemonize->count > 0;
        if (a_pid_file->count)
            pidFile = *a_pid_file->sval;
        showActivity = a_show_activity->count > 0;
        enableBLE = a_enable_ble->count > 0 || a_ble_macs->count > 0;
        for (int i = 0; i < a_ble_macs->count; i++) {
            env.filterDevice.transportNDeviceNames.emplace_back( MTT_BLE, *a_ble_macs[i].sval );
        }
        enableSerial = a_serial_ports->count > 0;
        for (int i = 0; i < a_serial_ports->count; i++) {
            env.filterDevice.transportNDeviceNames.emplace_back( MTT_SERIAL, *a_serial_ports[i].sval );
        }

        if (!enableSerial && !enableBLE) {
            std::cerr << _("No BLE or serial interface selected. Enable BLE: -b") << std::endl;
            errorCount++;
        }

        if (a_mode->count > 0) {
            std::string m(*a_mode->sval);
            std::transform(m.begin(), m.end(), m.begin(), ::tolower);
            if (m == "nodes")
                mode = CM_LIST_NODES;
            if (m == "channels")
                mode = CM_LIST_CHANNELS;
            if (m == "sendtext")
                mode = CM_SEND_TEXT;
        }

        for (int i = 0; i < a_node_id->count; i++) {
            const char* p = *a_node_id[i].sval;
            if (p[0] == '!')    // at least terminal '\0' char must exist so do not check length
                p++;    // skip '!'
            uint32_t a = strtoul(p, nullptr, 16);
            nodeNums.push_back(a);
        }

        if (a_value->count)
            value = *a_value->sval;

        verbose = a_verbose->count;

        // special case: '--help' takes precedence over error reporting
        if ((a_help->count) || errorCount) {
            if (errorCount)
                arg_print_errors(stderr, a_end, programName);
            std::cout << _("meshtastic client") << "\n"
                << _("Usage: ") << programName << std::endl;
            arg_print_syntax(stdout, argtable, "\n");
            arg_print_glossary(stdout, argtable, "  %-27s %s\n");
            arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
            return false;
        }
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return true;
    }
};

static MeshtasticEnvNParams envArgs;

static int init() {
#ifdef _MSC_VER
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    return 0;
#endif
}

static void done() {
#ifdef _MSC_VER
    WSACleanup();
#endif
}

static void stop()
{
    if (envArgs.state != CRS_RUNNING)
        return;

    std::unique_lock<std::mutex> lck2(envArgs.mutexState);
    envArgs.state = CRS_STOP_REQUEST;
    envArgs.cvState.notify_all();
}

static void run()
{
    envArgs.state = CRS_RUNNING;
    envArgs.env.setDebugLog(envArgs.verbose, &std::cout);

    ConsolePinCodeProvider consolePinCodeProvider;
    envArgs.env.pinCodeProvider = &consolePinCodeProvider;


    if (envArgs.enableBLE) {
        envArgs.env.addTransport(&envArgs.bleTransport);
    }

    if (envArgs.enableSerial) {
        envArgs.env.addTransport(&envArgs.serialTransport);
    }

    MyEventHandler eh;
    envArgs.env.addEventHandler(&eh);

    uint32_t devicesConfigured = envArgs.env.waitDeviceConfigured(30);
    envArgs.env.stopDiscovery(MTT_ANY);

    if (devicesConfigured == 0) {
        std::cerr << _("No meshtastic device found") << std::endl;
        return;
    }
    if (envArgs.verbose) {
        std::cout << envArgs.env.count() << _(" device(s) found") << std::endl;
    }

    switch (envArgs.mode) {
        case CM_LIST_NODES:
            for (int i = 0; i < envArgs.env.count(); i++) {
                auto d = envArgs.env.get(i);
                if (!d)
                    continue;
                std::cout << d->name << ' ' << std::endl;
                for (auto &n : d->context.nodes) {
                    std::cout << std::hex << n.second.num();
                    if (n.second.has_user())
                        std::cout << ' '
                                  << n.second.user().short_name() << " ("
                                  << n.second.user().long_name() << ")";
                    std::cout << std::endl;
                }
            }
            break;
        case CM_LIST_CHANNELS:
            for (int i = 0; i < envArgs.env.count(); i++) {
                auto d = envArgs.env.get(i);
                if (!d)
                    continue;
                std::cout << d->name << ' ' << std::endl;
                for (auto &ch : d->context.channels) {
                    std::cout << ch.index() << ' '
                        << MeshtasticString::channelRole2String(ch.role());
                    if (ch.has_settings()) {
                        auto &s = ch.settings();
                        std::cout << ' ' << (MeshtasticString::psk2String(s.psk()));
                    }
                    std::cout << std::endl;
                }
            }
            break;
        case CM_SEND_TEXT:
            envArgs.env.sendString(envArgs.nodeNums, envArgs.value);
            break;
        default:
            break;
    }

    std::unique_lock<std::mutex> lock(envArgs.mutexState);
    envArgs.cvState.wait(lock, [] {
        return envArgs.state == CRS_STOP_REQUEST;
    });

    envArgs.state = CRS_STOPPED;
}

#define TRACE_BUFFER_SIZE   256

static void printTrace() {
#ifdef _MSC_VER
#else
    void *t[TRACE_BUFFER_SIZE];
    auto size = backtrace(t, TRACE_BUFFER_SIZE);
    backtrace_symbols_fd(t, size, STDERR_FILENO);
#endif
}

#ifndef _MSC_VER
void signalHandler(int signal)
{
    switch (signal) {
        case SIGINT:
            std::cerr << MSG_INTERRUPTED << std::endl;
            stop();
            done();
            std::cerr << MSG_GRACEFULLY_STOPPED << std::endl;
            exit(CODE_OK);
        case SIGSEGV:
            std::cerr << ERR_SEGMENTATION_FAULT << std::endl;
            printTrace();
            exit(ERR_CODE_SEGMENTATION_FAULT);
        case SIGABRT:
            std::cerr << ERR_ABRT << std::endl;
            printTrace();
            exit(ERR_CODE_ABRT);
        case SIGHUP:
            std::cerr << ERR_HANGUP_DETECTED << std::endl;
            break;
        case SIGUSR2:	// 12
            std::cerr << MSG_SIG_FLUSH_FILES << std::endl;
            // flushFiles();
            break;
        case 42:	// restart
            std::cerr << MSG_RESTART_REQUEST << std::endl;
            stop();
            done();
            run();
            break;
        default:
            break;
    }
}
#else
BOOL WINAPI winSignalHandler(DWORD signal) {
    std::cerr << _("Interrupted..") << std::endl;
    stop();
    done();
    return true;
}
#endif

void setSignalHandler()
{
#ifdef _MSC_VER
    SetConsoleCtrlHandler(winSignalHandler,  true);
#else
    struct sigaction action {};
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = &signalHandler;
    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGHUP, &action, nullptr);
    sigaction(SIGSEGV, &action, nullptr);
    sigaction(SIGABRT, &action, nullptr);
    sigaction(SIGUSR2, &action, nullptr);
    sigaction(42, &action, nullptr);
#endif
}

int main(int argc, char **argv) {
    int r = init();
    if (r)
        return r;

    if (!envArgs.parseArgs(argc, argv))
        return -1;

    if (envArgs.daemonize) {
        Daemonize daemonize(programName, getCurrentDir(), run, stop, done, 0, envArgs.pidFile);
    } else {
        if (envArgs.showActivity) {
            TDeviceListView lv(&envArgs.env);
            lv.view();
        } else {
            setSignalHandler();
            run();
        }
        done();
    }
    return 0;
}
