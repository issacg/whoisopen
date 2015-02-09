#include <node.h>
#include <nan.h>
#include <Windows.h>
#include <Psapi.h>
#ifdef METRO_APPS
#include <Appmodel.h>
#include <stdlib.h>
#endif

#ifdef METRO_APPS
typedef LONG WINAPI GetPackageFullNameFnType(_In_ HANDLE hProcess, _Inout_ UINT32 *packageFullNameLength,_Out_opt_ PWSTR packageFullName);
LONG WINAPI GetPackageFullNameFn(_In_ HANDLE hProcess, _Inout_ UINT32 *packageFullNameLength,_Out_opt_ PWSTR packageFullName)
{
	HMODULE mod = GetModuleHandle(TEXT("Kernel32.dll"));
	if(mod) {
			GetPackageFullNameFnType* pfnGetPackageFullName = reinterpret_cast<GetPackageFullNameFnType*>(GetProcAddress(mod,"GetPackageFullName"));
			if(pfnGetPackageFullName) {
					return pfnGetPackageFullName(hProcess, packageFullNameLength, packageFullName);
			}
	}
}
#endif

/* keep buffer an EVEN number */
#define BUFFER 256
using namespace v8;

class WhoIsOpenWorker : public NanAsyncWorker {
private:
	char sz[BUFFER];
        int HWND;

public:
	WhoIsOpenWorker(NanCallback* callback) : NanAsyncWorker(callback) {}
	~WhoIsOpenWorker() {}

        void WinError() {
                DWORD dw;
                LPVOID lpMsgBuf;
                dw = GetLastError();
                FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &lpMsgBuf,
                    0, NULL );

                SetErrorMessage((const char *)lpMsgBuf);
                LocalFree(lpMsgBuf);
        }

	void Execute() {
                DWORD pid;
                HANDLE hProc;
                GetWindowThreadProcessId(GetForegroundWindow(), &pid);
                hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, TRUE, pid);
                if (!hProc) {
                        WinError();
                        return;
                }
                if (GetModuleBaseName(hProc, NULL, sz, BUFFER) == 0) {
                        WinError();
                }
                #ifdef METRO_APPS
                if (lstrcmpi(sz, TEXT("wwahost.exe")) == 0) {
                    // Windows Runtime JS app
                    UINT32 len = BUFFER / 2;
                    WCHAR buf[BUFFER / 2];
                    if (GetPackageFullNameFn(hProc, &len, buf) != ERROR_SUCCESS) {
                        WinError();
                    } else {
                        wcstombs(sz, buf, BUFFER);
                    }

                }
                #endif
                CloseHandle(hProc);
	}

	void HandleOKCallback() {
		NanScope();
		v8::Handle<v8::Value> argv[] = {
			NanNull(),
			NanNew(sz)
		};

		callback->Call(2, argv);
    };
};

NAN_METHOD(WhoIsOpenAsync) {
	NanScope();

	if (args.Length() != 1) {
		 return NanThrowError("Invalid arguments");
	}

	if (!args[0]->IsFunction()) {
		 return NanThrowError("Missing callback");
	}

	NanCallback *callback = new NanCallback(args[0].As<Function>());
	NanAsyncQueueWorker(new WhoIsOpenWorker(callback));

	NanReturnUndefined();
}

void InitAll(Handle<Object> exports) {
	exports->Set(NanNew<String>("WhoIsOpenAsync"),
		 NanNew<FunctionTemplate>(WhoIsOpenAsync)->GetFunction());
}

NODE_MODULE(whoisopen, InitAll)