#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

using namespace std;

template <typename T>
using Matrix = vector<vector<T>>;

using Weights = unordered_map</*Alpha char*/ char, /*signature*/ Matrix<int>>;

class Neuron
{
    inline static int s_default_threshold{35};

public:
    Neuron(int N, int M)
        : matrix_(N, vector(M, 0))
        , threshold_{s_default_threshold}
    {
    }

    auto accept(const filesystem::path& path) -> int
    {
        matrix_ = read_file(path);
        auto [max_weights, weights_table] = weights();

        vector<int> sums{};
        for (const auto& [sym, coeffs] : weights_table)
        {
            sums.push_back(calculate_sum(coeffs));
        }
        auto max_threshold = *max_element(sums.begin(), sums.end());

        return max_threshold > threshold_;
    }

private:
    static auto read_file(const filesystem::path& path) -> Matrix<int>
    {
        ifstream file{path};
        if (!file.is_open())
        {
            return {};
        }
        Matrix<int> matrix{};
        string line;
        while (getline(file, line))
        {
            Matrix<int>::value_type row{};
            transform(
                line.begin(), line.end(), back_inserter(row), [](const char c) { return c - '0'; });
            matrix.push_back(move(row));
        }
        return matrix;
    }

    [[nodiscard]] auto calculate_sum(const Matrix<int>& weights) const -> int
    {
        int sum{0};
        for (size_t i = 0; i < matrix_.size(); ++i)
        {
            for (size_t j = 0; j < matrix_[i].size(); ++j)
            {
                sum += (matrix_[i][j] * weights[i][j]);
            }
        }
        return sum;
    }

    static auto weights() -> pair<unordered_map</*Sym*/ char, /*max weight*/ int>, Weights>
    {
        int n{-1000};
        Weights weights{{'A',
                         {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1}}},
                        {'B',
                         {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}},
                        {'C',
                         {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}},
                        {'D',
                         {{1, 1, 1, 1, 1, 1, 1, 1, n, n},
                          {1, n, n, n, n, n, n, n, 1, n},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, n, 1},
                          {1, n, n, n, n, n, n, n, 1, n},
                          {1, 1, 1, 1, 1, 1, 1, 1, n, n}}},
                        {'E',
                         {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, n, n, n, n, n, n, n, n, n},
                          {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}};

        unordered_map</*Sym*/ char, /*max weight*/ int> max_weights{};
        for (const auto& [sym, weights] : weights)
        {
            int sum{0};
            for (const auto& row : weights)
            {
                sum += accumulate(
                    row.begin(), row.end(), 0, [&](auto a, auto b) { return a + (b > 0 ? b : 0); });
            }
            max_weights[sym] = sum;
        }
        return {max_weights, weights};
    }

    Matrix<int> matrix_{};
    int threshold_{0};
};

int main()
{
    Neuron neuron(10, 10);
    int out = neuron.accept({"Neuro/samples/A_success.txt"});

    cout << "Maybe A? : " << out << endl;

    return 0;
}