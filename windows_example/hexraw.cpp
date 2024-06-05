#include <iostream>  
#include <iomanip>  
#include <windows.h>  
  
bool ReadSerialPort(const char* portName, int baudRate) {  
    HANDLE hSerial;  
    DCB dcbSerialParams = { 0 };  
    COMMTIMEOUTS timeouts = { 0 };  
    char buffer[256] = { 0 };  
    DWORD bytesRead;  
  
    // 打开串口  
    hSerial = CreateFileA(portName,  
        GENERIC_READ | GENERIC_WRITE,  
        0,  
        NULL,  
        OPEN_EXISTING,  
        FILE_ATTRIBUTE_NORMAL,  
        NULL);  
  
    if (hSerial == INVALID_HANDLE_VALUE) {  
        std::cerr << "Error opening serial port: " << GetLastError() << std::endl;  
        return false;  
    }  
  
    // 配置串口参数（略去配置过程，与之前的示例相同）  
    // ...  
  
    // 设置读取超时（可以根据需要调整）  
    timeouts.ReadIntervalTimeout = MAXDWORD; // 非阻塞模式，设置为最大值  
    timeouts.ReadTotalTimeoutConstant = 50;  // 等待50毫秒  
    if (!SetCommTimeouts(hSerial, &timeouts)) {  
        std::cerr << "Error setting serial port timeouts: " << GetLastError() << std::endl;  
        CloseHandle(hSerial);  
        return false;  
    }  
  
    // 读取串口数据的循环  
    bool running = true;  
    while (running) {  
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {  
            if (bytesRead > 0) {  
                // 以16进制打印数据  
                for (DWORD i = 0; i < bytesRead; ++i) {  
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";  
                }  
                std::cout << std::endl;  
            }  
        } else {  
            // 检查是否是超时错误（ERROR_IO_PENDING 或 ERROR_MORE_DATA）  
            DWORD err = GetLastError();  
            if (err != ERROR_IO_PENDING && err != ERROR_MORE_DATA) {  
                std::cerr << "Error reading from serial port: " << err << std::endl;  
                running = false;  
            }  
        }  
  
        // 在这里可以添加一些延迟来减少CPU使用率，例如Sleep(10);  
    }  
  
    // 关闭串口  
    CloseHandle(hSerial);  
    return true;  
}  
  
int main() {  
    if (ReadSerialPort("COM4", 115200)) {  
        std::cout << "Serial port read successfully." << std::endl;  
    } else {  
        std::cout << "Failed to read serial port." << std::endl;  
    }  
    return 0;  
}