#include <node.h>
#include <nan.h>
#include <Windows.h>
#include <Psapi.h>

#define BUFFER 255
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