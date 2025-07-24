#include <iostream> // cout, hex, dec
#include <iomanip> // setw, setfill
#include <fstream> // fstream
#include <filesystem> // file_size
#include <cmath> // log
#include <SDL3/SDL.h>
#include <fmt/core.h>

/* procedure:
 - check if parameter is given
 - check filename exists
 - open file
 - view contents between a range
 - quit by pressing File > Quit
*/

#define JUMP(r, c) ("\033[r,cF")
#define CLEAR "\033[1J"
#define RESET "\033[1H"

// like a sliding window that shows current range of bytes of a file
class viewer {
    std::fstream contents;
    std::fstream::off_type page = 0;
    size_t page_size = 16;
    int line_width = 2;
public:
    viewer() = default;
    explicit viewer(const char *fname) {
        // open in binary so it extracts by byte not character
        contents.open(fname,std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        if (!contents.is_open())
            return;
        // figure out contents size when file exists
        line_width = (int)std::log10(std::filesystem::file_size(fname));
    }
    bool open(const char *fname){
        contents.open(fname,std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        if (!contents.is_open())
            return false;
        // figure out contents size when file exists
        line_width = (int)std::log10(std::filesystem::file_size(fname));
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
        contents.seekg(page*16);
        if (contents.tellg() == -1){
            page -= 1;
            return false;
        }
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
        std::cout << std::hex;
        std::cout << std::setfill(' ') << std::setw(line_width) << page*16 << " : ";
        std::cout << std::setfill('0');
        for (; pages_passed < page_size && contents.get(current); i = (i + 1) % 16) {
            auto c = +(unsigned char)current;
            std::cout << std::setw(2) << c << ' ';
            line += (c < 32 ? '.' : current);
            if (i == 15) {
                pages_passed += 1;
                std::cout << "| " << std::dec << line << '\n';
                if (pages_passed < page_size) {
                    std::cout << std::hex;
                    std::cout << std:: setfill(' ') << std::setw(line_width) << (page+pages_passed)*16 << " : ";
                    std::cout << std::setfill('0');
                }
                line.clear();
            }
        }
        // print remaining bytes on the right side
        if (i > 0)
            std::cout << std::string((16-i)*3,' ') << "| " << std::dec << line << '\n';
    }
    bool exists() const {
        return contents.is_open();
    }
};

int main(int argc, char* argv[]) {
    fmt::print("fmt works!!{}\n",1);
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
    std::cout << "scrolling down one page\n";
    dump.scrollDown();
    dump.print();
}
