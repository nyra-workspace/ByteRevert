#include <windows.h>
#include <stdio.h>

int PrintUsage(const char* prog) {
    printf("Usage: %s <HexInput.txt|-> <Output.bin>\n", prog);
    return 1;
}

HANDLE OpenFileForRead(const char* path) {
    return CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

HANDLE CreateFileForWrite(const char* path) {
    return CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

BOOL IsHexChar(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

int HexValue(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    return 0;
}

int ReadAll(HANDLE file, BYTE** buf, DWORD* size) {
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(file, &fileSize)) return (int)GetLastError();

    DWORD len = (DWORD)fileSize.QuadPart;
    BYTE* temp = (BYTE*)HeapAlloc(GetProcessHeap(), 0, len + 1);
    if (!temp) return 2;

    DWORD read = 0;
    if (!ReadFile(file, temp, len, &read, NULL)) {
        HeapFree(GetProcessHeap(), 0, temp);
        return (int)GetLastError();
    }

    temp[read] = 0;
    *buf = temp;
    *size = read;
    return 0;
}

int ParseHex(const BYTE* text, DWORD len, BYTE** out, DWORD* outLen) {
    BYTE* temp = (BYTE*)HeapAlloc(GetProcessHeap(), 0, len);
    if (!temp) return 1;

    DWORD n = 0;
    for (DWORD i = 0; i < len; i++) {
        char c = (char)text[i];
        if (c == '0' && i + 1 < len && (text[i + 1] == 'x' || text[i + 1] == 'X')) { i++; continue; }
        if (IsHexChar(c)) temp[n++] = (BYTE)HexValue(c);
    }

    DWORD byteCount = n / 2;
    BYTE* outBuf = (BYTE*)HeapAlloc(GetProcessHeap(), 0, byteCount);
    if (!outBuf) { HeapFree(GetProcessHeap(), 0, temp); return 2; }

    for (DWORD i = 0, j = 0; j < byteCount; i += 2, j++)
        outBuf[j] = (BYTE)((temp[i] << 4) | temp[i + 1]);

    HeapFree(GetProcessHeap(), 0, temp);
    *out = outBuf;
    *outLen = byteCount;
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 3) return PrintUsage(argv[0]);

    HANDLE in = strcmp(argv[1], "-") == 0 ? GetStdHandle(STD_INPUT_HANDLE) : OpenFileForRead(argv[1]);
    if (in == INVALID_HANDLE_VALUE) { printf("Error opening input %lu\n", GetLastError()); return 2; }

    BYTE* txt = NULL;
    DWORD txtLen = 0;
    int r = ReadAll(in, &txt, &txtLen);
    if (strcmp(argv[1], "-") != 0) CloseHandle(in);
    if (r != 0) { printf("Error reading %d\n", r); return r; }

    BYTE* bin = NULL;
    DWORD binLen = 0;
    r = ParseHex(txt, txtLen, &bin, &binLen);
    HeapFree(GetProcessHeap(), 0, txt);
    if (r != 0) { printf("Error parsing %d\n", r); return r; }

    HANDLE out = CreateFileForWrite(argv[2]);
    if (out == INVALID_HANDLE_VALUE) { HeapFree(GetProcessHeap(), 0, bin); printf("Error opening output %lu\n", GetLastError()); return 3; }

    DWORD written = 0;
    if (!WriteFile(out, bin, binLen, &written, NULL) || written != binLen) {
        CloseHandle(out);
        HeapFree(GetProcessHeap(), 0, bin);
        printf("Error writing output\n");
        return 4;
    }

    CloseHandle(out);
    HeapFree(GetProcessHeap(), 0, bin);
    printf("Wrote %lu bytes to %s\n", (unsigned long)binLen, argv[2]);
    return 0;
}