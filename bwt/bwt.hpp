#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace algorithm{

class BWT
{
    using BlockStr = std::vector<std::string>;
    size_t block_size;

  public:
    BWT(size_t b_size = 256)
	: block_size(b_size + 1)
    {
    }

    std::string transform(const std::string& str)
    {
	BlockStr bw_mat(block_size, std::string(block_size, ' '));
	std::string out;

	bw_mat[0][0] = '$';
	for (size_t i(0); i < str.size(); i++)
	    bw_mat[0][i + 1] = str[i];

	for (size_t i(0); i < block_size - 1; i++)
	    shift(bw_mat[i], bw_mat[i + 1]);
	
	std::sort(bw_mat.begin(), bw_mat.end());

	out.resize(block_size);
	for (size_t i(0); i < block_size; i++)
	    out[i] = bw_mat[i][block_size - 1];

	return out;
    }

    std::string invert(const std::string& str)
    {
	BlockStr bw_mat(block_size, std::string(block_size, ' '));

	for (size_t i(0); i < block_size - 1; i++)
	{
	    rotate(bw_mat, i);
	    for (size_t j(0); j < block_size; j++)
		bw_mat[j][0] = str[j];
	    std::sort(bw_mat.begin(), bw_mat.end());
	}

	bw_mat[0].back() = str[0];
	return {bw_mat[0].begin() + 1, bw_mat[0].end()};
    }

  private:
    //shift a raw to left one block
    inline void shift(const std::string& src, std::string& dest)
    {
	for (size_t i(0); i < block_size; i++)
	    dest[i] = src[(i + 1) % block_size];
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
