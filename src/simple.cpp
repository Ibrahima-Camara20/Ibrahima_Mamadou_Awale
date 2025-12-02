#include <iostream>
#include <limits>
int main() {
    int max = std::numeric_limits<int>::max();
    int min = std::numeric_limits<int>::min();
    std::cout << "Max: " << max << std::endl;
    std::cout << "Min: " << min << std::endl;   
    return 0;
}
