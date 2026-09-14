// driver-defender.cpp
// Windows Admin Defender user-mode administration utility.
//
// This program intentionally keeps package installation/removal in the standard
// Windows Driver Store / Service Control mechanisms. It does not bypass
// Secure Boot, code-signing, Defender, Device Guard, or UAC.

#include <windows.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr wchar_t kCommandName[] = L"driver-defender";
constexpr wchar_t kProgramDirectory[] = L"WindowsAdminDefender";

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
        << L"  module     Show the support-module location\n"
        << L"  path       Show the installed command path\n";
}

int RunProcess(const std::wstring& executable,
               const std::wstring& arguments,
               bool waitForCompletion = true)
{
    std::wstring commandLine = L"\\\"" + executable + L"\\\"";
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
        std::wcerr << kCommandName << L": CreateProcess failed: "
                   << GetLastError() << L"\n";
        return static_cast<int>(GetLastError());
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

bool AddCommandDirectoryToMachinePath()
{
    // PATH modification is an installation-time administrator action. The
    // utility only adds its own ProgramData support directory if requested by
    // the installer; normal command execution does not mutate PATH.
    std::wcout << L"PATH installation directory: " << ProgramDataDirectory() << L"\n";
    return true;
}

int RunServiceCommand(const wchar_t* verb)
{
    return RunProcess(L"C:\\Windows\\System32\\sc.exe",
                      std::wstring(L" ") + verb + L" WindowsAdminDefender");
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

int RunStatus()
{
    return RunServiceCommand(L"query");
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
        return RunStatus();
    }
    if (command == L"support") {
        std::wcout << L"Windows Admin Defender administrator support\n"
                   << L"Driver service: WindowsAdminDefender\n"
                   << L"Support directory: " << ProgramDataDirectory() << L"\n"
                   << L"Installation/removal: PnPUtil / Driver Store\n";
        return 0;
    }
    if (command == L"module") {
        std::wcout << ProgramDataDirectory() << L"\n";
        return 0;
    }
    if (command == L"path") {
        return AddCommandDirectoryToMachinePath() ? 0 : ERROR_INSTALL_FAILURE;
    }

    PrintUsage();
    return ERROR_INVALID_PARAMETER;
}
