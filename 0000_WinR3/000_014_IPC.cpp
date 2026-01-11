#include"common.h"

//				1. 管道
//				2. 剪切板


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				1. 管道

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


#define BUFSIZE 512
#define PIPE_NAME TEXT("\\\\.\\pipe\\mynamedpipe")

DWORD  pipeServer(LPVOID lpParameter)
{
    HANDLE hPipe;
    TCHAR chBuf[BUFSIZE];
    TCHAR lpvMessage[] = TEXT("Default message from server");
    BOOL fSuccess = FALSE;
    DWORD cbRead, cbToWrite, cbWritten;

    // 创建命名管道
    hPipe = CreateNamedPipe(
        PIPE_NAME,                  // 管道名称
        PIPE_ACCESS_DUPLEX,         // 读写访问
        PIPE_TYPE_MESSAGE |         // 消息类型管道
        PIPE_READMODE_MESSAGE |     // 消息读取模式
        PIPE_WAIT,                  // 阻塞模式
        1,                          // 最大实例数
        BUFSIZE,                    // 输出缓冲区大小
        BUFSIZE,                    // 输入缓冲区大小
        0,                          // 默认超时时间
        NULL);                      // 默认安全属性

    if (hPipe == INVALID_HANDLE_VALUE) {
        _tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError());
        return 1;
    }

    _tprintf(TEXT("Waiting for client to connect...\n"));

    // 等待客户端连接
    fSuccess = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (fSuccess)
    {
        _tprintf(TEXT("Client connected.\n"));

        // 从客户端读取数据
        while (1) {
            fSuccess = ReadFile(
                hPipe,        // 管道句柄
                chBuf,        // 接收数据的缓冲区
                BUFSIZE * sizeof(TCHAR), // 缓冲区大小
                &cbRead,     // 实际读取的字节数
                NULL);        // 非重叠I/O

            if (!fSuccess || cbRead == 0) {
                if (GetLastError() == ERROR_BROKEN_PIPE) {
                    _tprintf(TEXT("Client disconnected.\n"));
                }
                else {
                    _tprintf(TEXT("ReadFile failed, GLE=%d.\n"), GetLastError());
                }
                break;
            }

            _tprintf(TEXT("Received from client: %s\n"), chBuf);

            // 向客户端发送响应
            cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
            fSuccess = WriteFile(
                hPipe,         // 管道句柄
                lpvMessage,   // 消息
                cbToWrite,    // 消息长度
                &cbWritten,   // 实际写入的字节数
                NULL);        // 非重叠I/O

            if (!fSuccess) {
                _tprintf(TEXT("WriteFile failed, GLE=%d.\n"), GetLastError());
                break;
            }
        }
    }
    else {
        CloseHandle(hPipe);
        _tprintf(TEXT("ConnectNamedPipe failed, GLE=%d.\n"), GetLastError());
        return 1;
    }

    // 关闭管道句柄
    CloseHandle(hPipe);

    _tprintf(TEXT("Server exiting.\n"));
    return 0;
}

DWORD pipeClient(LPVOID lpParameter)
{
    HANDLE hPipe;
    TCHAR lpvMessage[] = TEXT("Default message from client");
    TCHAR chBuf[BUFSIZE];
    BOOL fSuccess = FALSE;
    DWORD cbRead, cbToWrite, cbWritten, dwMode;

    // 尝试连接到命名管道实例
    while (1) {
        hPipe = CreateFile(
            PIPE_NAME,      // 管道名称
            GENERIC_READ |  // 读访问
            GENERIC_WRITE,  // 写访问
            0,              // 不共享
            NULL,           // 默认安全属性
            OPEN_EXISTING,  // 只打开已存在的管道
            0,              // 默认属性
            NULL);          // 无模板文件

        // 如果管道连接成功，退出循环
        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        // 如果错误不是ERROR_PIPE_BUSY，则失败
        if (GetLastError() != ERROR_PIPE_BUSY) {
            _tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
            return 1;
        }

        // 所有管道实例都忙，等待20秒后重试
        if (!WaitNamedPipe(PIPE_NAME, 20000)) {
            _tprintf(TEXT("Could not open pipe: 20 second wait timed out."));
            return 1;
        }
    }

    _tprintf(TEXT("Connected to pipe.\n"));

    // 管道连接成功，设置读取模式为消息模式
    dwMode = PIPE_READMODE_MESSAGE;
    fSuccess = SetNamedPipeHandleState(
        hPipe,    // 管道句柄
        &dwMode,  // 新的管道模式
        NULL,     // 不设置最大字节数
        NULL);    // 不设置最大时间

    if (!fSuccess) {
        _tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
        return 1;
    }

    // 向服务器发送消息
    cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
    _tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

    fSuccess = WriteFile(
        hPipe,                  // 管道句柄
        lpvMessage,             // 消息
        cbToWrite,              // 消息长度
        &cbWritten,             // 实际写入的字节数
        NULL);                  // 非重叠I/O

    if (!fSuccess) {
        _tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
        return 1;
    }

    _tprintf(TEXT("Message sent to server, receiving reply...\n"));

    // 从管道读取服务器响应
    do {
        fSuccess = ReadFile(
            hPipe,    // 管道句柄
            chBuf,    // 接收数据的缓冲区
            BUFSIZE * sizeof(TCHAR), // 缓冲区大小
            &cbRead,  // 实际读取的字节数
            NULL);    // 非重叠I/O

        if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
            break;

        _tprintf(TEXT("Received from server: %s\n"), chBuf);
    } while (!fSuccess);  // 重复读取，直到没有更多数据

    if (!fSuccess) {
        _tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
        return 1;
    }

    _tprintf(TEXT("Closing pipe.\n"));
    CloseHandle(hPipe);

    _tprintf(TEXT("Client exiting.\n"));
    return 0;
}

void s1()
{
    HANDLE hThreadServer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pipeServer, NULL, 0, NULL);
    HANDLE hThreadClient = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pipeClient, NULL, 0, NULL);

    WaitForSingleObject(hThreadServer, INFINITE);
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				2. 匿名管道

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#if 0
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// 管道缓冲区大小
#define BUFSIZE 4096

// 错误处理宏
#define CHECK_ERROR(expr, msg) \
    if (!(expr)) { \
        printf(msg, GetLastError()); \
        goto Cleanup; \
    }

int _tmain(int argc, TCHAR* argv[])
{
    // 1. 定义管道句柄（双向通信需要两个管道）
    HANDLE hParentWritePipe = NULL;  // 父写 -> 子读
    HANDLE hParentReadPipe = NULL;   // 父读 <- 子写
    HANDLE hChildReadPipe = NULL;    // 子读 <- 父写
    HANDLE hChildWritePipe = NULL;   // 子写 -> 父读

    // 2. 定义进程相关结构
    PROCESS_INFORMATION piProcInfo = { 0 };
    STARTUPINFO siStartInfo = { 0 };
    BOOL bSuccess = FALSE;
    DWORD dwRead, dwWritten;
    CHAR chReadBuf[BUFSIZE] = { 0 };
    CHAR chWriteBuf[] = "父进程：你好，子进程！";

    // 3. 创建第一个管道（父写子读）
    SECURITY_ATTRIBUTES saAttr = { 0 };
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;  // 允许句柄继承（关键）
    saAttr.lpSecurityDescriptor = NULL;

    CHECK_ERROR(CreatePipe(&hChildReadPipe, &hParentWritePipe, &saAttr, 0),
        "创建父写子读管道失败，错误码：%d\n");

    // 4. 创建第二个管道（子写父读）
    CHECK_ERROR(CreatePipe(&hParentReadPipe, &hChildWritePipe, &saAttr, 0),
        "创建子写父读管道失败，错误码：%d\n");

    // 5. 设置子进程的启动信息（重定向标准输入/输出）
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdInput = hChildReadPipe;    // 子进程标准输入 = 父写子读管道的读端
    siStartInfo.hStdOutput = hChildWritePipe;  // 子进程标准输出 = 子写父读管道的写端
    siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;  // 启用标准句柄重定向

    // 6. 创建子进程（子进程是当前程序本身，通过参数区分父子）
    TCHAR szCmdline[] = _T("ChildProcess.exe");  // 子进程可执行文件（需编译为同名exe）
    // 若要调试，可改为当前程序路径，通过 argc 判断：if (argc > 1) 则为子进程逻辑
    CHECK_ERROR(CreateProcess(NULL, szCmdline, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo),
        "创建子进程失败，错误码：%d\n");

    // ====================== 父进程逻辑 ======================
    printf("父进程：开始向子进程发送消息...\n");
    // 7. 父进程向子进程写入数据
    CHECK_ERROR(WriteFile(hParentWritePipe, chWriteBuf, lstrlenA(chWriteBuf), &dwWritten, NULL),
        "父进程写入管道失败，错误码：%d\n");
    printf("父进程：已发送消息：%s\n", chWriteBuf);

    // 8. 父进程读取子进程的回复
    CHECK_ERROR(ReadFile(hParentReadPipe, chReadBuf, BUFSIZE, &dwRead, NULL),
        "父进程读取管道失败，错误码：%d\n");
    printf("父进程：收到子进程回复：%s\n", chReadBuf);

    bSuccess = TRUE;

Cleanup:
    // 9. 关闭所有句柄（避免资源泄漏）
    if (hParentWritePipe) CloseHandle(hParentWritePipe);
    if (hParentReadPipe) CloseHandle(hParentReadPipe);
    if (hChildReadPipe) CloseHandle(hChildReadPipe);
    if (hChildWritePipe) CloseHandle(hChildWritePipe);
    if (piProcInfo.hProcess) CloseHandle(piProcInfo.hProcess);
    if (piProcInfo.hThread) CloseHandle(piProcInfo.hThread);

    if (!bSuccess)
        return 1;

    return 0;
}

// ====================== 子进程逻辑（单独编译为 ChildProcess.exe） ======================
// 也可在同一个程序中通过参数区分，比如：if (argc > 1) 执行子进程逻辑
int ChildMain()
{
    CHAR chReadBuf[BUFSIZE] = { 0 };
    CHAR chWriteBuf[] = "子进程：收到消息，你好父进程！";
    DWORD dwRead, dwWritten;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);   // 子进程标准输入 = 父写子读管道
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); // 子进程标准输出 = 子写父读管道

    // 1. 子进程读取父进程发送的数据
    if (!ReadFile(hStdin, chReadBuf, BUFSIZE, &dwRead, NULL)) {
        printf("子进程读取管道失败，错误码：%d\n", GetLastError());
        return 1;
    }
    printf("子进程：收到父进程消息：%s\n", chReadBuf);

    // 2. 子进程向父进程回复数据
    if (!WriteFile(hStdout, chWriteBuf, lstrlenA(chWriteBuf), &dwWritten, NULL)) {
        printf("子进程写入管道失败，错误码：%d\n", GetLastError());
        return 1;
    }

    return 0;
}
#endif


// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//				3. 剪切板

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s3()
{
    // =========================== 剪切板写入
    const char* text = "Hello, 222Clipboard111!";
    int len = (int)strlen(text) + 1;

    // 打开剪切板
    if (!OpenClipboard(NULL))
        return;

    // 清空剪切板并获取所有权
    EmptyClipboard();

    // 分配全局内存（+1 用于终止符）
    HGLOBAL hMem = GlobalAlloc(GHND, len);
    if (!hMem)
    {
        CloseClipboard();
        return;
    }

    // 锁定内存并写入数据
    char* pMem = static_cast<char*>(GlobalLock(hMem));
    if (!pMem)
    {
        GlobalFree(hMem);
        CloseClipboard();
        return;
    }
    strcpy_s(pMem, len, text);
    GlobalUnlock(hMem);

    // 将数据句柄设置到剪切板
    SetClipboardData(CF_TEXT, hMem);

    // 关闭剪切板
    CloseClipboard();


    // =========================== 剪切板读取

    if (!OpenClipboard(NULL))
        return;

    if (!IsClipboardFormatAvailable(CF_TEXT))
    {
        CloseClipboard();
        return;
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (!hData)
    {
        CloseClipboard();
        return;
    }

    char* pData = static_cast<char*>(GlobalLock(hData));
    if (!pData)
    {
        CloseClipboard();
        return;
    }

    printf("Clipboard Text: %s\n", pData);

    printf("Clipboard Text size: %d\n", (int)GlobalSize(hData));

    GlobalUnlock(hData);

    CloseClipboard();
}



void p000_014()
{

}
