#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace algorithm
{

class SAIS
{
    using Info = std::pair<size_t, size_t>;
    using InfoMap = std::unordered_map<char, std::pair<size_t, size_t>>;
    using ValMap = std::unordered_map<char, uint8_t>;
    
    std::string str_;
    std::vector<char> type_;
    ValMap val_map_;
    std::vector<size_t> bucket_;
    InfoMap bucket_info_;

  public:
    std::vector<size_t> suffix_sort(const std::string& str)
    {
	init(str);
    }

  private:
    void init(const std::string& str)
    {
	str_ = str + "$";
	type_.clear();
	val_map_.clear();
	bucket_.clear();
	bucket_info_.clear();

	init_type();
	init_bucket_info(init_val_map());
    }

    void init_type()
    {
	for (auto i(str_.begin()); i != str_.end(); i++)
	{
	    if (*i == '$')
		type_.emplace_back('T');
	    else
	    {
		if (*i > *(i + 1))
		    type_.emplace_back('L');
		else if (*i < *(i + 1))
		    type_.emplace_back('S');
		else
		    type_.emplace_back(type_.back());
	    }
	}

	std::reverse(type_.begin(), type_.end());
    }

    std::vector<char> init_val_map()
    {
	std::vector<char> v;
	for (auto i : str_)
	{
	    size_t j(0);
	    for (; j < v.size(); j++)
		if (i == v[j])
		    break;

	    if (j == v.size())
		v.push_back(i);
	}
	std::sort(v.begin(), v.end());
	for (size_t i(0); i < v.size(); i++)
	    val_map_[v[i]] = i;

	return v;
    }
    
    void init_bucket_info(std::vector<char>&& v)
    {
	for (auto i(v.begin()); i != v.end(); i++)
	{
	    if (*i == '$')
		bucket_info_[*i] = {0, 0};
	    else
	    {
		uint64_t l_count(0), s_count(0);
		for (size_t j(0); j < str_.size(); j++)
		{
		    if (*i == str_[j])
		    {
			switch(type_[j])
			{
			    case 'L':
				l_count++;
			    case 'S':
			    case 'T':
				s_count++;
			}
		    }
		}
		
		bucket_info_[*i] = {
		    bucket_info_[*(i - 1)].second + 1, 
		    bucket_info_[*(i - 1)].second + l_count + s_count
		};
	    }
	}
    }
};

}
