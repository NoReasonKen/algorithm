#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace algorithm::bwt{

class SimpleBWT
{
  protected:
    using BlockStr = std::vector<std::string>;
    size_t block_size_;

  public:
    SimpleBWT(size_t b_size = 256)
	: block_size_(b_size + 1)
    {
    }

    std::string transform(const std::string& str)
    {
	BlockStr bw_mat(block_size_, std::string(block_size_, ' '));
	std::string out;

	bw_mat[0][0] = '$';
	for (size_t i(0); i < str.size(); i++)
	    bw_mat[0][i + 1] = str[i];

	for (size_t i(0); i < block_size_ - 1; i++)
	    shift(bw_mat[i], bw_mat[i + 1]);
	
	std::sort(bw_mat.begin(), bw_mat.end());

	out.resize(block_size_);
	for (size_t i(0); i < block_size_; i++)
	    out[i] = bw_mat[i][block_size_ - 1];

	return out;
    }

    std::string invert(const std::string& str)
    {
	BlockStr bw_mat(block_size_, std::string(block_size_, ' '));

	for (size_t i(0); i < block_size_ - 1; i++)
	{
	    rotate(bw_mat, i);
	    for (size_t j(0); j < block_size_; j++)
		bw_mat[j][0] = str[j];
	    std::sort(bw_mat.begin(), bw_mat.end());
	}

	bw_mat[0].back() = str[0];
	return {bw_mat[0].begin() + 1, bw_mat[0].end()};
    }

  protected:
    //shift a raw to left one block
    inline void shift(const std::string& src, std::string& dest)
    {
	for (size_t i(0); i < block_size_; i++)
	    dest[i] = src[(i + 1) % block_size_];
    }

    //rotate columns whose indice less than col to right one block
    inline void rotate(BlockStr& m, unsigned col)
    {
	for (auto& i : m)
	    for (unsigned j(0); j < col + 1; j++)
		i[col + 1 - j] = std::move(i[col - j]);
    }

};


}
