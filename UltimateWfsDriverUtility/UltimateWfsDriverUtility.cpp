//#include <windows.h>
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <shlwapi.h>
//#include <algorithm>
//#include <cctype>
//
//#define IOCTL_CREATE_VDISK CTL_CODE(FILE_DEVICE_DISK, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_REMOVE_VDISK CTL_CODE(FILE_DEVICE_DISK, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
//
//using namespace std;
//
//class HandleGuard {
//public:
//    explicit HandleGuard(HANDLE handle) : handle_(handle) {}
//    ~HandleGuard() {
//        if (handle_ != INVALID_HANDLE_VALUE) {
//            CloseHandle(handle_);
//        }
//    }
//    HANDLE get() const { return handle_; }
//    void release() { handle_ = INVALID_HANDLE_VALUE; }
//
//private:
//    HANDLE handle_;
//};
//
//void displayAscii(const string& filePath);
//void ensureElevated();
//
//void createVirtualDisk(const string& scriptsPath);
//void removeVirtualDisk(const string& scriptsPath);
//void installDriver(const string& scriptsPath);
//void uninstallDriver(const string& scriptsPath);
//void startDriver(const string& scriptsPath);
//void stopDriver(const string& scriptsPath);
//void performDeviceOperations();
//void showHelp();
//
//bool isDriverInstalled();
//bool isDriverRunning();
//
//string getScriptsPath();
//string getDriverPath();
//
//int main() {
//    cout << "\n";
//    displayAscii("../../UltimateWfsDriverUtility/banner.txt");
//    cout << "\n\t\t\t Ultimate wfsDriver Utility\n\n\n";
//
//    showHelp();
//
//    ensureElevated();
//
//    string scriptsPath = getScriptsPath();
//
//    string command;
//
//    while (true) {
//        cout << "\n>> ";
//        getline(cin, command);
//        cout << "\n";
//
//        transform(command.begin(), command.end(), command.begin(), ::toupper);
//
//        if (command == "INSTALL")      installDriver(scriptsPath);
//        else if (command == "UNINSTALL")    uninstallDriver(scriptsPath);
//        else if (command == "CREATEVDISK")  createVirtualDisk(scriptsPath);
//        else if (command == "REMOVEVDISK")  removeVirtualDisk(scriptsPath);
//        else if (command == "START")        startDriver(scriptsPath);
//        else if (command == "STOP")         stopDriver(scriptsPath);
//        else if (command == "DEVICEOPS")    performDeviceOperations();
//        else if (command == "HELP")         showHelp();
//        else if (command == "EXIT")         exit(0);
//        else cout << "Unknown command entered. Type 'help' to see available commands.\n";
//    }
//
//    return 0;
//}
//
//void displayAscii(const string& filePath) {
//    ifstream file(filePath, ios::in | ios::binary);
//    if (file) {
//        string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
//        cout << fileContent;
//        file.close();
//    }
//}
//
//void ensureElevated() {
//    BOOL isAdmin = FALSE;
//    HANDLE token = NULL;
//
//    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
//        TOKEN_ELEVATION elevation;
//        DWORD size;
//        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
//            isAdmin = elevation.TokenIsElevated;
//        }
//        CloseHandle(token);
//    }
//
//    if (!isAdmin) {
//        wchar_t path[MAX_PATH];
//        GetModuleFileName(NULL, path, MAX_PATH);
//
//        ShellExecute(NULL, L"runas", path, NULL, NULL, SW_SHOWNORMAL);
//
//        exit(0);
//    }
//}
//
//bool isDriverInstalled() {
//    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
//    if (!scManager) {
//        return false;
//    }
//
//    SC_HANDLE scService = OpenService(scManager, L"wfsDriver", SERVICE_QUERY_STATUS);
//    if (!scService) {
//        CloseServiceHandle(scManager);
//        return false;
//    }
//
//    CloseServiceHandle(scService);
//    CloseServiceHandle(scManager);
//
//    return true;
//}
//
//bool isDriverRunning() {
//    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
//    if (!scManager) {
//        return false;
//    }
//
//    SC_HANDLE scService = OpenService(scManager, L"wfsDriver", SERVICE_QUERY_STATUS);
//    if (!scService) {
//        CloseServiceHandle(scManager);
//        return false;
//    }
//
//    SERVICE_STATUS_PROCESS ssStatus;
//    DWORD dwBytesNeeded;
//    bool isRunning = false;
//
//    if (QueryServiceStatusEx(scService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
//        isRunning = (ssStatus.dwCurrentState == SERVICE_RUNNING);
//    }
//
//    CloseServiceHandle(scService);
//    CloseServiceHandle(scManager);
//
//    return isRunning;
//}
//
//string getScriptsPath() {
//    wchar_t exePath[MAX_PATH];
//    GetModuleFileName(NULL, exePath, MAX_PATH);
//
//    PathRemoveFileSpec(exePath);
//    PathRemoveFileSpec(exePath);
//    PathRemoveFileSpec(exePath);
//
//    PathAppend(exePath, L"\\Scripts");
//
//    int size_needed = WideCharToMultiByte(CP_UTF8, 0, exePath, (int)wcslen(exePath), NULL, 0, NULL, NULL);
//    string path(size_needed, 0);
//    WideCharToMultiByte(CP_UTF8, 0, exePath, (int)wcslen(exePath), &path[0], size_needed, NULL, NULL);
//
//    return path;
//}
//
//string getDriverPath() {
//    wchar_t exePath[MAX_PATH];
//    GetModuleFileName(NULL, exePath, MAX_PATH);
//
//    PathRemoveFileSpec(exePath);
//
//    PathAppend(exePath, L"\\wfsDriver.sys");
//
//    int size_needed = WideCharToMultiByte(CP_UTF8, 0, exePath, (int)wcslen(exePath), NULL, 0, NULL, NULL);
//    string path(size_needed, 0);
//    WideCharToMultiByte(CP_UTF8, 0, exePath, (int)wcslen(exePath), &path[0], size_needed, NULL, NULL);
//
//    return path;
//}
//
//void installDriver(const string& scriptsPath) {
//    if (isDriverInstalled()) {
//        cout << "Driver is already installed.\n";
//        return;
//    }
//    system((scriptsPath + "\\InstallDriver.bat \"" + getDriverPath() + "\"").c_str());
//}
//
//void uninstallDriver(const string& scriptsPath) {
//    if (isDriverRunning()) {
//        cout << "Driver is running. Please stop the driver before uninstalling.\n";
//        return;
//    }
//    if (!isDriverInstalled()) {
//        cout << "Driver is not installed.\n";
//        return;
//    }
//    system((scriptsPath + "\\UninstallDriver.bat").c_str());
//}
//
//void createVirtualDisk(const string& scriptsPath) {
//    system((scriptsPath + "\\CreateVirtualDisk.bat").c_str());
//}
//
//void removeVirtualDisk(const string& scriptsPath) {
//    system((scriptsPath + "\\RemoveVirtualDisk.bat").c_str());
//}
//
//void startDriver(const string& scriptsPath) {
//    if (isDriverRunning()) {
//        cout << "Driver is already running.\n";
//        return;
//    }
//    if (!isDriverInstalled()) {
//        cout << "Driver is not installed.\n";
//        return;
//    }
//    system((scriptsPath + "\\StartDriver.bat").c_str());
//}
//
//void stopDriver(const string& scriptsPath) {
//    if (!isDriverRunning()) {
//        cout << "Driver is not running.\n";
//        return;
//    }
//    system((scriptsPath + "\\StopDriver.bat").c_str());
//}
//
//void performDeviceOperations() {
//    const char* deviceName = "\\\\.\\WFSDevice";
//    wchar_t wDeviceName[MAX_PATH];
//    MultiByteToWideChar(CP_UTF8, 0, deviceName, -1, wDeviceName, MAX_PATH);
//
//    HANDLE hFile = CreateFile(
//        L"W:\\helloworld.txt",
//        GENERIC_WRITE,
//        0,
//        NULL,
//        CREATE_ALWAYS,
//        FILE_ATTRIBUTE_NORMAL,
//        NULL
//    );
//
//    if (hFile == INVALID_HANDLE_VALUE) {
//        DWORD error = GetLastError();
//        cout << "Failed to create file: " << error << endl;
//        return;
//    }
//
//    HandleGuard fileGuard(hFile);
//
//    const char* data = "Hello, World!";
//    DWORD bytesWritten;
//    BOOL writeResult = WriteFile(
//        hFile,
//        data,
//        static_cast<DWORD>(strlen(data)),
//        &bytesWritten,
//        NULL
//    );
//
//    if (!writeResult) {
//        DWORD error = GetLastError();
//        cout << "Failed to write to file: " << error << endl;
//    }
//    else {
//        cout << "Successfully wrote to file." << endl;
//    }
//}
//
//void showHelp() {
//    cout << "Ultimate wfsDriver Utility (UwU) - Command List\n\n";
//
//    cout << "install\t\t- Install the wfsDriver.\n";
//    cout << "uninstall\t- Uninstall the wfsDriver.\n";
//    cout << "createvdisk\t- Create a virtual disk.\n";
//    cout << "removevdisk\t- Remove the virtual disk.\n";
//    cout << "start\t\t- Start the drive.\n";
//    cout << "stop\t\t- Stop the driver.\n";
//    cout << "deviceops\t- Perform device operations.\n";
//    cout << "help\t\t- Show this help message.\n";
//    cout << "exit\t\t- Close UwU.\n";
//}
 





//TA WERSJA DZIALA PRZEZ WINDOWSA TWORZENIE 
/*#include <windows.h>
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

std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void displayAscii(const string& filePath);
void ensureElevated();

void createVirtualDisk(const string& scriptsPath);
void removeVirtualDisk(const string& scriptsPath);
void installDriver(const string& scriptsPath);
void uninstallDriver(const string& scriptsPath);
void startDriver(const string& scriptsPath);
void stopDriver(const string& scriptsPath);
void showHelp();

bool isDriverInstalled();
bool isDriverRunning();

string getScriptsPath();
string getDriverPath();

void createFile(const string& filePath);
void readFile(const string& filePath);
void writeFile(const string& filePath, const string& data);
void listFiles(const string& directoryPath);
void openFile(const string& filePath);
void queryFileAttributes(const string& filePath);

const string BASE_PATH = "W:/";

int main() {
    cout << "\n";
    displayAscii("../../UltimateWfsDriverUtility/banner.txt");
    cout << "\n\t\t\t Ultimate wfsDriver Utility\n\n\n";

    showHelp();

    ensureElevated();

    string scriptsPath = getScriptsPath();

    string command;
    string fileName;
    string data;
    string fileDirectory;

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
        else if (command == "HELP")         showHelp();
        else if (command == "EXIT")         exit(0);
        else if (command == "CREATEFILE") {
            cout << "Enter file name: ";
            getline(cin, fileName);
            createFile(fileName);
        }
        else if (command == "READFILE") {
            cout << "Enter file name: ";
            getline(cin, fileName);
            readFile(fileName);
        }
        else if (command == "WRITEFILE") {
            cout << "Enter file name: ";
            getline(cin, fileName);
            cout << "Enter data to write: ";
            getline(cin, data);
            writeFile(fileName, data);
        }
        else if (command == "LISTFILES") {
            cout << "Enter directory path: ";
            getline(cin, fileDirectory);
            listFiles(fileDirectory);
        }
        else if (command == "OPENFILE") {
            cout << "Enter file name: ";
            getline(cin, fileName);
            openFile(fileName);
        }
        else if (command == "QUERYATTRIBUTES") {
            cout << "Enter file name: ";
            getline(cin, fileName);
            queryFileAttributes(fileName);
        }
        else {
            cout << "Unknown command entered. Type 'help' to see available commands.\n";
        }
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


void createFile(const string& fileName) {
    string filePath = BASE_PATH + fileName;
    std::wstring wFilePath = stringToWString(filePath);
    HANDLE hFile = CreateFile(
        wFilePath.c_str(),
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

    cout << "File created successfully." << endl;
    CloseHandle(hFile);
}

void readFile(const string& fileName) {
    string filePath = BASE_PATH + fileName;
    std::wstring wFilePath = stringToWString(filePath);
    HANDLE hFile = CreateFile(
        wFilePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        cout << "Failed to open file: " << error << endl;
        return;
    }

    char buffer[1024];
    DWORD bytesRead;
    BOOL readResult = ReadFile(
        hFile,
        buffer,
        sizeof(buffer) - 1,
        &bytesRead,
        NULL
    );

    if (!readResult) {
        DWORD error = GetLastError();
        cout << "Failed to read file: " << error << endl;
    }
    else {
        buffer[bytesRead] = '\0';
        cout << "File content: " << buffer << endl;
    }

    CloseHandle(hFile);
}

void writeFile(const string& fileName, const string& data) {
    string filePath = BASE_PATH + fileName;
    std::wstring wFilePath = stringToWString(filePath);
    HANDLE hFile = CreateFile(
        wFilePath.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        cout << "Failed to open file: " << error << endl;
        return;
    }

    DWORD bytesWritten;
    BOOL writeResult = WriteFile(
        hFile,
        data.c_str(),
        static_cast<DWORD>(data.size()),
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

    CloseHandle(hFile);
}

void listFiles(const string& directoryPath) {
    std::wstring wDirectoryPath = stringToWString(directoryPath);
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((wDirectoryPath + L"\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        cout << "Failed to list files: " << error << endl;
        return;
    }

    do {
        wcout << findFileData.cFileName << endl;
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

void openFile(const string& fileName) {
    string filePath = BASE_PATH + fileName;
    std::wstring wFilePath = stringToWString(filePath);
    ShellExecute(NULL, L"open", wFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
 
void queryFileAttributes(const string& fileName) {
    string filePath = BASE_PATH + fileName;
    std::wstring wFilePath = stringToWString(filePath);
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (GetFileAttributesEx(wFilePath.c_str(), GetFileExInfoStandard, &fileInfo)) {
        cout << "File attributes for " << filePath << ":\n";
        cout << "Creation Time: " << fileInfo.ftCreationTime.dwLowDateTime << "\n";
        cout << "Last Access Time: " << fileInfo.ftLastAccessTime.dwLowDateTime << "\n";
        cout << "Last Write Time: " << fileInfo.ftLastWriteTime.dwLowDateTime << "\n";
        cout << "File Size: " << fileInfo.nFileSizeLow << " bytes\n";
    }
    else {
        DWORD error = GetLastError();
        cout << "Failed to get file attributes: " << error << endl;
    }
}

void showHelp() {
    cout << "Ultimate wfsDriver Utility (UwU) - Command List\n\n";

    cout << "install\t\t\t\t- Install the wfsDriver.\n";
    cout << "uninstall\t\t\t- Uninstall the wfsDriver.\n";
    cout << "createvdisk\t\t\t- Create a virtual disk.\n";
    cout << "removevdisk\t\t\t- Remove the virtual disk.\n";
    cout << "start\t\t\t\t- Start the driver.\n";
    cout << "stop\t\t\t\t- Stop the driver.\n";
    cout << "createfile <fileName>\t\t- Create a file.\n";
    cout << "readfile <fileName>\t\t- Read a file.\n";
    cout << "writefile <fileName> <data>\t- Write to a file.\n";
    cout << "listfiles <directoryPath>\t- List files in a directory.\n";
    cout << "openfile <fileName>\t\t- Open a file.\n";
    cout << "queryattributes <fileName>\t- Query file attributes.\n";
    cout << "help\t\t\t\t- Show this help message.\n";
    cout << "exit\t\t\t\t- Close UwU.\n";
}*/






#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <shlwapi.h>
#include <vector>
#include <algorithm>
#include <cctype>
#include <codecvt>

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
void queryFileInformation(HANDLE hFile, const string& queryType);
vector<string> listFilesInDirectory(const string& directory);
void queryFileFromList();

void createVirtualDisk(const string& scriptsPath);
void removeVirtualDisk(const string& scriptsPath);
void installDriver(const string& scriptsPath);
void uninstallDriver(const string& scriptsPath);
void startDriver(const string& scriptsPath);
void stopDriver(const string& scriptsPath);
void performDeviceOperations();
void createAndWriteNewFile();
void readFile();
void listFilesInVirtualDisk();
void deleteFileFromVirtualDisk(const std::string& fileName);
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
        else if (command == "WRITE")        createAndWriteNewFile();
        else if (command == "READ")         readFile();
        else if (command == "QUERYFILE")    queryFileFromList();
        else if (command == "LISTFILES")    listFilesInVirtualDisk();
        else if (command == "DELETE") {
            string fileName;
            cout << "Enter the full path of the file to delete (e.g.,W:\\writeTest.txt): ";
            getline(cin, fileName);
            deleteFileFromVirtualDisk(fileName);  // Call the delete function
        }
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

std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

void queryFileFromList() {
    // List the files in the virtual disk (W:\)
    std::vector<std::string> files = listFilesInDirectory("W:");
    if (files.empty()) {
        std::cout << "No files found in the directory.\n";
        return;
    }

    // Display the files to the user
    std::cout << "Files in W:\\:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << i + 1 << ". " << files[i] << std::endl;
    }

    // Ask the user to select a file by index
    int fileIndex = -1;
    std::cout << "Enter the number of the file you want to query: ";
    std::cin >> fileIndex;

    // Ensure the user input is valid
    if (fileIndex < 1 || fileIndex > files.size()) {
        std::cout << "Invalid selection.\n";
        return;
    }

    std::string selectedFile = files[fileIndex - 1];

    // Convert the selected file path from std::string to std::wstring
    std::wstring fullPath = stringToWString("W:\\" + selectedFile);

    // Open the selected file for querying information
    HANDLE hFile = CreateFile(
        fullPath.c_str(),  // Use wide-character string here
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::cout << "Failed to open file: " << error << std::endl;
        return;
    }

    HandleGuard fileGuard(hFile);

    // Prompt the user for the file information they want to query
    std::string queryType;
    std::cout << "Enter the type of file information to query (basic/standard): ";
    std::cin >> queryType;

    // Query file information based on the user input
    queryFileInformation(hFile, queryType);
}

std::vector<std::string> listFilesInDirectory(const std::string& directory) {
    WIN32_FIND_DATA findFileData;
    std::wstring wDirectory = stringToWString(directory);
    HANDLE hFind = FindFirstFile((wDirectory + L"\\*").c_str(), &findFileData);

    std::vector<std::string> files;

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cout << "No files found in the directory.\n";
        return files;
    }
    else {
        do {
            const std::wstring fileOrDir = findFileData.cFileName;

            // Exclude system/hidden files and folders
            if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ||
                (findFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ||
                fileOrDir == L"." || fileOrDir == L"..") {
                continue;  // Skip hidden/system folders
            }

            // Convert back to narrow string for display
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, fileOrDir.c_str(), (int)fileOrDir.size(), NULL, 0, NULL, NULL);
            std::string fileName(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, fileOrDir.c_str(), (int)fileOrDir.size(), &fileName[0], size_needed, NULL, NULL);

            files.push_back(fileName);
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
    }

    return files;
}

void PrintFileTime(const LARGE_INTEGER& largeIntegerTime) {
    // Convert LARGE_INTEGER to FILETIME
    FILETIME fileTime;
    fileTime.dwLowDateTime = largeIntegerTime.LowPart;
    fileTime.dwHighDateTime = largeIntegerTime.HighPart;

    SYSTEMTIME systemTime;

    // Convert the FILETIME to SYSTEMTIME (UTC)
    if (FileTimeToSystemTime(&fileTime, &systemTime)) {
        std::cout << systemTime.wYear << "-"
            << systemTime.wMonth << "-"
            << systemTime.wDay << " "
            << systemTime.wHour + 2 << ":"
            << systemTime.wMinute << ":"
            << systemTime.wSecond << std::endl;
    }
    else {
        std::cout << "Failed to convert FILETIME to SYSTEMTIME." << std::endl;
    }
}


void queryFileInformation(HANDLE hFile, const std::string& queryType) {
    if (queryType == "basic") {
        FILE_BASIC_INFO basicInfo;
        // Retrieve basic information
        if (GetFileInformationByHandleEx(hFile, FileBasicInfo, &basicInfo, sizeof(basicInfo))) {
            std::cout << "Creation Time: ";
            PrintFileTime(basicInfo.CreationTime);

            std::cout << "Last Access Time: ";
            PrintFileTime(basicInfo.LastAccessTime);

            std::cout << "Last Write Time: ";
            PrintFileTime(basicInfo.LastWriteTime);

            std::cout << "Change Time: ";
            PrintFileTime(basicInfo.ChangeTime);

            std::cout << "File Attributes: " << basicInfo.FileAttributes << std::endl;
        }
        else {
            DWORD error = GetLastError();
            std::cout << "Failed to query basic file information. Error code: " << error << std::endl;
        }
    }
    else if (queryType == "standard") {
        FILE_STANDARD_INFO standardInfo;
        if (GetFileInformationByHandleEx(hFile, FileStandardInfo, &standardInfo, sizeof(standardInfo))) {
            std::cout << "Allocation Size: " << standardInfo.AllocationSize.QuadPart << std::endl;
            std::cout << "End of File: " << standardInfo.EndOfFile.QuadPart << std::endl;
            std::cout << "Number of Links: " << standardInfo.NumberOfLinks << std::endl;
            std::cout << "Delete Pending: " << standardInfo.DeletePending << std::endl;
            std::cout << "Directory: " << (standardInfo.Directory ? "Yes" : "No") << std::endl;
        }
        else {
            DWORD error = GetLastError();
            std::cout << "Failed to query standard file information. Error code: " << error << std::endl;
        }
    }
    else {
        std::cout << "Unknown file information type entered. Please enter 'basic' or 'standard'." << std::endl;
    }
}

std::wstring stringToWstring(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

void writeFileToDevice(const std::string& filePath, const std::string& dataToWrite) {
    // Convert std::string to std::wstring
    std::wstring wideFilePath = stringToWstring(filePath);

    HANDLE hFile = CreateFile(
        wideFilePath.c_str(),  // Pass wide string here
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,  // Always create a new file or overwrite
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create or open file: " << GetLastError() << std::endl;
        return;
    }

    DWORD bytesWritten;
    BOOL success = WriteFile(
        hFile,
        dataToWrite.c_str(),
        static_cast<DWORD>(dataToWrite.size()),
        &bytesWritten,
        NULL
    );

    if (!success) {
        std::cerr << "Failed to write to file: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "Successfully wrote " << bytesWritten << " bytes to the file: " << filePath << std::endl;
    }

    CloseHandle(hFile);
}

void appendToFile(const std::string& filePath, const std::string& dataToAppend) {
    std::wstring wideFilePath = stringToWstring(filePath);

    HANDLE hFile = CreateFile(
        wideFilePath.c_str(),
        FILE_APPEND_DATA,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,  // Only open if the file exists
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file for appending: " << GetLastError() << std::endl;
        return;
    }

    DWORD bytesWritten;
    BOOL success = WriteFile(
        hFile,
        dataToAppend.c_str(),
        static_cast<DWORD>(dataToAppend.size()),
        &bytesWritten,
        NULL
    );

    if (!success) {
        std::cerr << "Failed to append data to file: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "Successfully appended " << bytesWritten << " bytes to the file." << std::endl;
    }

    CloseHandle(hFile);
}

// Function to allow user to input file name and write data
void createAndWriteNewFile() {
    // Get user input for file name
    std::string newFileName;
    std::cout << "Enter the full path and name of the new file (e.g., W:\\newTestFile.txt): ";
    std::getline(std::cin, newFileName);

    // Get user input for file content
    std::string fileContent;
    std::cout << "Enter the content to write to the file: ";
    std::getline(std::cin, fileContent);

    // Create the file and write the user's input content
    writeFileToDevice(newFileName, fileContent);

    // Ask if the user wants to append more data
    std::string appendChoice;
    std::cout << "Would you like to append more content to the file? (yes/no): ";
    std::getline(std::cin, appendChoice);

    if (appendChoice == "yes") {
        // Get user input for content to append
        std::string additionalData;
        std::cout << "Enter the content to append: ";
        std::getline(std::cin, additionalData);

        // Append the content to the file
        appendToFile(newFileName, additionalData);
    }

    std::cout << "Finished writing to the file." << std::endl;
}

void readFile() {
    DWORD bytesToRead = 1024;
    std::string PathToFile;
    std::cout << "Enter the full path and name of the file to read (e.g., W:\\helloworld.txt): ";
    std::getline(std::cin, PathToFile);
    std::wstring wideFilePath = stringToWstring(PathToFile);

    HANDLE hFile = CreateFile(
        wideFilePath.c_str(),
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file for reading: " << GetLastError() << std::endl;
        return;
    }

    std::vector<char> buffer(bytesToRead);
    DWORD bytesRead;
    BOOL success = ReadFile(
        hFile,
        buffer.data(),
        bytesToRead,
        &bytesRead,
        NULL
    );

    if (!success) {
        std::cerr << "Failed to read from file: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "Successfully read " << bytesRead << " bytes from the file: " << PathToFile << std::endl;
        std::cout << "Data: " << std::string(buffer.begin(), buffer.begin() + bytesRead) << std::endl;
    }

    CloseHandle(hFile);
}

void listFilesInVirtualDisk() {
    // List the files in the virtual disk (W:\)
    vector<string> files = listFilesInDirectory("W:");
    if (files.empty()) {
        cout << "No files found in the directory.\n";
        return;
    }

    // Display the files to the user
    cout << "Files in W:\\:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        cout << i + 1 << ". " << files[i] << endl;
    }
}

void queryFileInformation(HANDLE hFile) {
    FILE_BASIC_INFO basicInfo;
    if (GetFileInformationByHandleEx(hFile, FileBasicInfo, &basicInfo, sizeof(basicInfo))) {
        cout << "Creation Time: " << basicInfo.CreationTime.QuadPart << endl;
        cout << "Last Access Time: " << basicInfo.LastAccessTime.QuadPart << endl;
        cout << "File Attributes: " << basicInfo.FileAttributes << endl;
    }
    else {
        cout << "Failed to query file information." << endl;
    }
}

void deleteFileFromVirtualDisk(const std::string& fileName) {
    std::wstring wideFileName = std::wstring(fileName.begin(), fileName.end());

    // Open the file with DELETE access and share it for reading/writing/deletion
    HANDLE fileHandle = CreateFile(
        wideFileName.c_str(),
        DELETE,
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,  // Allow sharing for deletion
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::cerr << "Failed to open file for deletion, error code: " << error << std::endl;
        return;
    }

    // Attempt to delete the file
    BOOL deleteSuccess = DeleteFile(wideFileName.c_str());
    if (!deleteSuccess) {
        DWORD error = GetLastError();
        std::cerr << "Failed to delete file, error code: " << error << std::endl;
    }
    else {
        std::cout << "File successfully deleted: " << fileName << std::endl;
    }

    CloseHandle(fileHandle);
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

    // Query file information after writing
    //queryFileInformation(hFile);

    // Avoid naming conflict by renaming the second 'data' variable
    const char* filePath = "W:\\writeTest.txt";
    const std::string writeData = "Write test to the WFS driver!";

    writeFileToDevice(filePath, writeData);
    std::cout << "Finished writing to the file." << std::endl;
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
    cout << "write\t        - Perform write operations.\n";
    cout << "read\t        - Read file.\n";
    cout << "queryfile\t- See file inforamtion.\n";
    cout << "listfiles\t- See list of files.\n";
    cout << "delete\t\t- Delete a selected file.\n";
    cout << "help\t\t- Show this help message.\n";
    cout << "exit\t\t- Close UwU.\n";
}
