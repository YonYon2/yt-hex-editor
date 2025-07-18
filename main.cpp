#include <iostream>
#include <fstream>

/* procedure:
 - check if parameter is given
 - check filename exists
 - open file
 - view contents between a range
 - quit by pressing File > Quit
*/

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Nothin, pass filename as parameter!\n";
        return -1;
    }
    std::fstream dump(argv[1]);
    if (!dump) {
        std::cout << "File does not exist!\n";
        return -1;
    }
    char current;
    int pos = 0;
    while (dump.get(current)) {
        std::cout << +current << ' ';
        if (++pos >= 16){
            std::cout << '\n';
            pos = 0;
        }
    }
    return 0;
}
