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

//				2. 剪切板

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

static void s2()
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
