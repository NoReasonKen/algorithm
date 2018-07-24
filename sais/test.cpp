#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "sais.hpp"

using algorithm::sais;

int main()
{
    std::string str;
    //str = "immissiissippi";
    //str = "aabaaaab";
    //str = "mmississiippii";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> uid1(0, 25);
    std::uniform_int_distribution<> uid2(0, 3);

    str.reserve(1000000000);
    /*
    for (size_t i(0); i < 10000000; i++)
	str.push_back(uid1(gen) + 'a');
    */
    std::unordered_map<int, char> m{{0, 'A'}, {1, 'C'}, {2, 'G'}, {3, 'T'}};
    for (size_t i(0); i < 1000000000; i++)
	str.push_back(m[uid2(gen)]);

    //std::cout << str << "\n";

    clock_t c(clock());
    auto v(sais(str));
    c = clock() - c;
    std::cout << "Total: " << (float)c / CLOCKS_PER_SEC << "sec\n";

    /*
    for (const auto& i : v)
	std::cout << i << " ";
    std::cout << "\n";
    
    str.push_back('$');
    for (const auto& i : v)
    {
	for (size_t j(i); j < str.size(); j++)
	    std::cout << str[j];
	std::cout << "\n";
    }
    */
}
