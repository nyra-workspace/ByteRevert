# Hex to File Converter

A lightweight Windows console application that converts hexadecimal text input into any file type, including .exe, .sys, .bin, or any other extension. Ideal for generating files from hex dumps or scripts.

## Features

- Reads hexadecimal input from a file or standard input.
- Supports 0x prefix and both uppercase and lowercase hex characters.
- Writes the parsed data to any specified output file with any extension.
- Pure Win32 API implementation, no external dependencies.
- Minimal and portable C++ code.

## Usage

ByteRevert.exe <HexInput.txt|-> <OutputFile>

- <HexInput.txt>: Path to a text file containing hexadecimal data.
- - : Read hexadecimal data from standard input.
- <OutputFile>: Path and name of the output file (any extension: .exe, .sys, .bin, etc.).

## Examples

ByteRevert.exe input.txt output.sys  
ByteRevert.exe - output.exe < input.txt

## Build

1. Open a terminal or Visual Studio Developer Command Prompt.
2. Compile using:

cl /EHsc ByteRevert.cpp

## Error Codes

| Code | Description |
|------|-------------|
| 1    | Incorrect usage / memory allocation failed |
| 2    | Failed to read input file |
| 3    | Failed to open output file |
| 4    | Failed to write output file |

## Notes

- Only valid hexadecimal characters (0-9, a-f, A-F) are processed.
- The program handles any file extension for the output.
- Standard input can be used by specifying - as the input file.
- Uses heap allocation internally to manage buffers.
