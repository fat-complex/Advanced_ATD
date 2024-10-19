#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

using namespace std;
namespace fs = filesystem;

template <typename T>
using Matrix = vector<vector<T>>;

using Weights = pair</*Alpha char*/ char, /*signature*/ Matrix<int>>;

struct ExpectedResult
{
    char sym;
    bool value;
};

struct Answer
{
    char expected_sym;
    char actual_sym;
    bool matched_actual_sym;
    bool matched_expected_sym;
    Matrix<int> input;
};

class Neuron
{
public:
    virtual ~Neuron() = default;
    Neuron(size_t N, size_t M)
        : N_{N}
        , M_{M}
    {
    }

    [[nodiscard]] auto accept(const filesystem::path& sample) const -> Answer
    {
        auto [expected_input_result, input_matrix_] = read_file(sample);
        const auto expected_sym = weights_.first;
        const auto sum = calculate_sum(weights_.second, input_matrix_);

        return {expected_sym,
                expected_input_result.sym,
                expected_input_result.value,
                sum > threshold_,
                input_matrix_};
    }

    auto train(const filesystem::path& sample, int loop_limits = 10000) -> void
    {
        auto is_false_positive_matched = [](const auto& answer)
        { return answer.expected_sym != answer.actual_sym and answer.matched_expected_sym; };

        auto is_false_negative_matched = [](const auto& answer)
        {
            return answer.expected_sym == answer.actual_sym and
                   !(answer.matched_expected_sym == answer.matched_actual_sym);
        };
        auto is_wrong = [&](const auto& answer)
        { return is_false_positive_matched(answer) or is_false_negative_matched(answer); };

        auto res = accept(sample);
        while (is_wrong(res) and loop_limits--)
        {
            if (is_false_positive_matched(res))
            {
                decrease_weights(res.input);
            }
            if (is_false_negative_matched(res))
            {
                increase_weights(res.input);
            }
            res = accept(sample);
        }
    }

private:
    static auto read_file(const filesystem::path& path) -> pair<ExpectedResult, Matrix<int>>
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
        return {get_file_info(path), matrix};
    }

    static auto get_file_info(const filesystem::path& path) -> ExpectedResult
    {
        const auto filename_view = path.filename().string();
        const auto sym{filename_view[0]};
        const auto success{filename_view.find("success") != string_view::npos};

        return {sym, success};
    }

    [[nodiscard]] auto calculate_sum(const Matrix<int>& weights,
                                     const Matrix<int>& input_matrix) const -> int
    {
        int sum{0};
        for (size_t i = 0; i < N_; ++i)
        {
            for (size_t j = 0; j < M_; ++j)
            {
                sum += (weights[i][j] * input_matrix[i][j]);
            }
        }
        return sum;
    }

    auto increase_weights(const Matrix<int>& input_matrix) -> void
    {
        for (size_t i = 0; i < N_; ++i)
        {
            for (size_t j = 0; j < M_; ++j)
            {
                weights_.second[i][j] += input_matrix[i][j];
            }
        }
    }

    auto decrease_weights(const Matrix<int>& input_matrix) -> void
    {
        for (size_t i = 0; i < N_; ++i)
        {
            for (size_t j = 0; j < M_; ++j)
            {
                weights_.second[i][j] -= input_matrix[i][j];
            }
        }
    }

    size_t N_{};
    size_t M_{};

protected:
    int threshold_{};
    Weights weights_{};
};

class NeuronA : public Neuron
{
public:
    NeuronA(size_t N, size_t M)
        : Neuron(N, M)
    {
        threshold_ = 35;
        int p{0};
        int n{0};
        weights_ = {'A',
                    {{p, p, p, p, p, p, p, p, p, p},
                     {p, n, n, n, n, n, n, n, n, p},
                     {p, n, n, n, n, n, n, n, n, p},
                     {p, n, n, n, n, n, n, n, n, p},
                     {p, n, n, n, n, n, n, n, n, p},
                     {p, p, p, p, p, p, p, p, p, p},
                     {p, n, n, n, n, n, n, n, n, p},
                     {p, n, n, n, n, n, n, n, n, p},
                     {p, n, n, n, n, n, n, n, n, p},
                     {p, n, n, n, n, n, n, n, n, p}}};
    }
};

int main()
{
    const auto input_path_dir = fs::path{"samples"};

    NeuronA neuron_a(10, 10);
    for (const auto& file : filesystem::directory_iterator{input_path_dir})
    {
        neuron_a.train(file);
    }
    for (const auto& file : filesystem::directory_iterator{input_path_dir})
    {
        auto [expected_sym, actual_sym, matched_actual, matched_expected, _] =
            neuron_a.accept(file.path());

        cout << "sample: " << file.path().filename() << endl;
        cout << "The expected value of the symbol " << expected_sym << " for actual input: ["
             << actual_sym << ", " << matched_actual << "]"
             << " is " << matched_expected << endl
             << endl;
    }

    return 0;
}