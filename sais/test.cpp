#include <gperftools/profiler.h>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "sais.hpp"

using algorithm::sais;

int main()
{
    ProfilerStart("sais");
    clock_t c(clock());
    std::string str;
    //str = "immissiissippi";
    //str = "aabaaaab";
    //str = "mmississiippii";
    ;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> uid1(0, 25);
    std::uniform_int_distribution<> uid2(0, 3);

    str.reserve(30000000);
    std::unordered_map<int, char> m{{0, 'A'}, {1, 'C'}, {2, 'G'}, {3, 'T'}};
    for (size_t i(0); i < 30000000; i++)
	str.push_back(m[uid2(gen)]);

    c = clock() - c;
    std::cout << "Generate str " << (float)c / CLOCKS_PER_SEC << "sec\n";
    //std::cout << str << "\n";
    std::cout << "SA-IS algorithm start:\n";

    c = clock();
    auto v(sais(str));
    c = clock() - c;
    std::cout << "Total: " << (float)c / CLOCKS_PER_SEC << "sec\n";

    ProfilerStop();
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
