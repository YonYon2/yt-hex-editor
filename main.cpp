#include <iostream> // cout, hex, dec
#include <iomanip> // setw, setfill
#include <fstream> // fstream
#include <chrono> // high_resolution_clock, duration_cast, nanoseconds

/* procedure:
 - check if parameter is given
 - check filename exists
 - open file
 - view contents between a range
 - quit by pressing File > Quit
*/

// like a sliding window that shows current range of bytes of a file
class viewer {
    std::fstream contents;
    std::istreambuf_iterator<char> start;
    std::istreambuf_iterator<char> end;
    size_t page_size = 16;
public:
    viewer() = default;
    viewer(const char* fname) {
        // open in binary so it extracts by byte not character
        contents.open(fname,std::ios_base::in | std::ios_base::out | std::ios_base::binary);
        if (!contents)
            return;

    }
    std::string view(){
        // file does not exist, do not read
        if (!contents)
                return "";
        auto res = "";
        while (start != end){
            ++start;
        }
        return res;
    }
    [[nodiscard]] bool exists() const{
        return (bool)contents;
    }
};

// something to measure speed with
template <class F, class... A>
std::chrono::nanoseconds measure(F func, A&&... args){
    auto start = std::chrono::high_resolution_clock::now();
    func(std::forward<A>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "No arguments given, pass filename to view contents.\n";
        return -1;
    }
    std::fstream dump(argv[1], std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    if (!dump) {
        std::cout << "File does not exist.\n";
        return -1;
    }

    char current;
    std::string line;
    int pos = 0;
    // good for one time use, but find a way to print without emptying fstream
    std::cout << std::hex << std::setfill('0');
    while (dump.get(current)) {
        std::cout << std::setw(2) << +(unsigned char)current << ' ';
        line += current;
        if (++pos >= 16) {
            std::cout << "| " << std::dec << line << '\n' << std::hex;
            line.clear();
            pos = 0;
        }
    }
    // print remaining
    auto print_string = [&](){
        std::cout << std::string((16-pos)*3,' ') << "| " << std::dec << line << '\n';
    };
    auto print_setw = [&](){
        std::cout << std::setfill(' ') << std::setw((16-pos)*3) << ' ' << "| " << std::dec << line << '\n';
    };
    std::cout << measure(print_string) << '\n';
    std::cout << measure(print_setw) << '\n';
    return 0;
}
