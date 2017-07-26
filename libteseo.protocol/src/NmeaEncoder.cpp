#include <teseo/protocol/NmeaEncoder.h>

#include <utility>
#include <teseo/vendor/frozen/unordered_map.h>

using namespace stm::model;

/*
namespace frozen {
template <>
struct elsa<stm::model::Message> {

  constexpr std::size_t operator()(stm::model::Message const &value) const { return value; }

  constexpr std::size_t operator()(stm::model::Message const &value, std::size_t seed) const {
    std::size_t key = seed ^ value;
    key = (~key) + (key << 21); // key = (key << 21) - key - 1;
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8); // key * 265
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4); // key * 21
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key;
  }
};
}
//*/

namespace stm {
namespace protocol {

namespace messages {
constexpr const auto get_versions = make_message<TalkerId::PSTM>(BA("VER"), BA("255"));
} // namespace messages

/*
constexpr frozen::unordered_map<model::MessageId, std::pair<uint8_t *, std::size_t>, 1> pairs_map =
{
	{model::MessageId::GetVersions, messages::get_versions.getPair()}
};
//*/

template<std::size_t N>
ByteVector ba2bv(const ByteArray<N> & ba)
{
	return ByteVector(ba.begin(), ba.end());
}

void NmeaEncoder::encode(MessageId id)
{
	switch(id)
	{
		case MessageId::GetVersions:
			{
				auto p = messages::get_versions.getByteArray();
				encodedBytes(p.data(), p.size());
			}
			break;
	}
}

NmeaEncoder::NmeaEncoder() :
	IEncoder()
{ }

NmeaEncoder::~NmeaEncoder()
{ }

} // namespace protocol
} // namespace stm