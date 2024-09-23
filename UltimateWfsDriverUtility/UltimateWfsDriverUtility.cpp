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
    std::cout << "\n";
    displayAscii("../../UltimateWfsDriverUtility/banner.txt");
    std::cout << "\n\t\t\t Ultimate wfsDriver Utility\n\n\n";

    showHelp();

    ensureElevated();

    std::string scriptsPath = getScriptsPath();
    std::string command;

    while (true) {
        cout << "\n>> ";
        getline(cin, command); 
        cout << "\n";

        command.erase(std::remove_if(command.begin(), command.end(), ::isspace), command.end());

        if (command.empty()) {
            continue;
        }

        std::transform(command.begin(), command.end(), command.begin(), ::toupper);

        if (command == "INSTALL") {
            installDriver(scriptsPath);
        }
        else if (command == "UNINSTALL") {
            uninstallDriver(scriptsPath);
        }
        else if (command == "CREATEVDISK") {
            createVirtualDisk(scriptsPath);
        }
        else if (command == "REMOVEVDISK") {
            removeVirtualDisk(scriptsPath);
        }
        else if (command == "START") {
            startDriver(scriptsPath);
        }
        else if (command == "STOP") {
            stopDriver(scriptsPath);
        }
        else if (command == "DEVICEOPS") {
            performDeviceOperations();
        }
        else if (command == "WRITE") {
            createAndWriteNewFile();
        }
        else if (command == "READ") {
            readFile();
        }
        else if (command == "QUERYFILE") {
            queryFileFromList();
            continue; 
        }
        else if (command == "LISTFILES") {
            listFilesInVirtualDisk();
        }
        else if (command == "DELETE") {
            std::string fileName;
            std::cout << "Enter the full path of the file to delete (e.g., W:\\writeTest.txt): ";
            std::getline(std::cin, fileName);
            deleteFileFromVirtualDisk(fileName);
        }
        else if (command == "HELP") {
            showHelp();
        }
        else if (command == "EXIT") {
            exit(0);
        }
        else {
            std::cout << "Unknown command entered. Type 'help' to see available commands.\n";
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

std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

void queryFileFromList() {
    std::vector<std::string> files = listFilesInDirectory("W:");
    if (files.empty()) {
        std::cout << "No files found in the directory.\n";
        return;  
    }


    std::cout << "Files in W:\\:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << i + 1 << ". " << files[i] << std::endl;
    }


    int fileIndex = -1;
    std::cout << "Enter the number of the file you want to query: ";
    std::cin >> fileIndex;


    if (fileIndex < 1 || fileIndex > files.size()) {
        std::cout << "Invalid selection.\n";
        return;  
    }

    std::string selectedFile = files[fileIndex - 1];

    std::wstring fullPath = stringToWString("W:\\" + selectedFile);


    HANDLE hFile = CreateFile(
        fullPath.c_str(),  
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

    std::string queryType;
    std::cout << "Enter the type of file information to query (basic/standard): ";
    std::cin >> queryType;

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

            if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ||
                (findFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ||
                fileOrDir == L"." || fileOrDir == L"..") {
                continue;  
            }

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
    FILETIME fileTime;
    fileTime.dwLowDateTime = largeIntegerTime.LowPart;
    fileTime.dwHighDateTime = largeIntegerTime.HighPart;

    SYSTEMTIME systemTime;

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

bool writeFileToDevice(const std::string& filePath, const std::string& dataToWrite) {

    std::wstring wideFilePath = stringToWstring(filePath);

    HANDLE hFile = CreateFile(
        wideFilePath.c_str(),  
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create or open file: " << GetLastError() << std::endl;
        return false; 
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
        CloseHandle(hFile);
        return false;
    }
    else {
        std::cout << "Successfully wrote " << bytesWritten << " bytes to the file: " << filePath << std::endl;
    }

    CloseHandle(hFile);
    return true;  
}


bool appendToFile(const std::string& filePath, const std::string& dataToAppend) {
    std::wstring wideFilePath = stringToWstring(filePath);

    HANDLE hFile = CreateFile(
        wideFilePath.c_str(),
        FILE_APPEND_DATA,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file for appending: " << GetLastError() << std::endl;
        return false; 
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
        CloseHandle(hFile);
        return false;  
    }
    else {
        std::cout << "Successfully appended " << bytesWritten << " bytes to the file." << std::endl;
    }

    CloseHandle(hFile);
    return true; 
}

void createAndWriteNewFile() {
   
    std::string newFileName;
    std::cout << "Enter the full path and name of the new file (e.g., W:\\newTestFile.txt): ";
    std::getline(std::cin, newFileName);

    
    HANDLE hFile = CreateFile(
        stringToWstring(newFileName).c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create or open file: " << GetLastError() << std::endl;
        std::cerr << "Error occurred. Aborting further operations." << std::endl;
        return; 
    }

    CloseHandle(hFile);

  
    std::string fileContent;
    std::cout << "Enter the content to write to the file: ";
    std::getline(std::cin, fileContent);

 
    if (!writeFileToDevice(newFileName, fileContent)) {
        std::cerr << "Error occurred. Aborting further operations." << std::endl;
        return; 
    }


    std::string appendChoice;
    std::cout << "Would you like to append more content to the file? (yes/no): ";
    std::getline(std::cin, appendChoice);

    if (appendChoice == "yes") {
     
        std::string additionalData;
        std::cout << "Enter the content to append: ";
        std::getline(std::cin, additionalData);

        if (!appendToFile(newFileName, additionalData)) {
            std::cerr << "Failed to append content. Aborting." << std::endl;
            return;  
        }
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

    vector<string> files = listFilesInDirectory("W:");
    if (files.empty()) {
        cout << "No files found in the directory.\n";
        return;
    }

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


    HANDLE fileHandle = CreateFile(
        wideFileName.c_str(),
        DELETE,
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 
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

    const char* filePath = "W:\\writeTest.txt";
    const std::string writeData = "Write test to the WFS driver!";

    writeFileToDevice(filePath, writeData);
    std::cout << "Finished writing to the file." << std::endl;
}

void showHelp() {
    cout << "Ultimate wfsDriver Utility (UwU) - Command List\n\n";

    cout << "UwU initialization comments.\n";
    cout << "------------------------------------------\n";
    cout << "install\t\t- Install the wfsDriver.\n";
    cout << "uninstall\t- Uninstall the wfsDriver.\n";
    cout << "createvdisk\t- Create a virtual disk.\n";
    cout << "removevdisk\t- Remove the virtual disk.\n";
    cout << "start\t\t- Start the drive.\n";
    cout << "stop\t\t- Stop the driver.\n";
    cout << "deviceops\t- Perform device operations.\n\n";
    cout << "UwU Features.\n";
    cout << "------------------------------------------\n";
    cout << "write\t        - Perform write operations.\n";
    cout << "read\t        - Read file.\n";
    cout << "queryfile\t- See file inforamtion.\n";
    cout << "listfiles\t- See list of files.\n";
    cout << "delete\t\t- Delete a selected file.\n";
    cout << "help\t\t- Show this help message.\n";
    cout << "exit\t\t- Close UwU.\n";
}