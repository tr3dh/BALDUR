#include<windows.h>
#include<functional>
#include<iostream>

// Minimaler Wrapper, um WM_DROPFILES zu verarbeiten
void ProcessWindowMessages(void* windowHandle, std::function<void(char**, int)>HandleDroppedFiles) {
    
    HWND hwnd = (HWND)windowHandle;

    MSG msg;
    while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_DROPFILES) {

            std::cout << "Drop" << std::endl;

            HDROP hDrop = (HDROP)msg.wParam;
            UINT count = DragQueryFileA(hDrop, 0xFFFFFFFF, NULL, 0);
            char *files[count];
            for (UINT i = 0; i < count; i++) {
                static char path[MAX_PATH];
                DragQueryFileA(hDrop, i, path, MAX_PATH);
                files[i] = path;
            }
            HandleDroppedFiles(files, count);
            DragFinish(hDrop);
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}