#pragma once

#include <algorithm>
#include <unordered_map>
#include <string>
#include "SimpleBWT.hpp"

namespace algorithm::bwt{

class BRankingBWT : SimpleBWT
{
    using Order = std::unordered_map<char, std::vector<size_t>>;

  public:
    BRankingBWT(size_t b_size)
	: SimpleBWT(b_size)
    {
    }

    std::pair<std::string, Order> transform(const std::string& str)
    {
	BlockStr bw_mat(block_size_, std::string(block_size_, ' '));
	std::pair<std::string, Order> out;

	init_order(out);

	bw_mat[0][0] = '$';
	for (size_t i(0); i < str.size(); i++)
	    bw_mat[0][i + 1] = str[i];

	for (size_t i(0); i < block_size_ - 1; i++)
	    shift(bw_mat[i], bw_mat[i + 1]);
	
	std::sort(bw_mat.begin(), bw_mat.end());

	out.first.resize(block_size_);
	for (size_t i(0); i < block_size_; i++)
	    out.first[i] = bw_mat[i][block_size_ - 1];

	return out;
    }

    std::string invert(const std::string& str)
    {
	
    }

  protected:
    void init_order(std::pair<std::string, Order>& out)
    {
	auto& str(out.first);
	auto& order(out.second);
	for (size_t i(0); i < str.size(); i++)
	{
	    if (str[i] != '$' && order.find(str[i]) == order.end())
		order[str[i]] = {};
	    order[str[i]].emplace_back((i + 1) % str.size());
	}
    }

    void sort(BlockStr& bw_mat, Order& order)
    {

    }
};

}
