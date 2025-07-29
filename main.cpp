#include <iostream> // cout, hex, dec
#include <iomanip> // setw, setfill
#include <fstream> // fstream
#include <filesystem> // file_size
#include <cmath> // log
#include <windows.h>

#define UP "\033[1A"
#define DOWN "\033[1B"
#define LEFT "\033[1D"
#define RIGHT "\033[1C"
#define JUMP(r, c) ("\033[r,cF")
#define CLEAR "\033[1J"
#define RESET "\033[1H"

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    cursorInfo.dwSize = 1; // idk, someone says it crashes if you don't do this
    SetConsoleCursorInfo(out, &cursorInfo);
}

// like a sliding window that shows current range of bytes of a file
class viewer {
    std::fstream contents;
    size_t page = 0;
    size_t page_size = 16;
    int cursor_row = 0;
    int cursor_col = 0;
public:
    viewer() = default;
    explicit viewer(const char *fname) {
        // open in binary so it extracts by byte not character
        contents.open(fname,std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        if (!contents.is_open())
            return;
    }
    bool open(const char *fname){
        contents.open(fname,std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        if (!contents.is_open())
            return false;
        return true;
    }
    bool scrollUp(){
        if (page > 0) {
            page -= 1;
            return true;
        }
        return false;
    }
    bool scrollDown(){
        page += 1;
        contents.clear();
        contents.seekg(16*page*page_size-1);
        contents.get();
        if (contents.fail()) {
            page -= 1;
            return false;
        }
        contents.clear();
        contents.seekg(16*page);
        return true;
    }
    void print() {
        // file has nothing, do not read
        if (!contents.is_open())
            return;
        // preparation
        std::string line;
        char current;
        std::fstream::off_type i = 0, pages_passed = 0;
        contents.clear();
        contents.seekg(page*16);
        for (; pages_passed < page_size && contents.get(current); i = (i + 1) % 16) {
            if (i == 0)
                std::cout << std::hex << std::setfill('0') << std::setw(8) << (page+pages_passed)*16 << ": ";
            auto c = +(unsigned char)current;
            std::cout << std::setw(2) << c << ' ';
            line += (c < 32 ? '.' : current);
            if (i == 15) {
                pages_passed += 1;
                std::cout << "| " << std::dec << line << '\n';
//                if (pages_passed < page_size) {
//                    std::cout << std::hex;
//                    std::cout << std::setw(8) << (page+pages_passed)*16 << ": ";
//                }
                line.clear();
            }
        }
        // print remaining bytes on the right side
        if (i > 0)
            std::cout << std::string((16-i)*3,' ') << "| " << std::dec << line << '\n';
        std::cout << '\n';
    }
    bool exists() const {
        return contents.is_open();
    }
};

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "No arguments given, pass filename to view contents.\n";
        return -1;
    }
    auto dump = viewer(argv[1]);
    if (!dump.exists()) {
        std::cout << "File does not exist.\n";
        return -1;
    }
    dump.print();
    dump.scrollDown();
    dump.print();

    // Get the standard input handle.
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        std::cerr << "Error getting standard input handle." << std::endl;
        return 1;
    }

    DWORD cNumRead, cEvents = 0;//counts
    INPUT_RECORD irInBuf[128];//input buffer for window handle
    std::cout << "erm";
    enum Key { left = 37, up, right, down };
    bool run = true;
//    ShowConsoleCursor(false);
    while (run) {
        GetNumberOfConsoleInputEvents(hStdin, &cEvents);
        // Wait for keyboard events
        if (cEvents > 0) {
            // Peek at the events in the buffer
            PeekConsoleInput(hStdin, irInBuf, 128, &cNumRead);
            for (DWORD i = 0; i < cNumRead; ++i) {
                if (irInBuf[i].EventType == KEY_EVENT && irInBuf[i].Event.KeyEvent.bKeyDown) {
                    // Handle key press
                    auto key = irInBuf[i].Event.KeyEvent.wVirtualKeyCode;
                    //std::cout << '\r' << +key << "     ";
                    if (key == 'q' || key == 'Q'){
                        std::cout << "\nexiting..." << std::endl;
                        run = false;
                        break;
                    } else if (key == Key::left) {
                        std::cout << LEFT;
                    } else if (key == Key::right) {
                        std::cout << RIGHT;
                    } else if (key == Key::up) {
                        std::cout << UP;
                    } else if (key == Key::down) {
                        std::cout << DOWN;
                    }
                }
            }
            // Remove the events from the buffer.
            ReadConsoleInput(hStdin, irInBuf, cNumRead, &cNumRead);
        }
    }
//    ShowConsoleCursor(true);
    return 0;
}
