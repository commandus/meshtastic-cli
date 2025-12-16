#include <iomanip>
#include <sstream>

#ifdef _MSC_VER
#include <winrt/windows.devices.bluetooth.advertisement.h>
#endif

#include "MeshtasticString.h"
#include "meshtastic-magic-numbers.h"
#include "BLEGuid.h"

const char ADDR_DELIMITER = ':';
const char UUID_DELIMITER = '-';
const char NUMBER_FILL = '0';

static const char* GATT_ATTR_PROP_NAMES[] {
"Broadcast",
"Read",
"WriteWithoutResponse",
"Write",
"Notify",
"Indicate",
"AuthenticatedSignedWrites",
"ExtendedProperties",
"ReliableWrites",
"WritableAuxiliaries",
};

static const char* TRANSPORT_TYPE_NAMES[] {
    "",
    "BLE",
    "COM",
    "Wi-Fi"
};

static const char* commonDataTypes[] {
    "",
    "Flags",
    "Incomplete List of 16-bit Service or Service Class UUIDs",
    "Complete List of 16-bit Service or Service Class UUIDs",
    "Incomplete List of 32-bit Service or Service Class UUIDs",
    "Complete List of 32-bit Service or Service Class UUIDs",
    "Incomplete List of 128-bit Service or Service Class UUIDs",
    "Complete List of 128-bit Service or Service Class UUIDs",
    "Shortened Local Name",
    "Complete Local Name",
    "Tx Power Level",
    "",
    "Class of Device",
    "Simple Pairing Hash C-192",
    "Simple Pairing Randomizer R-192",
    "Device ID",
    "Security Manager TK Value",
    "Security Manager Out of Band Flags",
    "Peripheral Connection Interval Range",
    "",
    "List of 16-bit Service Solicitation UUIDs",
    "List of 128-bit Service Solicitation UUIDs",
    "Service Data - 16-bit UUID",
    "Public Target Address",
    "Random Target Address",
    "Appearance",
    "Advertising Interval",
    "LE Bluetooth Device Address",
    "LE Role",
    "Simple Pairing Hash C-256",
    "Simple Pairing Randomizer R-256",
    "List of 32-bit Service Solicitation UUIDs",
    "Service Data - 32-bit UUID",
    "Service Data - 128-bit UUID",
    "LE Secure Connections Confirmation Value",
    "LE Secure Connections Random Value",
    "URI",
    "Indoor Positioning",
    "Transport Discovery Data",
    "LE Supported Features",
    "Channel Map Update Indication",
    "PB-ADV",
    "Mesh Message",
    "Mesh Beacon",
    "BIGInfo",
    "Broadcast_Code",
    "Resolvable Set Identifier",
    "Advertising Interval - long",
    "Broadcast_Name",
    "Encrypted Advertising Data",
    "Periodic Advertising Response Timing Information",
    "",
    "Electronic Shelf Label",
    "3D Information Data",
    "Manufacturer Specific Data"    // 54 -> 0xff
};

static const std::map<uint16_t, const char*> characteristicsNames {
    { 0x0001, "SDP" },
    { 0x0003, "RFCOMM" },
    { 0x0005, "TCS-BIN" },
    { 0x0007, "ATT" },
    { 0x0008, "OBEX" },
    { 0x000f, "BNEP" },
    { 0x0010, "UPNP" },
    { 0x0011, "HIDP" },
    { 0x0012, "Hardcopy Control Channel" },
    { 0x0014, "Hardcopy Data Channel" },
    { 0x0016, "Hardcopy Notification" },
    { 0x0017, "AVCTP" },
    { 0x0019, "AVDTP" },
    { 0x001b, "CMTP" },
    { 0x001e, "MCAP Control Channel" },
    { 0x001f, "MCAP Data Channel" },
    { 0x0100, "L2CAP" },
    // 0x0101 to 0x0fff undefined
    { 0x1000, "Service Discovery Server Service Class" },
    { 0x1001, "Browse Group Descriptor Service Class" },
    { 0x1002, "Public Browse Root" },
    // 0x1003 to 0x1100 undefined
    { 0x1101, "Serial Port" },
    { 0x1102, "LAN Access Using PPP" },
    { 0x1103, "Dialup Networking" },
    { 0x1104, "IrMC Sync" },
    { 0x1105, "OBEX Object Push" },
    { 0x1106, "OBEX File Transfer" },
    { 0x1107, "IrMC Sync Command" },
    { 0x1108, "Headset" },
    { 0x1109, "Cordless Telephony" },
    { 0x110a, "Audio Source" },
    { 0x110b, "Audio Sink" },
    { 0x110c, "A/V Remote Control Target" },
    { 0x110d, "Advanced Audio Distribution" },
    { 0x110e, "A/V Remote Control" },
    { 0x110f, "A/V Remote Control Controller" },
    { 0x1110, "Intercom" },
    { 0x1111, "Fax" },
    { 0x1112, "Headset AG" },
    { 0x1113, "WAP" },
    { 0x1114, "WAP Client" },
    { 0x1115, "PANU" },
    { 0x1116, "NAP" },
    { 0x1117, "GN" },
    { 0x1118, "Direct Printing" },
    { 0x1119, "Reference Printing" },
    { 0x111a, "Basic Imaging Profile" },
    { 0x111b, "Imaging Responder" },
    { 0x111c, "Imaging Automatic Archive" },
    { 0x111d, "Imaging Referenced Objects" },
    { 0x111e, "Handsfree" },
    { 0x111f, "Handsfree Audio Gateway" },
    { 0x1120, "Direct Printing Refrence Objects Service" },
    { 0x1121, "Reflected UI" },
    { 0x1122, "Basic Printing" },
    { 0x1123, "Printing Status" },
    { 0x1124, "Human Interface Device Service" },
    { 0x1125, "Hardcopy Cable Replacement" },
    { 0x1126, "HCR Print" },
    { 0x1127, "HCR Scan" },
    { 0x1128, "Common ISDN Access" },
    // 0x1129 to 0x112a undefined
    { 0x112d, "SIM Access" },
    { 0x112e, "Phonebook Access Client" },
    { 0x112f, "Phonebook Access Server" },
    { 0x1130, "Phonebook Access" },
    { 0x1131, "Headset HS" },
    { 0x1132, "Message Access Server" },
    { 0x1133, "Message Notification Server" },
    { 0x1134, "Message Access Profile" },
    { 0x1135, "GNSS" },
    { 0x1136, "GNSS Server" },
    { 0x1137, "3D Display" },
    { 0x1138, "3D Glasses" },
    { 0x1139, "3D Synchronization" },
    { 0x113a, "MPS Profile" },
    { 0x113b, "MPS Service" },
    { 0x113c, "CTN Access Service" },
    { 0x113d, "CTN Notification Service" },
    { 0x113e, "CTN Profile" },
    // 0x113f to 0x11ff undefined
    { 0x1200, "PnP Information" },
    { 0x1201, "Generic Networking" },
    { 0x1202, "Generic File Transfer" },
    { 0x1203, "Generic Audio" },
    { 0x1204, "Generic Telephony" },
    { 0x1205, "UPNP Service" },
    { 0x1206, "UPNP IP Service" },
    { 0x1300, "UPNP IP PAN" },
    { 0x1301, "UPNP IP LAP" },
    { 0x1302, "UPNP IP L2CAP" },
    { 0x1303, "Video Source" },
    { 0x1304, "Video Sink" },
    { 0x1305, "Video Distribution" },
    // 0x1306 to 0x13ff undefined
    { 0x1400, "HDP" },
    { 0x1401, "HDP Source" },
    { 0x1402, "HDP Sink" },
    // 0x1403 to 0x17ff undefined
    { 0x1800, "Generic Access Profile" },
    { 0x1801, "Generic Attribute Profile" },
    { 0x1802, "Immediate Alert" },
    { 0x1803, "Link Loss" },
    { 0x1804, "Tx Power" },
    { 0x1805, "Current Time Service" },
    { 0x1806, "Reference Time Update Service" },
    { 0x1807, "Next DST Change Service" },
    { 0x1808, "Glucose" },
    { 0x1809, "Health Thermometer" },
    { 0x180a, "Device Information" },
    // 0x180b to 0x180c undefined
    { 0x180d, "Heart Rate" },
    { 0x180e, "Phone Alert Status Service" },
    { 0x180f, "Battery Service" },
    { 0x1810, "Blood Pressure" },
    { 0x1811, "Alert Notification Service" },
    { 0x1812, "Human Interface Device" },
    { 0x1813, "Scan Parameters" },
    { 0x1814, "Running Speed and Cadence" },
    { 0x1815, "Automation IO" },
    { 0x1816, "Cycling Speed and Cadence" },
    // 0x1817 undefined
    { 0x1818, "Cycling Power" },
    { 0x1819, "Location and Navigation" },
    { 0x181a, "Environmental Sensing" },
    { 0x181b, "Body Composition" },
    { 0x181c, "User Data" },
    { 0x181d, "Weight Scale" },
    { 0x181e, "Bond Management" },
    { 0x181f, "Continuous Glucose Monitoring" },
    { 0x1820, "Internet Protocol Support" },
    { 0x1821, "Indoor Positioning" },
    { 0x1822, "Pulse Oximeter" },
    { 0x1823, "HTTP Proxy" },
    { 0x1824, "Transport Discovery" },
    { 0x1825, "Object Transfer" },
    { 0x1826, "Fitness Machine" },
    { 0x1827, "Mesh Provisioning" },
    { 0x1828, "Mesh Proxy" },
    { 0x1829, "Reconnection Configuration" },
    // 0x182a to 0x1839 undefined
    { 0x183a, "Insulin Delivery" },
    { 0x183b, "Binary Sensor" },
    { 0x183c, "Emergency Configuration" },
    { 0x183d, "Authorization Control" },
    { 0x183e, "Physical Activity Monitor" },
    { 0x183f, "Elapsed Time" },
    { 0x1840, "Generic Health Sensor" },
    { 0x1843, "Audio Input Control" },
    { 0x1844, "Volume Control" },
    { 0x1845, "Volume Offset Control" },
    { 0x1846, "Coordinated Set Identification Service" },
    { 0x1847, "Device Time" },
    { 0x1848, "Media Control Service" },
    { 0x1849, "Generic Media Control Service" },
    { 0x184a, "Constant Tone Extension" },
    { 0x184b, "Telephone Bearer Service" },
    { 0x184c, "Generic Telephone Bearer Service" },
    { 0x184d, "Microphone Control" },
    { 0x184e, "Audio Stream Control Service" },
    { 0x184f, "Broadcast Audio Scan Service" },
    { 0x1850, "Published Audio Capabilities Service" },
    { 0x1851, "Basic Audio Announcement Service" },
    { 0x1852, "Broadcast Audio Announcement Service" },
    { 0x1853, "Common Audio" },
    { 0x1854, "Hearing Access" },
    { 0x1855, "Telephony and Media Audio" },
    { 0x1856, "Public Broadcast Announcement" },
    { 0x1857, "Electronic Shelf Label" },
    { 0x1859, "Mesh Proxy Solicitation" },
    // 0x185A to 0x26ff undefined
    // 0x2700  GATT Units
    { 0x2700, "unitless" },
    { 0x2701, "length (metre)" },
    { 0x2702, "mass (kilogram)" },
    { 0x2703, "time (second)" },
    { 0x2704, "electric current (ampere)" },
    { 0x2705, "thermodynamic temperature (kelvin)" },
    { 0x2706, "amount of substance (mole)" },
    { 0x2707, "luminous intensity (candela)" },
    { 0x2710, "area (square metres)" },
    { 0x2711, "volume (cubic metres)" },
    { 0x2712, "velocity (metres per second)" },
    { 0x2713, "acceleration (metres per second squared)" },
    { 0x2714, "wavenumber (reciprocal metre)" },
    { 0x2715, "density (kilogram per cubic metre)" },
    { 0x2716, "surface density (kilogram per square metre)" },
    { 0x2717, "specific volume (cubic metre per kilogram)" },
    { 0x2718, "current density (ampere per square metre)" },
    { 0x2719, "magnetic field strength (ampere per metre)" },
    { 0x271a, "amount concentration (mole per cubic metre)" },
    { 0x271b, "mass concentration (kilogram per cubic metre)" },
    { 0x271c, "luminance (candela per square metre)" },
    { 0x271d, "refractive index" },
    { 0x271e, "relative permeability" },
    { 0x2720, "plane angle (radian)" },
    { 0x2721, "solid angle (steradian)" },
    { 0x2722, "frequency (hertz)" },
    { 0x2723, "force (newton)" },
    { 0x2724, "pressure (pascal)" },
    { 0x2725, "energy (joule)" },
    { 0x2726, "power (watt)" },
    { 0x2727, "electric charge (coulomb)" },
    { 0x2728, "electric potential difference (volt)" },
    { 0x2729, "capacitance (farad)" },
    { 0x272a, "electric resistance (ohm)" },
    { 0x272b, "electric conductance (siemens)" },
    { 0x272c, "magnetic flux (weber)" },
    { 0x272d, "magnetic flux density (tesla)" },
    { 0x272e, "inductance (henry)" },
    { 0x272f, "Celsius temperature (degree Celsius)" },
    { 0x2730, "luminous flux (lumen)" },
    { 0x2731, "illuminance (lux)" },
    { 0x2732, "activity referred to a radionuclide (becquerel)" },
    { 0x2733, "absorbed dose (gray)" },
    { 0x2734, "dose equivalent (sievert)" },
    { 0x2735, "catalytic activity (katal)" },
    { 0x2740, "dynamic viscosity (pascal second)" },
    { 0x2741, "moment of force (newton metre)" },
    { 0x2742, "surface tension (newton per metre)" },
    { 0x2743, "angular velocity (radian per second)" },
    { 0x2744, "angular acceleration (radian per second squared)" },
    { 0x2745, "heat flux density (watt per square metre)" },
    { 0x2746, "heat capacity (joule per kelvin)" },
    { 0x2747, "specific heat capacity (joule per kilogram kelvin)" },
    { 0x2748, "specific energy (joule per kilogram)" },
    { 0x2749, "thermal conductivity (watt per metre kelvin)" },
    { 0x274a, "energy density (joule per cubic metre)" },
    { 0x274b, "electric field strength (volt per metre)" },
    { 0x274c, "electric charge density (coulomb per cubic metre)" },
    { 0x274d, "surface charge density (coulomb per square metre)" },
    { 0x274e, "electric flux density (coulomb per square metre)" },
    { 0x274f, "permittivity (farad per metre)" },
    { 0x2750, "permeability (henry per metre)" },
    { 0x2751, "molar energy (joule per mole)" },
    { 0x2752, "molar entropy (joule per mole kelvin)" },
    { 0x2753, "exposure (coulomb per kilogram)" },
    { 0x2754, "absorbed dose rate (gray per second)" },
    { 0x2755, "radiant intensity (watt per steradian)" },
    { 0x2756, "radiance (watt per square metre steradian)" },
    { 0x2757, "catalytic activity concentration (katal per cubic metre)" },
    { 0x2760, "time (minute)" },
    { 0x2761, "time (hour)" },
    { 0x2762, "time (day)" },
    { 0x2763, "plane angle (degree)" },
    { 0x2764, "plane angle (minute)" },
    { 0x2765, "plane angle (second)" },
    { 0x2766, "area (hectare)" },
    { 0x2767, "volume (litre)" },
    { 0x2768, "mass (tonne)" },
    { 0x2780, "pressure (bar)" },
    { 0x2781, "pressure (millimetre of mercury)" },
    { 0x2782, "length (ångström)" },
    { 0x2783, "length (nautical mile)" },
    { 0x2784, "area (barn)" },
    { 0x2785, "velocity (knot)" },
    { 0x2786, "logarithmic radio quantity (neper)" },
    { 0x2787, "logarithmic radio quantity (bel)" },
    { 0x27a0, "length (yard)" },
    { 0x27a1, "length (parsec)" },
    { 0x27a2, "length (inch)" },
    { 0x27a3, "length (foot)" },
    { 0x27a4, "length (mile)" },
    { 0x27a5, "pressure (pound-force per square inch)" },
    { 0x27a6, "velocity (kilometre per hour)" },
    { 0x27a7, "velocity (mile per hour)" },
    { 0x27a8, "angular velocity (revolution per minute)" },
    { 0x27a9, "energy (gram calorie)" },
    { 0x27aa, "energy (kilogram calorie)" },
    { 0x27ab, "energy (kilowatt hour)" },
    { 0x27ac, "thermodynamic temperature (degree Fahrenheit)" },
    { 0x27ad, "percentage" },
    { 0x27ae, "per mille" },
    { 0x27af, "period (beats per minute)" },
    { 0x27b0, "electric charge (ampere hours)" },
    { 0x27b1, "mass density (milligram per decilitre)" },
    { 0x27b2, "mass density (millimole per litre)" },
    { 0x27b3, "time (year)" },
    { 0x27b4, "time (month)" },
    { 0x27b5, "concentration (count per cubic metre)" },
    { 0x27b6, "irradiance (watt per square metre)" },
    { 0x27b7, "milliliter (per kilogram per minute)" },
    { 0x27b8, "mass (pound)" },
    { 0x27b9, "metabolic equivalent" },
    { 0x27ba, "step (per minute)" },
    { 0x27bc, "stroke (per minute)" },
    { 0x27bd, "pace (kilometre per minute)" },
    { 0x27be, "luminous efficacy (lumen per watt)" },
    { 0x27bf, "luminous energy (lumen hour)" },
    { 0x27c0, "luminous exposure (lux hour)" },
    { 0x27c1, "mass flow (gram per second)" },
    { 0x27c2, "volume flow (litre per second)" },
    { 0x27c3, "sound pressure (decible)" },
    { 0x27c4, "parts per million" },
    { 0x27c5, "parts per billion" },
    { 0x2800, "Primary Service" },
    { 0x2801, "Secondary Service" },
    { 0x2802, "Include" },
    { 0x2803, "Characteristic" },
    // 0x2804 to 0x28ff undefined
    // descriptors (SIG)
    { 0x2900, "Characteristic Extended Properties" },
    { 0x2901, "Characteristic User Description" },
    { 0x2902, "Client Characteristic Configuration" },
    { 0x2903, "Server Characteristic Configuration" },
    { 0x2904, "Characteristic Presentation Format" },
    { 0x2905, "Characteristic Aggregate Format" },
    { 0x2906, "Valid Range" },
    { 0x2907, "External Report Reference" },
    { 0x2908, "Report Reference" },
    { 0x2909, "Number of Digitals" },
    { 0x290a, "Value Trigger Setting" },
    { 0x290b, "Environmental Sensing Configuration" },
    { 0x290c, "Environmental Sensing Measurement" },
    { 0x290d, "Environmental Sensing Trigger Setting" },
    { 0x290e, "Time Trigger Setting" },
    { 0x290f, "Complete BR-EDR Transport Block Data" },
    // 0x2910 to 0x29ff undefined
    // 0x2a00.. GATT characteristic and Object Types
    { 0x2a00, "Device Name" },
    { 0x2a01, "Appearance" },
    { 0x2a02, "Peripheral Privacy Flag" },
    { 0x2a03, "Reconnection Address" },
    { 0x2a04, "Peripheral Preferred Connection Parameters" },
    { 0x2a05, "Service Changed" },
    { 0x2a06, "Alert Level" },
    { 0x2a07, "Tx Power Level" },
    { 0x2a08, "Date Time" },
    { 0x2a09, "Day of Week" },
    { 0x2a0a, "Day Date Time" },
    { 0x2a0b, "Exact Time 100" },
    { 0x2a0c, "Exact Time 256" },
    { 0x2a0d, "DST Offset" },
    { 0x2a0e, "Time Zone" },
    { 0x2a0f, "Local Time Information" },
    { 0x2a10, "Secondary Time Zone" },
    { 0x2a11, "Time with DST" },
    { 0x2a12, "Time Accuracy" },
    { 0x2a13, "Time Source" },
    { 0x2a14, "Reference Time Information" },
    { 0x2a15, "Time Broadcast" },
    { 0x2a16, "Time Update Control Point" },
    { 0x2a17, "Time Update State" },
    { 0x2a18, "Glucose Measurement" },
    { 0x2a19, "Battery Level" },
    { 0x2a1a, "Battery Power State" },
    { 0x2a1b, "Battery Level State" },
    { 0x2a1c, "Temperature Measurement" },
    { 0x2a1d, "Temperature Type" },
    { 0x2a1e, "Intermediate Temperature" },
    { 0x2a1f, "Temperature Celsius" },
    { 0x2a20, "Temperature Fahrenheit" },
    { 0x2a21, "Measurement Interval" },
    { 0x2a22, "Boot Keyboard Input Report" },
    { 0x2a23, "System ID" },
    { 0x2a24, "Model Number String" },
    { 0x2a25, "Serial Number String" },
    { 0x2a26, "Firmware Revision String" },
    { 0x2a27, "Hardware Revision String" },
    { 0x2a28, "Software Revision String" },
    { 0x2a29, "Manufacturer Name String" },
    { 0x2a2a, "IEEE 11073-20601 Regulatory Cert. Data List" },
    { 0x2a2b, "Current Time" },
    { 0x2a2c, "Magnetic Declination" },
    // 0x2a2d to 0x2a2e undefined
    { 0x2a2f, "Position 2D" },
    { 0x2a30, "Position 3D" },
    { 0x2a31, "Scan Refresh" },
    { 0x2a32, "Boot Keyboard Output Report" },
    { 0x2a33, "Boot Mouse Input Report" },
    { 0x2a34, "Glucose Measurement Context" },
    { 0x2a35, "Blood Pressure Measurement" },
    { 0x2a36, "Intermediate Cuff Pressure" },
    { 0x2a37, "Heart Rate Measurement" },
    { 0x2a38, "Body Sensor Location" },
    { 0x2a39, "Heart Rate Control Point" },
    { 0x2a3a, "Removable" },
    { 0x2a3b, "Service Required" },
    { 0x2a3c, "Scientific Temperature Celsius" },
    { 0x2a3d, "String" },
    { 0x2a3e, "Network Availability" },
    { 0x2a3f, "Alert Status" },
    { 0x2a40, "Ringer Control Point" },
    { 0x2a41, "Ringer Setting" },
    { 0x2a42, "Alert Category ID Bit Mask" },
    { 0x2a43, "Alert Category ID" },
    { 0x2a44, "Alert Notification Control Point" },
    { 0x2a45, "Unread Alert Status" },
    { 0x2a46, "New Alert" },
    { 0x2a47, "Supported New Alert Category" },
    { 0x2a48, "Supported Unread Alert Category" },
    { 0x2a49, "Blood Pressure Feature" },
    { 0x2a4a, "HID Information" },
    { 0x2a4b, "Report Map" },
    { 0x2a4c, "HID Control Point" },
    { 0x2a4d, "Report" },
    { 0x2a4e, "Protocol Mode" },
    { 0x2a4f, "Scan Interval Window" },
    { 0x2a50, "PnP ID" },
    { 0x2a51, "Glucose Feature" },
    { 0x2a52, "Record Access Control Point" },
    { 0x2a53, "RSC Measurement" },
    { 0x2a54, "RSC Feature" },
    { 0x2a55, "SC Control Point" },
    { 0x2a56, "Digital" },
    { 0x2a57, "Digital Output" },
    { 0x2a58, "Analog" },
    { 0x2a59, "Analog Output" },
    { 0x2a5a, "Aggregate" },
    { 0x2a5b, "CSC Measurement" },
    { 0x2a5c, "CSC Feature" },
    { 0x2a5d, "Sensor Location" },
    { 0x2a5e, "PLX Spot-Check Measurement" },
    { 0x2a5f, "PLX Continuous Measurement Characteristic" },
    { 0x2a60, "PLX Features" },
    { 0x2a62, "Pulse Oximetry Control Point" },
    { 0x2a63, "Cycling Power Measurement" },
    { 0x2a64, "Cycling Power Vector" },
    { 0x2a65, "Cycling Power Feature" },
    { 0x2a66, "Cycling Power Control Point" },
    { 0x2a67, "Location and Speed" },
    { 0x2a68, "Navigation" },
    { 0x2a69, "Position Quality" },
    { 0x2a6a, "LN Feature" },
    { 0x2a6b, "LN Control Point" },
    { 0x2a6c, "Elevation" },
    { 0x2a6d, "Pressure" },
    { 0x2a6e, "Temperature" },
    { 0x2a6f, "Humidity" },
    { 0x2a70, "True Wind Speed" },
    { 0x2a71, "True Wind Direction" },
    { 0x2a72, "Apparent Wind Speed" },
    { 0x2a73, "Apparent Wind Direction" },
    { 0x2a74, "Gust Factor" },
    { 0x2a75, "Pollen Concentration" },
    { 0x2a76, "UV Index" },
    { 0x2a77, "Irradiance" },
    { 0x2a78, "Rainfall" },
    { 0x2a79, "Wind Chill" },
    { 0x2a7a, "Heat Index" },
    { 0x2a7b, "Dew Point" },
    { 0x2a7c, "Trend" },
    { 0x2a7d, "Descriptor Value Changed" },
    { 0x2a7e, "Aerobic Heart Rate Lower Limit" },
    { 0x2a7f, "Aerobic Threshold" },
    { 0x2a80, "Age" },
    { 0x2a81, "Anaerobic Heart Rate Lower Limit" },
    { 0x2a82, "Anaerobic Heart Rate Upper Limit" },
    { 0x2a83, "Anaerobic Threshold" },
    { 0x2a84, "Aerobic Heart Rate Upper Limit" },
    { 0x2a85, "Date of Birth" },
    { 0x2a86, "Date of Threshold Assessment" },
    { 0x2a87, "Email Address" },
    { 0x2a88, "Fat Burn Heart Rate Lower Limit" },
    { 0x2a89, "Fat Burn Heart Rate Upper Limit" },
    { 0x2a8a, "First Name" },
    { 0x2a8b, "Five Zone Heart Rate Limits" },
    { 0x2a8c, "Gender" },
    { 0x2a8d, "Heart Rate Max" },
    { 0x2a8e, "Height" },
    { 0x2a8f, "Hip Circumference" },
    { 0x2a90, "Last Name" },
    { 0x2a91, "Maximum Recommended Heart Rate" },
    { 0x2a92, "Resting Heart Rate" },
    { 0x2a93, "Sport Type for Aerobic/Anaerobic Thresholds" },
    { 0x2a94, "Three Zone Heart Rate Limits" },
    { 0x2a95, "Two Zone Heart Rate Limit" },
    { 0x2a96, "VO2 Max" },
    { 0x2a97, "Waist Circumference" },
    { 0x2a98, "Weight" },
    { 0x2a99, "Database Change Increment" },
    { 0x2a9a, "User Index" },
    { 0x2a9b, "Body Composition Feature" },
    { 0x2a9c, "Body Composition Measurement" },
    { 0x2a9d, "Weight Measurement" },
    { 0x2a9e, "Weight Scale Feature" },
    { 0x2a9f, "User Control Point" },
    { 0x2aa0, "Magnetic Flux Density - 2D" },
    { 0x2aa1, "Magnetic Flux Density - 3D" },
    { 0x2aa2, "Language" },
    { 0x2aa3, "Barometric Pressure Trend" },
    { 0x2aa4, "Bond Management Control Point" },
    { 0x2aa5, "Bond Management Feature" },
    { 0x2aa6, "Central Address Resolution" },
    { 0x2aa7, "CGM Measurement" },
    { 0x2aa8, "CGM Feature" },
    { 0x2aa9, "CGM Status" },
    { 0x2aaa, "CGM Session Start Time" },
    { 0x2aab, "CGM Session Run Time" },
    { 0x2aac, "CGM Specific Ops Control Point" },
    { 0x2aad, "Indoor Positioning Configuration" },
    { 0x2aae, "Latitude" },
    { 0x2aaf, "Longitude" },
    { 0x2ab0, "Local North Coordinate" },
    { 0x2ab1, "Local East Coordinate" },
    { 0x2ab2, "Floor Number" },
    { 0x2ab3, "Altitude" },
    { 0x2ab4, "Uncertainty" },
    { 0x2ab5, "Location Name" },
    { 0x2ab6, "URI" },
    { 0x2ab7, "HTTP Headers" },
    { 0x2ab8, "HTTP Status Code" },
    { 0x2ab9, "HTTP Entity Body" },
    { 0x2aba, "HTTP Control Point" },
    { 0x2abb, "HTTPS Security" },
    { 0x2abc, "TDS Control Point" },
    { 0x2abd, "OTS Feature" },
    { 0x2abe, "Object Name" },
    { 0x2abf, "Object Type" },
    { 0x2ac0, "Object Size" },
    { 0x2ac1, "Object First-Created" },
    { 0x2ac2, "Object Last-Modified" },
    { 0x2ac3, "Object ID" },
    { 0x2ac4, "Object Properties" },
    { 0x2ac5, "Object Action Control Point" },
    { 0x2ac6, "Object List Control Point" },
    { 0x2ac7, "Object List Filter" },
    { 0x2ac8, "Object Changed" },
    { 0x2ac9, "Resolvable Private Address Only" },
    // 0x2aca to 0x2acb undefined
    { 0x2acc, "Fitness Machine Feature" },
    { 0x2acd, "Treadmill Data" },
    { 0x2ace, "Cross Trainer Data" },
    { 0x2acf, "Step Climber Data" },
    { 0x2ad0, "Stair Climber Data" },
    { 0x2ad1, "Rower Data" },
    { 0x2ad2, "Indoor Bike Data" },
    { 0x2ad3, "Training Status" },
    { 0x2ad4, "Supported Speed Range" },
    { 0x2ad5, "Supported Inclination Range" },
    { 0x2ad6, "Supported Resistance Level Range" },
    { 0x2ad7, "Supported Heart Rate Range" },
    { 0x2ad8, "Supported Power Range" },
    { 0x2ad9, "Fitness Machine Control Point" },
    { 0x2ada, "Fitness Machine Status" },
    { 0x2adb, "Mesh Provisioning Data In" },
    { 0x2adc, "Mesh Provisioning Data Out" },
    { 0x2add, "Mesh Proxy Data In" },
    { 0x2ade, "Mesh Proxy Data Out" },
    { 0x2ae0, "Average Current" },
    { 0x2ae1, "Average Voltage" },
    { 0x2ae2, "Boolean" },
    { 0x2ae3, "Chromatic Distance From Planckian" },
    { 0x2ae4, "Chromaticity Coordinates" },
    { 0x2ae5, "Chromaticity In CCT And Duv Values" },
    { 0x2ae6, "Chromaticity Tolerance" },
    { 0x2ae7, "CIE 13.3-1995 Color Rendering Index" },
    { 0x2ae8, "Coefficient" },
    { 0x2ae9, "Correlated Color Temperature" },
    { 0x2aea, "Count 16" },
    { 0x2aeb, "Count 24" },
    { 0x2aec, "Country Code" },
    { 0x2aed, "Date UTC" },
    { 0x2aee, "Electric Current" },
    { 0x2aef, "Electric Current Range" },
    { 0x2af0, "Electric Current Specification" },
    { 0x2af1, "Electric Current Statistics" },
    { 0x2af2, "Energy" },
    { 0x2af3, "Energy In A Period Of Day" },
    { 0x2af4, "Event Statistics" },
    { 0x2af5, "Fixed String 16" },
    { 0x2af6, "Fixed String 24" },
    { 0x2af7, "Fixed String 36" },
    { 0x2af8, "Fixed String 8" },
    { 0x2af9, "Generic Level" },
    { 0x2afa, "Global Trade Item Number" },
    { 0x2afb, "Illuminance" },
    { 0x2afc, "Luminous Efficacy" },
    { 0x2afd, "Luminous Energy" },
    { 0x2afe, "Luminous Exposure" },
    { 0x2aff, "Luminous Flux" },
    { 0x2b00, "Luminous Flux Range" },
    { 0x2b01, "Luminous Intensity" },
    { 0x2b02, "Mass Flow" },
    { 0x2b03, "Perceived Lightness" },
    { 0x2b04, "Percentage 8" },
    { 0x2b05, "Power" },
    { 0x2b06, "Power Specification" },
    { 0x2b07, "Relative Runtime In A Current Range" },
    { 0x2b08, "Relative Runtime In A Generic Level Range" },
    { 0x2b09, "Relative Value In A Voltage Range" },
    { 0x2b0a, "Relative Value In An Illuminance Range" },
    { 0x2b0b, "Relative Value In A Period of Day" },
    { 0x2b0c, "Relative Value In A Temperature Range" },
    { 0x2b0d, "Temperature 8" },
    { 0x2b0e, "Temperature 8 In A Period Of Day" },
    { 0x2b0f, "Temperature 8 Statistics" },
    { 0x2b10, "Temperature Range" },
    { 0x2b11, "Temperature Statistics" },
    { 0x2b12, "Time Decihour 8" },
    { 0x2b13, "Time Exponential 8" },
    { 0x2b14, "Time Hour 24" },
    { 0x2b15, "Time Millisecond 24" },
    { 0x2b16, "Time Second 16" },
    { 0x2b17, "Time Second 8" },
    { 0x2b18, "Voltage" },
    { 0x2b19, "Voltage Specification" },
    { 0x2b1a, "Voltage Statistics" },
    { 0x2b1b, "Volume Flow" },
    { 0x2b1c, "Chromaticity Coordinate" },
    { 0x2b1d, "RC Feature" },
    { 0x2b1e, "RC Settings" },
    { 0x2b1f, "Reconnection Configuration Control Point" },
    { 0x2b20, "IDD Status Changed" },
    { 0x2b21, "IDD Status" },
    { 0x2b22, "IDD Annunciation Status" },
    { 0x2b23, "IDD Features" },
    { 0x2b24, "IDD Status Reader Control Point" },
    { 0x2b25, "IDD Command Control Point" },
    { 0x2b26, "IDD Command Data" },
    { 0x2b27, "IDD Record Access Control Point" },
    { 0x2b28, "IDD History Data" },
    { 0x2b29, "Client Supported Features" },
    { 0x2b2a, "Database Hash" },
    { 0x2b2b, "BSS Control Point" },
    { 0x2b2c, "BSS Response" },
    { 0x2b2d, "Emergency ID" },
    { 0x2b2e, "Emergency Text" },
    { 0x2b2f, "ACS Status" },
    { 0x2b30, "ACS Data In" },
    { 0x2b31, "ACS Data Out Notify" },
    { 0x2b32, "ACS Data Out Indicate" },
    { 0x2b33, "ACS Control Point" },
    { 0x2b34, "Enhanced Blood Pressure Measurement" },
    { 0x2b35, "Enhanced Intermediate Cuff Pressure" },
    { 0x2b36, "Blood Pressure Record" },
    { 0x2b37, "Registered User" },
    { 0x2b38, "BR-EDR Handover Data" },
    { 0x2b39, "Bluetooth SIG Data" },
    { 0x2b3a, "Server Supported Features" },
    { 0x2b3b, "Physical Activity Monitor Features" },
    { 0x2b3c, "General Activity Instantaneous Data" },
    { 0x2b3d, "General Activity Summary Data" },
    { 0x2b3e, "CardioRespiratory Activity Instantaneous Data" },
    { 0x2b3f, "CardioRespiratory Activity Summary Data" },
    { 0x2b40, "Step Counter Activity Summary Data" },
    { 0x2b41, "Sleep Activity Instantaneous Data" },
    { 0x2b42, "Sleep Activity Summary Data" },
    { 0x2b43, "Physical Activity Monitor Control Point" },
    { 0x2b44, "Current Session" },
    { 0x2b45, "Session" },
    { 0x2b46, "Preferred Units" },
    { 0x2b47, "High Resolution Height" },
    { 0x2b48, "Middle Name" },
    { 0x2b49, "Stride Length" },
    { 0x2b4a, "Handedness" },
    { 0x2b4b, "Device Wearing Position" },
    { 0x2b4c, "Four Zone Heart Rate Limits" },
    { 0x2b4d, "High Intensity Exercise Threshold" },
    { 0x2b4e, "Activity Goal" },
    { 0x2b4f, "Sedentary Interval Notification" },
    { 0x2b50, "Caloric Intake" },
    { 0x2b51, "TMAP Role" },
    { 0x2b77, "Audio Input State" },
    { 0x2b78, "Gain Settings Attribute" },
    { 0x2b79, "Audio Input Type" },
    { 0x2b7a, "Audio Input Status" },
    { 0x2b7b, "Audio Input Control Point" },
    { 0x2b7c, "Audio Input Description" },
    { 0x2b7d, "Volume State" },
    { 0x2b7e, "Volume Control Point" },
    { 0x2b7f, "Volume Flags" },
    { 0x2b80, "Offset State" },
    { 0x2b81, "Audio Location" },
    { 0x2b82, "Volume Offset Control Point" },
    { 0x2b83, "Audio Output Description" },
    { 0x2b84, "Set Identity Resolving Key Characteristic" },
    { 0x2b85, "Size Characteristic" },
    { 0x2b86, "Lock Characteristic" },
    { 0x2b87, "Rank Characteristic" },
    { 0x2b88, "Encrypted Data Key Material" },
    { 0x2b89, "Apparent Energy 32" },
    { 0x2b8a, "Apparent Power" },
    { 0x2b8b, "Live Health Observations" },
    { 0x2b8c, "CO\textsubscript{2} Concentration" },
    { 0x2b8d, "Cosine of the Angle" },
    { 0x2b8e, "Device Time Feature" },
    { 0x2b8f, "Device Time Parameters" },
    { 0x2b90, "Device Time" },
    { 0x2b91, "Device Time Control Point" },
    { 0x2b92, "Time Change Log Data" },
    { 0x2b93, "Media Player Name" },
    { 0x2b94, "Media Player Icon Object ID" },
    { 0x2b95, "Media Player Icon URL" },
    { 0x2b96, "Track Changed" },
    { 0x2b97, "Track Title" },
    { 0x2b98, "Track Duration" },
    { 0x2b99, "Track Position" },
    { 0x2b9a, "Playback Speed" },
    { 0x2b9b, "Seeking Speed" },
    { 0x2b9c, "Current Track Segments Object ID" },
    { 0x2b9d, "Current Track Object ID" },
    { 0x2b9e, "Next Track Object ID" },
    { 0x2b9f, "Parent Group Object ID" },
    { 0x2ba0, "Current Group Object ID" },
    { 0x2ba1, "Playing Order" },
    { 0x2ba2, "Playing Orders Supported" },
    { 0x2ba3, "Media State" },
    { 0x2ba4, "Media Control Point" },
    { 0x2ba5, "Media Control Point Opcodes Supported" },
    { 0x2ba6, "Search Results Object ID" },
    { 0x2ba7, "Search Control Point" },
    { 0x2ba8, "Energy 32" },
    { 0x2ba9, "Media Player Icon Object Type" },
    { 0x2baa, "Track Segments Object Type" },
    { 0x2bab, "Track Object Type" },
    { 0x2bac, "Group Object Type" },
    { 0x2bad, "Constant Tone Extension Enable" },
    { 0x2bae, "Advertising Constant Tone Extension Minimum Length" },
    { 0x2baf, "Advertising Constant Tone Extension Minimum Transmit Count" },
    { 0x2bb0, "Advertising Constant Tone Extension Transmit Duration" },
    { 0x2bb1, "Advertising Constant Tone Extension Interval" },
    { 0x2bb2, "Advertising Constant Tone Extension PHY" },
    { 0x2bb3, "Bearer Provider Name" },
    { 0x2bb4, "Bearer UCI" },
    { 0x2bb5, "Bearer Technology" },
    { 0x2bb6, "Bearer URI Schemes Supported List" },
    { 0x2bb7, "Bearer Signal Strength" },
    { 0x2bb8, "Bearer Signal Strength Reporting Interval" },
    { 0x2bb9, "Bearer List Current Calls" },
    { 0x2bba, "Content Control ID" },
    { 0x2bbb, "Status Flags" },
    { 0x2bbc, "Incoming Call Target Bearer URI" },
    { 0x2bbd, "Call State" },
    { 0x2bbe, "Call Control Point" },
    { 0x2bbf, "Call Control Point Optional Opcodes" },
    { 0x2bc0, "Termination Reason" },
    { 0x2bc1, "Incoming Call" },
    { 0x2bc2, "Call Friendly Name" },
    { 0x2bc3, "Mute" },
    { 0x2bc4, "Sink ASE" },
    { 0x2bc5, "Source ASE" },
    { 0x2bc6, "ASE Control Point" },
    { 0x2bc7, "Broadcast Audio Scan Control Point" },
    { 0x2bc8, "Broadcast Receive State" },
    { 0x2bc9, "Sink PAC" },
    { 0x2bca, "Sink Audio Locations" },
    { 0x2bcb, "Source PAC" },
    { 0x2bcc, "Source Audio Locations" },
    { 0x2bcd, "Available Audio Contexts" },
    { 0x2bce, "Supported Audio Contexts" },
    { 0x2bcf, "Ammonia Concentration" },
    { 0x2bd0, "Carbon Monoxide Concentration" },
    { 0x2bd1, "Methane Concentration" },
    { 0x2bd2, "Nitrogen Dioxide Concentration" },
    { 0x2bd3, "Non-Methane Volatile Organic Compounds Concentration" },
    { 0x2bd4, "Ozone Concentration" },
    { 0x2bd5, "Particulate Matter - PM1 Concentration" },
    { 0x2bd6, "Particulate Matter - PM2.5 Concentration" },
    { 0x2bd7, "Particulate Matter - PM10 Concentration" },
    { 0x2bd8, "Sulfur Dioxide Concentration" },
    { 0x2bd9, "Sulfur Hexafluoride Concentration" },
    { 0x2bda, "Hearing Aid Features" },
    { 0x2bdb, "Hearing Aid Preset Control Point" },
    { 0x2bdc, "Active Preset Index" },
    { 0x2bdd, "Stored Health Observations" },
    { 0x2bde, "Fixed String 64" },
    { 0x2bdf, "High Temperature" },
    { 0x2be0, "High Voltage" },
    { 0x2be1, "Light Distribution" },
    { 0x2be2, "Light Output" },
    { 0x2be3, "Light Source Type" },
    { 0x2be4, "Noise" },
    { 0x2be5, "Relative Runtime in a Correlated Color Temperature Range" },
    { 0x2be6, "Time Second 32" },
    { 0x2be7, "VOC Concentration" },
    { 0x2be8, "Voltage Frequency" },
    { 0x2be9, "Battery Critical Status" },
    { 0x2bea, "Battery Health Status" },
    { 0x2beb, "Battery Health Information" },
    { 0x2bec, "Battery Information" },
    { 0x2bed, "Battery Level Status" },
    { 0x2bee, "Battery Time Status" },
    { 0x2bef, "Estimated Service Date" },
    { 0x2bf0, "Battery Energy Status" },
    { 0x2bf1, "Observation Schedule Changed" },
    { 0x2bf2, "Current Elapsed Time" },
    { 0x2bf3, "Health Sensor Features" },
    { 0x2bf4, "GHS Control Point" },
    { 0x2bf5, "LE GATT Security Levels" },
    { 0x2bf6, "ESL Address" },
    { 0x2bf7, "AP Sync Key Material" },
    { 0x2bf8, "ESL Response Key Material" },
    { 0x2bf9, "ESL Current Absolute Time" },
    { 0x2bfa, "ESL Display Information" },
    { 0x2bfb, "ESL Image Information" },
    { 0x2bfc, "ESL Sensor Information" },
    { 0x2bfd, "ESL LED Information" },
    { 0x2bfe, "ESL Control Point" },
    { 0x2bff, "UDI for Medical Devices" },
    { 0xfe1c, "NetMedia, Inc." },
    { 0xfe1d, "Illuminati Instrument Corporation" },
    { 0xfe1e, "Smart Innovations Co., Ltd" },
    { 0xfe1f, "Garmin International, Inc." },
    { 0xfe20, "Emerson" },
    { 0xfe21, "Bose Corporation" },
    { 0xfe22, "Zoll Medical Corporation" },
    { 0xfe23, "Zoll Medical Corporation" },
    { 0xfe24, "August Home Inc" },
    { 0xfe25, "Apple, Inc." },
    { 0xfe26, "Google Inc." },
    { 0xfe27, "Google Inc." },
    { 0xfe28, "Ayla Network" },
    { 0xfe29, "Gibson Innovations" },
    { 0xfe2a, "DaisyWorks, Inc." },
    { 0xfe2b, "ITT Industries" },
    { 0xfe2c, "Google Inc." },
    { 0xfe2d, "SMART INNOVATION Co. },Ltd" },
    { 0xfe2e, "ERi },Inc." },
    { 0xfe2f, "CRESCO Wireless, Inc" },
    { 0xfe30, "Volkswagen AG" },
    { 0xfe31, "Volkswagen AG" },
    { 0xfe32, "Pro-Mark, Inc." },
    { 0xfe33, "CHIPOLO d.o.o." },
    { 0xfe34, "SmallLoop LLC" },
    { 0xfe35, "HUAWEI Technologies Co., Ltd" },
    { 0xfe36, "HUAWEI Technologies Co., Ltd" },
    { 0xfe37, "Spaceek LTD" },
    { 0xfe38, "Spaceek LTD" },
    { 0xfe39, "TTS Tooltechnic Systems AG & Co. KG" },
    { 0xfe3a, "TTS Tooltechnic Systems AG & Co. KG" },
    { 0xfe3b, "Dolby Laboratories" },
    { 0xfe3c, "Alibaba" },
    { 0xfe3d, "BD Medical" },
    { 0xfe3e, "BD Medical" },
    { 0xfe3f, "Friday Labs Limited" },
    { 0xfe40, "Inugo Systems Limited" },
    { 0xfe41, "Inugo Systems Limited" },
    { 0xfe42, "Nets A/S" },
    { 0xfe43, "Andreas Stihl AG & Co. KG" },
    { 0xfe44, "SK Telecom" },
    { 0xfe45, "Snapchat Inc" },
    { 0xfe46, "B&O Play A/S" },
    { 0xfe47, "General Motors" },
    { 0xfe48, "General Motors" },
    { 0xfe49, "SenionLab AB" },
    { 0xfe4a, "OMRON HEALTHCARE Co., Ltd." },
    { 0xfe4b, "Koninklijke Philips N.V." },
    { 0xfe4c, "Volkswagen AG" },
    { 0xfe4d, "Casambi Technologies Oy" },
    { 0xfe4e, "NTT docomo" },
    { 0xfe4f, "Molekule, Inc." },
    { 0xfe50, "Google Inc." },
    { 0xfe51, "SRAM" },
    { 0xfe52, "SetPoint Medical" },
    { 0xfe53, "3M" },
    { 0xfe54, "Motiv, Inc." },
    { 0xfe55, "Google Inc." },
    { 0xfe56, "Google Inc." },
    { 0xfe57, "Dotted Labs" },
    { 0xfe58, "Nordic Semiconductor ASA" },
    { 0xfe59, "Nordic Semiconductor ASA" },
    { 0xfe5a, "Chronologics Corporation" },
    { 0xfe5b, "GT-tronics HK Ltd" },
    { 0xfe5c, "million hunters GmbH" },
    { 0xfe5d, "Grundfos A/S" },
    { 0xfe5e, "Plastc Corporation" },
    { 0xfe5f, "Eyefi, Inc." },
    { 0xfe60, "Lierda Science & Technology Group Co., Ltd." },
    { 0xfe61, "Logitech International SA" },
    { 0xfe62, "Indagem Tech LLC" },
    { 0xfe63, "Connected Yard, Inc." },
    { 0xfe64, "Siemens AG" },
    { 0xfe65, "CHIPOLO d.o.o." },
    { 0xfe66, "Intel Corporation" },
    { 0xfe67, "Lab Sensor Solutions" },
    { 0xfe68, "Qualcomm Life Inc" },
    { 0xfe69, "Qualcomm Life Inc" },
    { 0xfe6a, "Kontakt Micro-Location Sp. z o.o." },
    { 0xfe6b, "TASER International, Inc." },
    { 0xfe6c, "TASER International, Inc." },
    { 0xfe6d, "The University of Tokyo" },
    { 0xfe6e, "The University of Tokyo" },
    { 0xfe6f, "LINE Corporation" },
    { 0xfe70, "Beijing Jingdong Century Trading Co., Ltd." },
    { 0xfe71, "Plume Design Inc" },
    { 0xfe72, "St. Jude Medical, Inc." },
    { 0xfe73, "St. Jude Medical, Inc." },
    { 0xfe74, "unwire" },
    { 0xfe75, "TangoMe" },
    { 0xfe76, "TangoMe" },
    { 0xfe77, "Hewlett-Packard Company" },
    { 0xfe78, "Hewlett-Packard Company" },
    { 0xfe79, "Zebra Technologies" },
    { 0xfe7a, "Bragi GmbH" },
    { 0xfe7b, "Orion Labs, Inc." },
    { 0xfe7c, "Stollmann E+V GmbH" },
    { 0xfe7d, "Aterica Health Inc." },
    { 0xfe7e, "Awear Solutions Ltd" },
    { 0xfe7f, "Doppler Lab" },
    { 0xfe80, "Doppler Lab" },
    { 0xfe81, "Medtronic Inc." },
    { 0xfe82, "Medtronic Inc." },
    { 0xfe83, "Blue Bite" },
    { 0xfe84, "RF Digital Corp" },
    { 0xfe85, "RF Digital Corp" },
    { 0xfe86, "HUAWEI Technologies Co., Ltd." },
    { 0xfe87, "Qingdao Yeelink Information Technology Co., Ltd." },
    { 0xfe88, "SALTO SYSTEMS S.L." },
    { 0xfe89, "B&O Play A/S" },
    { 0xfe8a, "Apple, Inc." },
    { 0xfe8b, "Apple, Inc." },
    { 0xfe8c, "TRON Forum" },
    { 0xfe8d, "Interaxon Inc." },
    { 0xfe8e, "ARM Ltd" },
    { 0xfe8f, "CSR" },
    { 0xfe90, "JUMA" },
    { 0xfe91, "Shanghai Imilab Technology Co. },Ltd" },
    { 0xfe92, "Jarden Safety & Security" },
    { 0xfe93, "OttoQ Inc." },
    { 0xfe94, "OttoQ Inc." },
    { 0xfe95, "Xiaomi Inc." },
    { 0xfe96, "Tesla Motor Inc." },
    { 0xfe97, "Tesla Motor Inc." },
    { 0xfe98, "Currant, Inc." },
    { 0xfe99, "Currant, Inc." },
    { 0xfe9a, "Estimote" },
    { 0xfe9b, "Samsara Networks, Inc" },
    { 0xfe9c, "GSI Laboratories, Inc." },
    { 0xfe9d, "Mobiquity Networks Inc" },
    { 0xfe9e, "Dialog Semiconductor B.V." },
    { 0xfe9f, "Google" },
    { 0xfea0, "Google" },
    { 0xfea1, "Intrepid Control Systems, Inc." },
    { 0xfea2, "Intrepid Control Systems, Inc." },
    { 0xfea3, "ITT Industries" },
    { 0xfea4, "Paxton Access Ltd" },
    { 0xfea5, "GoPro, Inc." },
    { 0xfea6, "GoPro, Inc." },
    { 0xfea7, "UTC Fire and Security" },
    { 0xfea8, "Savant Systems LLC" },
    { 0xfea9, "Savant Systems LLC" },
    { 0xfeaa, "Google" },
    { 0xfeab, "Nokia Corporation" },
    { 0xfeac, "Nokia Corporation" },
    { 0xfead, "Nokia Corporation" },
    { 0xfeae, "Nokia Corporation" },
    { 0xfeaf, "Nest Labs Inc." },
    { 0xfeb0, "Nest Labs Inc." },
    { 0xfeb1, "Electronics Tomorrow Limited" },
    { 0xfeb2, "Microsoft Corporation" },
    { 0xfeb3, "Taobao" },
    { 0xfeb4, "WiSilica Inc." },
    { 0xfeb5, "WiSilica Inc." },
    { 0xfeb6, "Vencer Co, Ltd" },
    { 0xfeb7, "Facebook, Inc." },
    { 0xfeb8, "Facebook, Inc." },
    { 0xfeb9, "LG Electronics" },
    { 0xfeba, "Tencent Holdings Limited" },
    { 0xfebb, "adafruit industries" },
    { 0xfebc, "Dexcom, Inc." },
    { 0xfebd, "Clover Network, Inc." },
    { 0xfebe, "Bose Corporation" },
    { 0xfebf, "Nod, Inc." },
    { 0xfec0, "KDDI Corporation" },
    { 0xfec1, "KDDI Corporation" },
    { 0xfec2, "Blue Spark Technologies, Inc." },
    { 0xfec3, "360fly, Inc." },
    { 0xfec4, "PLUS Location Systems" },
    { 0xfec5, "Realtek Semiconductor Corp." },
    { 0xfec6, "Kocomojo, LLC" },
    { 0xfec7, "Apple, Inc." },
    { 0xfec8, "Apple, Inc." },
    { 0xfec9, "Apple, Inc." },
    { 0xfeca, "Apple, Inc." },
    { 0xfecb, "Apple, Inc." },
    { 0xfecc, "Apple, Inc." },
    { 0xfecd, "Apple, Inc." },
    { 0xfece, "Apple, Inc." },
    { 0xfecf, "Apple, Inc." },
    { 0xfed0, "Apple, Inc." },
    { 0xfed1, "Apple, Inc." },
    { 0xfed2, "Apple, Inc." },
    { 0xfed3, "Apple, Inc." },
    { 0xfed4, "Apple, Inc." },
    { 0xfed5, "Plantronics Inc." },
    { 0xfed6, "Broadcom Corporation" },
    { 0xfed7, "Broadcom Corporation" },
    { 0xfed8, "Google" },
    { 0xfed9, "Pebble Technology Corporation" },
    { 0xfeda, "ISSC Technologies Corporation" },
    { 0xfedb, "Perka, Inc." },
    { 0xfedc, "Jawbone" },
    { 0xfedd, "Jawbone" },
    { 0xfede, "Coin, Inc." },
    { 0xfedf, "Design SHIFT" },
    { 0xfee0, "Anhui Huami Information Technology Co." },
    { 0xfee1, "Anhui Huami Information Technology Co." },
    { 0xfee2, "Anki, Inc." },
    { 0xfee3, "Anki, Inc." },
    { 0xfee4, "Nordic Semiconductor ASA" },
    { 0xfee5, "Nordic Semiconductor ASA" },
    { 0xfee6, "Seed Labs, Inc." },
    { 0xfee7, "Tencent Holdings Limited" },
    { 0xfee8, "Quintic Corp." },
    { 0xfee9, "Quintic Corp." },
    { 0xfeea, "Swirl Networks, Inc." },
    { 0xfeeb, "Swirl Networks, Inc." },
    { 0xfeec, "Tile, Inc." },
    { 0xfeed, "Tile, Inc." },
    { 0xfeee, "Polar Electro Oy" },
    { 0xfeef, "Polar Electro Oy" },
    { 0xfef0, "Intel" },
    { 0xfef1, "CSR" },
    { 0xfef2, "CSR" },
    { 0xfef3, "Google" },
    { 0xfef4, "Google" },
    { 0xfef5, "Dialog Semiconductor GmbH" },
    { 0xfef6, "Wicentric, Inc." },
    { 0xfef7, "Aplix Corporation" },
    { 0xfef8, "Aplix Corporation" },
    { 0xfef9, "PayPal, Inc." },
    { 0xfefa, "PayPal, Inc." },
    { 0xfefb, "Stollmann E+V GmbH" },
    { 0xfefc, "Gimbal, Inc." },
    { 0xfefd, "Gimbal, Inc." },
    { 0xfefe, "GN ReSound A/S" },
    { 0xfeff, "GN Netcom" },
    { 0xfffc, "AirFuel Alliance" },
    { 0xfffd, "Fast IDentity Online Alliance (FIDO)" },
    { 0xfffe, "Alliance for Wireless Power (A4WP)" }
};

static const char* devicePairingResultStatusStrings[] {
    "paired",
    "not ready to pair",
    "not paired",
    "already paired",
    "connection rejected",
    "too many connections",
    "hardware failure",
    "authentication timeout",
    "authentication not allowed",
    "authentication failure",
    "no supported profiles",
    "protection level could not be met",
    "access denied",
    "invalid ceremony data",
    "pairing canceled",
    "operation already in progress",
    "required handler not registered",
    "rejected by handler",
    "remote device has association",
    "failed"
};

static const char* meshtasticMessageTypeStrings[] {
    "unknown",
    "log",
    "from radio",
    "to radio"
};

static const char* gattSessionStatusStrings[] {
    "closed",
    "active"
};

static const char* channelRoleStrings[] {
    "disabled",
    "primary",
    "secondary"
};

static const char* asyncStatusStrings[] {
        "started",
        "Completed",
        "canceled",
        "error"
};

static const char* routungErrorStrings0_9[]{
        "none",
        "no route",
        "got nak",
        "timeout",
        "no interface",
        "max retransmit",
        "no channel",
        "too large",
        "no response",
        "duty cycle limit"
};

static const char* routungErrorStrings32_38[] {
        "bad request",
        "not authorized",
        "pki failed",
        "pki unknown_pubkey",
        "admin bad session key",
        "admin public key unauthorized",
        "rate limit exceeded"
};

/**
 * * 0-63   Core Meshtastic use, do not use for third party apps
 * 64-127 Registered 3rd party apps, send in a pull request that adds a new entry to portnums.proto to  register your application
 * 256-511 Use one of these portnums for your private applications that you don't want to register publically
 */
static const char* portNames0_12[] {
    "unknown",
    "text_message",
    "remote_hardware",
    "position",
    "nodeinfo",
    "routing",
    "admin",
    "text_message_compressed",
    "waypoint",
    "audio",
    "detection_sensor",
    "alert",
    "key_verification"
};

static const char* portNames32_34[] {
    "reply",
    "ip_tunnel",
    "paxcounter"
};

static const char* portNames64_77[]{
    "serial",
    "store forward",
    "range test",
    "telemetry",
    "zps",
    "simulator",
    "traceroute",
    "neighbor info",
    "atak plugin",
    "map report",
    "power stress",
    "reticulum tunnel",
    "cayenne"
};

static const char* portNames256_257[]{
"private",
"atak forwarder"
};

std::string MeshtasticString::port2string(
    meshtastic::PortNum portNum
)
{
    if (portNum <= meshtastic::KEY_VERIFICATION_APP)
        return portNames0_12[(int) portNum];
    if ((portNum >= meshtastic::IP_TUNNEL_APP) && (portNum <= meshtastic::PAXCOUNTER_APP))
        return portNames32_34[(int) portNum - 32];
    if ((portNum >= meshtastic::SERIAL_APP) && (portNum <= meshtastic::CAYENNE_APP))
        return portNames64_77[(int) portNum - 64];
    if ((portNum >= meshtastic::PRIVATE_APP) && (portNum <= meshtastic::ATAK_FORWARDER))
        return portNames256_257[(int) portNum - 256];
    return "";
}

std::string MeshtasticString::advertisementDataType2String(
    uint8_t dataType
)
{
    if (dataType == 0xff)
        dataType = 54;
    else if (dataType > 53)
        dataType = 0;
    return commonDataTypes[dataType];
}

std::string MeshtasticString::macAddress2string(
    uint64_t addr
)
{
    std::stringstream ss;
    ss << std::hex << std::setfill(NUMBER_FILL);
    for (int i = 5; i > 0; i--) {
        ss << std::setw(2) << ((addr >> (i * 8)) & 0xff) << ADDR_DELIMITER;
    }
    ss << std::setw(2) << (addr & 0xff);
    return ss.str();
}


std::string MeshtasticString::transportType2String(
    MeshtasticTransportType typ
) {
    return TRANSPORT_TYPE_NAMES[typ];
}

std::string MeshtasticString::hex(
    const std::string &str
) {
    return hex((void *) str.c_str(), str.size());
}

std::string MeshtasticString::hex(
    void *buffer,
    size_t size
) {
    std::stringstream ss;
    ss << std::hex << std::setfill(NUMBER_FILL);
    for (size_t i = 0; i < size; i++) {
        ss << std::setw(2) << (int) *(((unsigned char*) buffer)  + i);
    }
    return ss.str();
}

std::string MeshtasticString::meshtasticMessageType2String(
    MeshtasticMessageType typ
)
{
    return meshtasticMessageTypeStrings[(int) typ];
}

std::string MeshtasticString::channelRole2String(
    meshtastic::Channel_Role role
) {
    if (((int) role) < 3)
        return channelRoleStrings[(int) role];
    return std::to_string((int) role);
}

std::string MeshtasticString::psk2String(
    const std::string &psk
) {
    auto sz = psk.size();
    switch (sz) {
        case 1:
        {
            uint8_t c = (uint8_t) psk[0];
            if (c == 0)
                return "";  // none
            // default PSK. Replace last byte by channel index.
            uint8_t b[] DEF_PSK_1;
            return hex(b, 15) + hex(&c, 1);
        }
        case 16:
        case 32:
            return hex((void*) psk.c_str(), sz);
        default:
            break;
    }
    return "";  // none
}

std::string MeshtasticString::routingError2string(
        meshtastic::Routing_Error error
) {
    int e = (int) error;
    if (e >= 0 && e <= 9)
        return routungErrorStrings0_9[e];
    else
    if (e >= 32 && e <= 38)
        return routungErrorStrings32_38[e];
    return "";
}

#ifdef _MSC_VER

std::string MeshtasticString::characteristicProperties2String(
    const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic &characteristic
)
{
    std::stringstream ss;
    winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicProperties properties = characteristic.CharacteristicProperties();
    for (int i = 0; i < 10; i++) {
        if ((properties & (winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicProperties) (1 << i)) != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicProperties::None) {
            ss << GATT_ATTR_PROP_NAMES[i] << ' ';
        }
    }
    return ss.str();
}

std::string MeshtasticString::characteristic2String(
    const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic &characteristic
)
{
    auto uuid = characteristic.Uuid();
    if (uuid.Data2 == 0 && uuid.Data3 == 0x1000
        && uuid.Data4[0] == 0x80 && uuid.Data4[1] == 0x00 && uuid.Data4[2] == 0x00 && uuid.Data4[3] == 0x80
        && uuid.Data4[4] == 0x5f && uuid.Data4[5] == 0x9b && uuid.Data4[6] == 0x34 && uuid.Data4[7] == 0xfb) {

        auto f = characteristicsNames.find(uuid.Data1);
        if (f != characteristicsNames.end())
            return f->second;
    }
    return BLEGuid::UUIDToString(uuid);
}

std::string MeshtasticString::devicePairingStatus2string(
    winrt::Windows::Devices::Enumeration::DevicePairingResultStatus status)
{
    return devicePairingResultStatusStrings[(int) status];
}

static const char* gattCommunicationStatusStrings[] {
        "success",
        "unreachable",
        "protocol error",
        "access denied"
};

std::string MeshtasticString::gattCommunicationStatus2string(
    const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus &status
) {
    return gattCommunicationStatusStrings[(int) status];
}

std::string MeshtasticString::hstring2string(const winrt::hstring &value) {
#pragma warning(push)
#pragma warning(disable: 4244)
    return std::string(value.begin(), value.end());
#pragma warning(pop)
}

std::string MeshtasticString::sessionStatus2string(
    const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSessionStatus status
) {
    return gattSessionStatusStrings[(int) status];
}

std::string MeshtasticString::asyncStatus2string(
    const winrt::Windows::Foundation::AsyncStatus status
) {
    return asyncStatusStrings[(int) status];
}

std::string MeshtasticString::currentTimeStamp()
{
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm *tm = std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(tm, "%FT%T%z");
    return ss.str();
}

#endif
