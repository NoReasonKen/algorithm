#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace algorithm
{

using PairSize = std::pair<size_t, size_t>;
using Info = std::pair<size_t, std::pair<size_t, size_t>>;
using InfoMap = std::unordered_map<char, Info>;
using ValMap = std::unordered_map<char, uint8_t>;
    
std::vector<size_t> sais(const std::string&);
void init(std::string_view, std::vector<bool>&, ValMap&, 
	std::vector<char>&, std::vector<size_t>&, InfoMap&);
void init_type(std::string_view, std::vector<bool>&);
std::vector<char> init_val_map(std::string_view, ValMap&);
void init_bucket_info(std::string_view,	const std::vector<bool>&, 
	InfoMap&, const std::vector<char>&);
std::vector<PairSize> get_lms(const std::vector<bool>&);
void sort_lms(std::vector<PairSize>&, std::string_view, 
	const std::vector<bool>&, std::vector<size_t>&, InfoMap&, 
	const std::vector<char>&);
void put_bucket(const std::vector<PairSize>&, std::string_view, 
	std::vector<size_t>&, InfoMap&);
void induced_sort(std::string_view, const std::vector<bool>&, 
	std::vector<size_t>&, InfoMap&);
std::vector<PairSize> restruct_lms(const std::vector<PairSize>&, const std::vector<bool>&, 
	const std::vector<size_t>&);
std::vector<size_t> update_lms(const std::vector<PairSize>&, 
	const std::vector<size_t>&);
std::string make_new_str(const std::vector<PairSize>&, 
	std::string_view); 
bool is_lms_equal(const std::vector<PairSize>&); 

std::vector<size_t> sais(const std::string& str)
{
    clock_t c(clock());
    //A buf for add \'$\' and then pointed by sv
    std::string buf(str + '$');

    //A string_view to replace str in function for efficiency
    std::string_view sv(buf);
    
    //A string as long as str_, which determined each char is whether 
    //L-type(true) or S-type(false)
    std::vector<bool> type;

    //store every char in str, index means the rank of stored char
    std::vector<char> val_rank;

    //An unordered_map to map every unique char to there rank
    ValMap val_map;

    //A container for bucket sort
    std::vector<size_t> bucket;

    //A unordered_map which stored info during bucket sorting
    //These info are actually place assignment
    InfoMap bucket_info;

    std::cout << "construct: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    init(sv, type, val_map, val_rank, bucket, bucket_info);
    std::cout << "init: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    auto v_lms(get_lms(type));
    std::cout << "get_lms: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";
    //std::cout << "num of lms: " << v_lms.size() << "\n";

    c = clock();
    sort_lms(v_lms, sv, type, bucket, bucket_info, val_rank);
    std::cout << "sort_lms: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    put_bucket(v_lms, sv, bucket, bucket_info);
    std::cout << "put_bucket: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    init_bucket_info(sv, type, bucket_info, val_rank);
    std::cout << "init_bucket_info: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    induced_sort(sv, type, bucket, bucket_info);
    std::cout << "induced_sort: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    return bucket;
}

void init(std::string_view sv, std::vector<bool>& type, 
	ValMap& val_map, std::vector<char>& val_rank, 
	std::vector<size_t>& bucket, InfoMap& bucket_info)
{
    type.clear();
    val_map.clear();
    bucket.clear();
    bucket_info.clear();

    val_rank = init_val_map(sv, val_map);

    init_type(sv, type);
    bucket.resize(sv.size(), sv.size());
    init_bucket_info(sv, type, bucket_info, val_rank);
}

void init_type(std::string_view sv, std::vector<bool>& type)
{
    type.resize(sv.size(), false);

    for (size_t i(type.size() - 2); (int64_t)i >= 0; i--)
    {
	if (sv[i] > sv[i + 1])
	    type[i] = true;
	else if (sv[i] < sv[i + 1])
	    type[i] = false;
	else
	    type[i] = type[i + 1];
    }
}

std::vector<char> init_val_map(std::string_view sv, ValMap& val_map)
{
    std::vector<char> v{'$'};
    for (auto i : sv)
    {
	size_t j(0);
	for (; j < v.size(); j++)
	    if (i == v[j])
		break;

	if (j == v.size())
	    v.emplace_back(i);
    }
    std::sort(v.begin(), v.end());
    val_map.reserve(v.size());
    for (size_t i(0); i < v.size(); i++)
	val_map[v[i]] = i;

    return v;
}

void init_bucket_info(std::string_view sv, 
	const std::vector<bool>& type, InfoMap& bucket_info, 
	const std::vector<char>& val_rank)
{
    for (auto i(val_rank.begin()); i != val_rank.end(); i++)
    {
	if (*i == '$')
	{
	    bucket_info[*i] = {0, {0, 0}};
	}
	else
	{
	    uint64_t l_count(0), s_count(0);
	    for (size_t j(0); j < sv.size() - 1; j++)
	    {
		if (*i == sv[j])
		{
		    switch((uint8_t)type[j])
		    {
			case true:
			    l_count++;
			    break;
			case false:
			    s_count++;
			    break;
		    }
		}
	    }

	    bucket_info[*i] = {
		bucket_info[*(i - 1)].second.second + 1, 
		{
		    bucket_info[*(i - 1)].second.second + l_count + 1,
		    bucket_info[*(i - 1)].second.second + l_count + s_count
		}
	    };
	}
    }
}

std::vector<PairSize> get_lms(const std::vector<bool>& type)
{
    std::vector<PairSize> v_lms;
    size_t count(0), idx;
    bool is_counting(false);

    v_lms.reserve(type.size() / 3);

    for (size_t i(1); i < type.size(); i++)
    {
	if (!type[i] && type[i - 1])
	{
	    if (is_counting)
		v_lms.emplace_back(idx, count);
	    count = 1;
	    is_counting = true;
	    idx = i;
	}
	count++;
    }

    v_lms.emplace_back(type.size() - 1, 1);
    return v_lms;
}

void sort_lms(std::vector<PairSize>& v_lms, std::string_view sv, 
	const std::vector<bool>& type, std::vector<size_t>& bucket, 
	InfoMap& bucket_info, const std::vector<char>& val_rank)
{
    clock_t c(clock());
    put_bucket(v_lms, sv, bucket, bucket_info);
    std::cout << "sort_lms-put_bucket: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";
    init_bucket_info(sv, type, bucket_info, val_rank);
    c = clock();
    induced_sort(sv, type, bucket, bucket_info);
    std::cout << "sort_lms-induced_sort: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";
    c = clock();
    restruct_lms(v_lms, type, bucket);
    std::cout << "sort_lms-restruct_lms: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    /*
    std::string new_str(make_new_str(v_lms, sv));
    for (auto i(new_str.begin()); i != new_str.end(); i++)
	if (*i == *(i + 1))
	{
	    sais(new_str);
	    break;
	}
    */

    for (auto& i : bucket)
	i = sv.size() + 1;
    init_bucket_info(sv, type, bucket_info, val_rank);
}

void put_bucket(const std::vector<PairSize>& v_lms, 
	std::string_view sv, std::vector<size_t>& bucket, 
	InfoMap& bucket_info)
{
    for (const auto& lms : v_lms)
    {
	auto& info(bucket_info[sv[lms.first]]);
	bucket[info.second.first] = lms.first;
	info.second.first++;
    }
    /*
    for (const auto& lms : v_lms)
    {
	auto& info(bucket_info[sv[lms.first]]);
	if (is_lms)
	{
	    bucket[info.second] = lms.first;
	    if (info.first != info.second)
		info.second--;
	}
	else
	{
	    bucket[info.first] = lms.first;
	    if (info.first != info.second)
		info.first++;
	}
    }
    */
}

void induced_sort(std::string_view sv, const std::vector<bool>& type, 
	std::vector<size_t>& bucket, InfoMap& bucket_info)
{
    for (auto i(bucket.begin()); i != bucket.end(); i++)
    {
	if (*i < sv.size() && *i > 0 && type[*i - 1] == true)
	{
	    auto& info(bucket_info[sv[*i - 1]]);
	    bucket[info.first] = *i - 1;
	    if (info.first != info.second.first)
		info.first++;
	}
    }

    for (auto i(bucket.rbegin()); i != bucket.rend(); i++)
    {
	if (*i < sv.size() && *i > 0 && type[*i - 1] == false)
	{
	    auto& info(bucket_info[sv[*i - 1]]);
	    bucket[info.second.second] = *i - 1;
	    if (info.second.first != info.second.second)
		info.second.second--;
	}
    }
}

std::vector<PairSize> restruct_lms(const std::vector<PairSize>& v_lms, 
	const std::vector<bool>& type, 
	const std::vector<size_t>& bucket)
{
    std::vector<PairSize> order{std::make_pair(0, 1)};

    order.reserve(v_lms.size());

    for (size_t i(1); i < bucket.size(); i++)
    {
	if (bucket[i] > 0 && !type[bucket[i]] && type[bucket[i] - 1])
	{
	    for (size_t j(bucket[i] + 1); j < bucket.size(); j++)
		if (!type[j] && type[j - 1])
		{
		    order.emplace_back(bucket[i], j - bucket[i] + 1);
		    break;
		}
	}
    }

    return order;
    /*
    for (size_t i(0); i < bucket.size(); i++)
	if (bucket[i] > 0 && !type[bucket[i]] && type[bucket[i] - 1])
	    for (size_t j(idx); j < v_lms.size(); j++)
		if (v_lms[j].first == bucket[i])
		{
		    auto tmp(std::move(v_lms[idx]));
		    v_lms[idx] = std::move(v_lms[j]);
		    v_lms[j] = tmp;
		    idx++;
		    if (idx == v_lms.size() - 1)
			return;
		}
    */
}

std::vector<size_t> update_lms(const std::vector<PairSize>& v_lms, 
	const std::vector<size_t>& bucket)
{
    std::vector<size_t> order;
    
    order.reserve(v_lms.size());
    for (const auto& i : bucket)
	for (const auto& j : v_lms)
	    if (i == j.first)
	    {
		order.emplace_back(i);
		
		if (order.size() == v_lms.size())
		    return order;
	    }

    return order;
}

std::string make_new_str(const std::vector<PairSize>& v_lms, 
	std::string_view sv) 
{
    std::string new_str("");
    char alphabet('A');

    new_str.reserve(v_lms.size());
    
    for (size_t i(0); i < v_lms.size(); i++)
    {
	new_str.push_back(alphabet);

	auto& lms1(v_lms[v_lms[i].first]);
	auto& lms2(v_lms[v_lms[i + 1].first]);

	if (std::string(sv.begin() + lms1.first, lms1.second) != 
	    std::string(sv.begin() + lms2.first, lms2.second))
	{
	    if (alphabet == 'Z')
		alphabet = 'a';
	    else
		alphabet++;
	}
    }

    return new_str;
}

bool is_lms_equal(const std::vector<PairSize>& v_lms)
{
    for (size_t i(0); i < v_lms.size() - 1; i++)
	if (v_lms[i] == v_lms[i + 1])
	    return true;

    return false;
}

}
