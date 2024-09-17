#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <shlwapi.h>
#include <algorithm>
#include <cctype>

#define IOCTL_CREATE_VDISK CTL_CODE(FILE_DEVICE_DISK, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_REMOVE_VDISK CTL_CODE(FILE_DEVICE_DISK, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

using namespace std;

class HandleGuard {
public:
    explicit HandleGuard(HANDLE handle) : handle_(handle) {}
    ~HandleGuard() {
        if (handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
        }
    }
    HANDLE get() const { return handle_; }
    void release() { handle_ = INVALID_HANDLE_VALUE; }

private:
    HANDLE handle_;
};

void displayAscii(const string& filePath);
void ensureElevated();

void createVirtualDisk(const string& scriptsPath);
void removeVirtualDisk(const string& scriptsPath);
void installDriver(const string& scriptsPath);
void uninstallDriver(const string& scriptsPath);
void startDriver(const string& scriptsPath);
void stopDriver(const string& scriptsPath);
void performDeviceOperations();
void showHelp();

bool isDriverInstalled();
bool isDriverRunning();

string getScriptsPath();
string getDriverPath();

int main() {
    cout << "\n";
    displayAscii("../../UltimateWfsDriverUtility/banner.txt");
    cout << "\n\t\t\t Ultimate wfsDriver Utility\n\n\n";

    showHelp();

    ensureElevated();

    string scriptsPath = getScriptsPath();

    string command;

    while (true) {
        cout << "\n>> ";
        getline(cin, command);
        cout << "\n";

        transform(command.begin(), command.end(), command.begin(), ::toupper);

        if (command == "INSTALL")      installDriver(scriptsPath);
        else if (command == "UNINSTALL")    uninstallDriver(scriptsPath);
        else if (command == "CREATEVDISK")  createVirtualDisk(scriptsPath);
        else if (command == "REMOVEVDISK")  removeVirtualDisk(scriptsPath);
        else if (command == "START")        startDriver(scriptsPath);
        else if (command == "STOP")         stopDriver(scriptsPath);
        else if (command == "DEVICEOPS")    performDeviceOperations();
        else if (command == "HELP")         showHelp();
        else if (command == "EXIT")         exit(0);
        else cout << "Unknown command entered. Type 'help' to see available commands.\n";
    }

    return 0;
}

void displayAscii(const string& filePath) {
    ifstream file(filePath, ios::in | ios::binary);
    if (file) {
        string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        cout << fileContent;
        file.close();
    }
}

void ensureElevated() {
    BOOL isAdmin = FALSE;
    HANDLE token = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size;
        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isAdmin = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }

    if (!isAdmin) {
        wchar_t path[MAX_PATH];
        GetModuleFileName(NULL, path, MAX_PATH);

        ShellExecute(NULL, L"runas", path, NULL, NULL, SW_SHOWNORMAL);

        exit(0);
    }
}

bool isDriverInstalled() {
    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scManager) {
        return false;
    }

    SC_HANDLE scService = OpenService(scManager, L"wfsDriver", SERVICE_QUERY_STATUS);
    if (!scService) {
        CloseServiceHandle(scManager);
        return false;
    }

    CloseServiceHandle(scService);
    CloseServiceHandle(scManager);

    return true;
}

bool isDriverRunning() {
    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scManager) {
        return false;
    }

    SC_HANDLE scService = OpenService(scManager, L"wfsDriver", SERVICE_QUERY_STATUS);
    if (!scService) {
        CloseServiceHandle(scManager);
        return false;
    }

    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;
    bool isRunning = false;

    if (QueryServiceStatusEx(scService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
        isRunning = (ssStatus.dwCurrentState == SERVICE_RUNNING);
    }

    CloseServiceHandle(scService);
    CloseServiceHandle(scManager);

    return isRunning;
}

string getScriptsPath() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    PathRemoveFileSpec(exePath);
    PathRemoveFileSpec(exePath);
    PathRemoveFileSpec(exePath);

    PathAppend(exePath, L"\\Scripts");

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, exePath, (int)wcslen(exePath), NULL, 0, NULL, NULL);
    string path(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, exePath, (int)wcslen(exePath), &path[0], size_needed, NULL, NULL);

    return path;
}

string getDriverPath() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    PathRemoveFileSpec(exePath);

    PathAppend(exePath, L"\\wfsDriver.sys");

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, exePath, (int)wcslen(exePath), NULL, 0, NULL, NULL);
    string path(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, exePath, (int)wcslen(exePath), &path[0], size_needed, NULL, NULL);

    return path;
}

void installDriver(const string& scriptsPath) {
    if (isDriverInstalled()) {
        cout << "Driver is already installed.\n";
        return;
    }
    system((scriptsPath + "\\InstallDriver.bat \"" + getDriverPath() + "\"").c_str());
}

void uninstallDriver(const string& scriptsPath) {
    if (isDriverRunning()) {
        cout << "Driver is running. Please stop the driver before uninstalling.\n";
        return;
    }
    if (!isDriverInstalled()) {
        cout << "Driver is not installed.\n";
        return;
    }
    system((scriptsPath + "\\UninstallDriver.bat").c_str());
}

void createVirtualDisk(const string& scriptsPath) {
    system((scriptsPath + "\\CreateVirtualDisk.bat").c_str());
}

void removeVirtualDisk(const string& scriptsPath) {
    system((scriptsPath + "\\RemoveVirtualDisk.bat").c_str());
}

void startDriver(const string& scriptsPath) {
    if (isDriverRunning()) {
        cout << "Driver is already running.\n";
        return;
    }
    if (!isDriverInstalled()) {
        cout << "Driver is not installed.\n";
        return;
    }
    system((scriptsPath + "\\StartDriver.bat").c_str());
}

void stopDriver(const string& scriptsPath) {
    if (!isDriverRunning()) {
        cout << "Driver is not running.\n";
        return;
    }
    system((scriptsPath + "\\StopDriver.bat").c_str());
}

void performDeviceOperations() {
    const char* deviceName = "\\\\.\\WFSDevice";
    wchar_t wDeviceName[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, deviceName, -1, wDeviceName, MAX_PATH);

    HANDLE hFile = CreateFile(
        L"W:\\helloworld.txt",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        cout << "Failed to create file: " << error << endl;
        return;
    }

    HandleGuard fileGuard(hFile);

    const char* data = "Hello, World!";
    DWORD bytesWritten;
    BOOL writeResult = WriteFile(
        hFile,
        data,
        static_cast<DWORD>(strlen(data)),
        &bytesWritten,
        NULL
    );

    if (!writeResult) {
        DWORD error = GetLastError();
        cout << "Failed to write to file: " << error << endl;
    }
    else {
        cout << "Successfully wrote to file." << endl;
    }
}

void showHelp() {
    cout << "Ultimate wfsDriver Utility (UwU) - Command List\n\n";

    cout << "install\t\t- Install the wfsDriver.\n";
    cout << "uninstall\t- Uninstall the wfsDriver.\n";
    cout << "createvdisk\t- Create a virtual disk.\n";
    cout << "removevdisk\t- Remove the virtual disk.\n";
    cout << "start\t\t- Start the drive.\n";
    cout << "stop\t\t- Stop the driver.\n";
    cout << "deviceops\t- Perform device operations.\n";
    cout << "help\t\t- Show this help message.\n";
    cout << "exit\t\t- Close UwU.\n";
}
