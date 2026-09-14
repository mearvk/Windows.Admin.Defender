// driver-defender.cpp
// Windows Admin Defender administrator utility.
//
// The kernel component uses a Windows file-system minifilter to deny handles
// opened directly on protected directories. Normal file opens and reads remain
// available, while directory-handle based enumeration, copying, deletion, and
// rename operations are denied.

#include <windows.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr wchar_t kCommandName[] = L"driver-defender";
constexpr wchar_t kProgramDirectory[] = L"WindowsAdminDefender";
constexpr wchar_t kServiceName[] = L"WindowsAdminDefender";
constexpr wchar_t kDriverPath[] =
    L"C:\\Windows\\System32\\drivers\\WindowsAdminDefender.sys";

void PrintUsage()
{
    std::wcout
        << L"Windows Admin Defender\n\n"
        << L"Usage: driver-defender <command>\n\n"
        << L"Commands:\n"
        << L"  load       Start the installed driver service\n"
        << L"  unload     Stop the driver service\n"
        << L"  add        Add/install a driver package with PnPUtil\n"
        << L"  install    Alias for add\n"
        << L"  remove     Remove the installed package with PnPUtil\n"
        << L"  delete     Alias for remove\n"
        << L"  uninstall  Alias for remove\n"
        << L"  status     Query the driver service\n"
        << L"  support    Show administrator support information\n"
        << L"  module     Show the driver module location\n"
        << L"  path       Show the installed support directory\n"
        << L"  policy     Show the directory-lock policy\n";
}

int RunProcess(const std::wstring& executable,
               const std::wstring& arguments,
               bool waitForCompletion = true)
{
    std::wstring commandLine = L"\"" + executable + L"\"";
    if (!arguments.empty()) {
        commandLine += L" " + arguments;
    }

    std::vector<wchar_t> mutableCommand(commandLine.begin(), commandLine.end());
    mutableCommand.push_back(L'\0');

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInfo{};

    if (!CreateProcessW(nullptr,
                        mutableCommand.data(),
                        nullptr,
                        nullptr,
                        FALSE,
                        0,
                        nullptr,
                        nullptr,
                        &startupInfo,
                        &processInfo)) {
        DWORD error = GetLastError();
        std::wcerr << kCommandName << L": CreateProcess failed: "
                   << error << L"\n";
        return static_cast<int>(error);
    }

    if (!waitForCompletion) {
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return 0;
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    DWORD exitCode = 1;
    GetExitCodeProcess(processInfo.hProcess, &exitCode);
    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return static_cast<int>(exitCode);
}

std::wstring ProgramDataDirectory()
{
    wchar_t buffer[MAX_PATH]{};
    DWORD length = GetEnvironmentVariableW(L"ProgramData", buffer, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        return L"C:\\ProgramData\\" + std::wstring(kProgramDirectory);
    }
    return std::wstring(buffer) + L"\\" + kProgramDirectory;
}

int RunServiceCommand(const wchar_t* verb)
{
    return RunProcess(L"C:\\Windows\\System32\\sc.exe",
                      std::wstring(verb) + L" " + kServiceName);
}

int RunPnPUtilAdd()
{
    std::wcout << L"Enter the full path to WindowsAdminDefender.inf: ";
    std::wstring infPath;
    std::getline(std::wcin, infPath);

    if (infPath.empty() || !std::filesystem::exists(infPath)) {
        std::wcerr << L"driver-defender: INF path does not exist.\n";
        return ERROR_FILE_NOT_FOUND;
    }

    return RunProcess(L"C:\\Windows\\System32\\pnputil.exe",
                      L"/add-driver \"" + infPath + L"\" /install");
}

int RunPnPUtilRemove()
{
    std::wcout << L"Enter the published INF name (for example, oem42.inf): ";
    std::wstring publishedInf;
    std::getline(std::wcin, publishedInf);

    if (publishedInf.empty()) {
        std::wcerr << L"driver-defender: published INF name is required.\n";
        return ERROR_INVALID_PARAMETER;
    }

    return RunProcess(L"C:\\Windows\\System32\\pnputil.exe",
                      L"/delete-driver \"" + publishedInf + L"\" /uninstall");
}

void PrintPolicy()
{
    std::wcout
        << L"Directory-lock policy:\n"
        << L"  Protected roots: Windows system/application data directories\n"
        << L"  Directory handles: denied\n"
        << L"  Normal file opens/reads: permitted\n"
        << L"  Directory enumeration/copy/delete/rename: denied\n"
        << L"  Enforcement: Windows file-system minifilter\n"
        << L"  Administrative installation: Driver Store / PnPUtil\n";
}

} // namespace

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 2) {
        PrintUsage();
        return ERROR_INVALID_PARAMETER;
    }

    const std::wstring command = argv[1];

    if (command == L"load") {
        return RunServiceCommand(L"start");
    }
    if (command == L"unload") {
        return RunServiceCommand(L"stop");
    }
    if (command == L"add" || command == L"install") {
        return RunPnPUtilAdd();
    }
    if (command == L"remove" || command == L"delete" || command == L"uninstall") {
        return RunPnPUtilRemove();
    }
    if (command == L"status") {
        return RunServiceCommand(L"query");
    }
    if (command == L"policy") {
        PrintPolicy();
        return 0;
    }
    if (command == L"support") {
        std::wcout
            << L"Windows Admin Defender administrator support\n"
            << L"Driver service: " << kServiceName << L"\n"
            << L"Driver module: " << kDriverPath << L"\n"
            << L"Support directory: " << ProgramDataDirectory() << L"\n"
            << L"Enforcement: directory-handle denial with normal file reads preserved\n";
        return 0;
    }
    if (command == L"module") {
        std::wcout << kDriverPath << L"\n";
        return 0;
    }
    if (command == L"path") {
        std::wcout << ProgramDataDirectory() << L"\n";
        return 0;
    }

    PrintUsage();
    return ERROR_INVALID_PARAMETER;
}
