#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace algorithm
{

using PairSize = std::pair<size_t, size_t>;
using Info = std::pair<size_t, std::pair<size_t, size_t>>;
using InfoMap = std::unordered_map<uint64_t, Info>;
using ValMap = std::unordered_map<uint64_t, uint64_t>;
    
template <typename S>
std::vector<size_t> sais(const S&);
template <typename S>
void init(const S&, std::vector<bool>&, ValMap&, 
	std::vector<uint64_t>&, std::vector<size_t>&, InfoMap&);
template <typename S>
void init_type(const S&, std::vector<bool>&);
template <typename S>
std::vector<uint64_t> init_val_map(const S&, ValMap&);
template <typename S>
void init_bucket_info(const S&, const std::vector<bool>&, 
	InfoMap&, const std::vector<uint64_t>&);
std::vector<PairSize> get_lms(const std::vector<bool>&);
template <typename S>
void sort_lms(std::vector<PairSize>&, const S&, 
	const std::vector<bool>&, std::vector<size_t>&, InfoMap&, 
	const std::vector<uint64_t>&);
template <typename S>
void put_bucket(const std::vector<PairSize>&, const S&, 
	std::vector<size_t>&, InfoMap&);
template <typename S>
void induced_sort(const S&, const std::vector<bool>&, 
	std::vector<size_t>&, InfoMap&);
std::vector<PairSize> restruct_lms(const std::vector<PairSize>&, 
	const std::vector<bool>&, const std::vector<size_t>&);
template <typename S>
std::pair<bool, std::vector<uint64_t>> make_new(
	const std::vector<PairSize>&, const std::vector<PairSize>&, 
	const S&); 
template <typename S>
bool is_lms_equal(const PairSize&, const PairSize&, const S&); 

template <typename S>
std::vector<size_t> sais(const S& str)
{
    clock_t c(clock());
    //A buf for add \'!\' and then pointed by sv
    auto buf(str);
    buf.push_back('!');
    
    //A string as long as str_, which determined each char is whether 
    //L-type(true) or S-type(false)
    std::vector<bool> type;

    //store every char in str, index means the rank of stored char
    std::vector<uint64_t> val_rank;

    //An unordered_map to map every unique char to there rank
    ValMap val_map;

    //A container for bucket sort
    std::vector<size_t> bucket;

    //A unordered_map which stored info during bucket sorting
    //These info are actually place assignment
    InfoMap bucket_info;

    std::cout << "construct: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    init(buf, type, val_map, val_rank, bucket, bucket_info);
    std::cout << "init: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    auto v_lms(get_lms(type));
    std::cout << "get_lms: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";
    std::cout << "num of lms: " << v_lms.size() << "\n";

    c = clock();
    sort_lms(v_lms, buf, type, bucket, bucket_info, val_rank);
    std::cout << "sort_lms: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    put_bucket(v_lms, buf, bucket, bucket_info);
    std::cout << "put_bucket: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    init_bucket_info(buf, type, bucket_info, val_rank);
    std::cout << "init_bucket_info: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    c = clock();
    induced_sort(buf, type, bucket, bucket_info);
    std::cout << "induced_sort: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    return bucket;
}

template <typename S>
void init(const S& sv, std::vector<bool>& type, 
	ValMap& val_map, std::vector<uint64_t>& val_rank, 
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

template <typename S>
void init_type(const S& sv, std::vector<bool>& type)
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

template <typename S>
std::vector<uint64_t> init_val_map(const S& sv, ValMap& val_map)
{
    std::vector<uint64_t> v{'!'};
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

template <typename S>
void init_bucket_info(const S& sv, 
	const std::vector<bool>& type, InfoMap& bucket_info, 
	const std::vector<uint64_t>& val_rank)
{
    for (auto i(val_rank.begin()); i != val_rank.end(); i++)
    {
	if (*i == '!')
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

template <typename S>
void sort_lms(std::vector<PairSize>& v_lms, const S& sv, 
	const std::vector<bool>& type, std::vector<size_t>& bucket, 
	InfoMap& bucket_info, const std::vector<uint64_t>& val_rank)
{
    clock_t c(clock());
    put_bucket(v_lms, sv, bucket, bucket_info);
    std::cout << "sort_lms-put_bucket: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";
    init_bucket_info(sv, type, bucket_info, val_rank);
    c = clock();
    induced_sort(sv, type, bucket, bucket_info);
    std::cout << "sort_lms-induced_sort: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";
    c = clock();
    auto v_lms_ordered(restruct_lms(v_lms, type, bucket));
    std::cout << "sort_lms-restruct_lms: " << (float)(clock() - c) / CLOCKS_PER_SEC << "sec\n";

    auto new_one(make_new(v_lms, v_lms_ordered, sv));
    if (new_one.first)
    {
	bucket = sais(new_one.second);

	for (size_t i(0); i < v_lms_ordered.size(); i++)
	    v_lms_ordered[i] = v_lms[bucket[i + 1]];
    }

    v_lms = std::move(v_lms_ordered);

    bucket.resize(sv.size());
    for (auto& i : bucket)
	i = sv.size();
    init_bucket_info(sv, type, bucket_info, val_rank);
}

template <typename S>
void put_bucket(const std::vector<PairSize>& v_lms, 
	const S& sv, std::vector<size_t>& bucket, 
	InfoMap& bucket_info)
{
    for (const auto& lms : v_lms)
    {
	auto& info(bucket_info[sv[lms.first]]);
	bucket[info.second.first++] = lms.first;
    }
}

template <typename S>
void induced_sort(const S& sv, const std::vector<bool>& type, 
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
    std::vector<PairSize> order{std::make_pair(bucket.size() - 1, 1)};

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
}

template <typename S>
std::pair<bool, std::vector<uint64_t>> make_new(
	const std::vector<PairSize>& v_lms, 
	const std::vector<PairSize>& v_lms_ordered, 
	const S& sv) 
{
    std::vector<uint64_t> v;
    bool is_dup(false);

    v.resize(v_lms.size());

    for (size_t i(1), count(35); i < v_lms_ordered.size(); i++)
    {
	for (size_t j(0); j < v_lms.size(); j++)
	    if (v_lms[j].first == v_lms_ordered[i].first)
	    {
		if (S(sv.begin() + v_lms_ordered[i].first, 
		    sv.begin() + v_lms_ordered[i].first + v_lms_ordered[i].second) 
			== 
		    S(sv.begin() + v_lms_ordered[i - 1].first, 
		    sv.begin() + v_lms_ordered[i - 1].first + v_lms_ordered[i - 1].second
		    ))
		{
		    count--;
		    is_dup = true;

		}
		v[j] = count++;
		break;
	    }
    }
    v.back() = 34;

    return {is_dup, v};
}

template <typename S>
bool is_lms_equal(const PairSize& lms1, const PairSize& lms2, 
	const S& sv)
{
	if (lms1.second == lms2.second)
	    return false;

	std::string sv1(&sv[lms1.first], lms1.second), 
	    sv2(&sv[lms2.first], lms2.second);

	if (sv1 == sv2)
	    return true;
    return false;
}

}
