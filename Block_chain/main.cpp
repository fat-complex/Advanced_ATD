#include <assert.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <optional>
#include <utility>
#include <vector>

#include <openssl/md5.h>

using namespace std;
using namespace std::chrono;

using TimePoint = time_point<high_resolution_clock>;
using Data = std::string;

struct Block
{
    std::size_t index{0};
    std::string previous_hash;
    std::string current_hash;
    std::size_t nonce;
    Data data;
    TimePoint time;
};

class BlockChain
{
public:
    explicit BlockChain(string expected_hash_tail_value = {})
        : expected_hash_tail_value_{std::move(expected_hash_tail_value)}
    {
        auto block = Block{0, "", "0000", 0, "start block"};
        blocks_.emplace_back(move(block));
        cout << "BlockChain created" << endl;
    }

    auto add_block(Data data) -> string
    {
        auto previous_block = *blocks_.back();
        auto block = Block{.index = previous_block.index + 1,
                           .previous_hash = previous_block.current_hash,
                           .current_hash = "none",
                           .nonce = 0,
                           .data = std::move(data),
                           .time = TimePoint::clock::now()};
        if (expected_hash_tail_value_.empty())
        {
            block.current_hash = find_hash_without_nonce_for(block);
        }
        else
        {
            auto [new_hash, nonce] = find_hash_with_nonce_for(block);
            block.current_hash = new_hash;
            block.nonce = nonce;
        }
        auto ret = block.current_hash;
        blocks_.emplace_back(move(block));
        return ret;
    }

    [[nodiscard]] std::optional<Block> find_block(string hash) const
    {
        auto foundIt = find_if(blocks_.cbegin(),
                               blocks_.cend(),
                               [&](const auto& block) { return block->current_hash == hash; });
        return foundIt != blocks_.end() ? *foundIt : std::nullopt;
    }

    bool check_chain()
    {
        if (expected_hash_tail_value_.empty())
        {
            return check_chain_without_nonce();
        }
        return check_chain_with_nonce();
    }

    private:
    [[nodiscard]] auto find_hash_with_nonce_for(const Block& block) const -> pair</*hash*/ string, /*nonce*/ size_t>
    {
        string head_hash = find_hash_without_nonce_for(block);

        unsigned char digest[MD5_DIGEST_LENGTH];
        MD5_CTX context;
        MD5_Init(&context);
        MD5_Update(&context, head_hash.data(), head_hash.size());

        string curr;

        int nonce = 0;
        while (!hasEnding(curr, expected_hash_tail_value_))
        {
            auto stringNonce = to_string(++nonce);
            MD5_Update(&context, stringNonce.data(), stringNonce.length());
            MD5_Final(digest, &context);
            curr = MD5_buffer_to_string(digest);
        }
        return {curr, nonce};
    }

    static auto find_hash_without_nonce_for(const Block& block) -> string
    {
        MD5_CTX context;
        unsigned char digest[MD5_DIGEST_LENGTH];
        MD5_Init(&context);

        auto reprStringIdx = to_string(block.index);
        MD5_Update(&context, reprStringIdx.data(), reprStringIdx.size());
        MD5_Update(&context, block.previous_hash.data(), block.previous_hash.size());

        auto reprStringTimePoint = to_string(block.time.time_since_epoch().count());
        MD5_Update(&context, reprStringTimePoint.data(), reprStringTimePoint.size());
        MD5_Update(&context, block.data.data(), block.data.size());
        MD5_Final(digest, &context);

        return MD5_buffer_to_string(digest);
    }

    static auto hasEnding(const string& source, const string& tail) -> bool
    {
        if (source.size() < tail.size())
        {
            return false;
        }
        return source.compare(source.size() - tail.size(), tail.size(), tail) == 0;
    }

    static auto MD5_buffer_to_string(unsigned char digest[MD5_DIGEST_LENGTH]) -> string
    {
        stringstream ss;
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << int(digest[i]);
        }
        return ss.str();
    }

    bool check_chain_with_nonce()
    {
        for (auto it = blocks_.cbegin() + 1; it != blocks_.cend(); ++it)
        {
            if (find_hash_with_nonce_for(*(*it)).first != (*it)->current_hash)
            {
                return false;
            }
        }
        return true;
    }

    bool check_chain_without_nonce()
    {
        for (auto it = blocks_.cbegin() + 1; it != blocks_.cend(); ++it)
        {
            if (find_hash_without_nonce_for(**it) != (*it)->current_hash)
            {
                return false;
            }
        }
        return true;
    }

    string expected_hash_tail_value_;
    vector<optional<Block>> blocks_;
};

int main()
{
    BlockChain chain{"00"};

    auto data = Data{"first block in world"};
    auto hash = chain.add_block(data);
    cout << "Hash: " << hash << endl;

    auto block = chain.find_block(hash);
    assert(block);
    cout << "Block nonce: " << block->nonce << endl;

    assert(chain.check_chain());
}