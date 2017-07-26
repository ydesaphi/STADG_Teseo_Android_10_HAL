#ifndef TESEO_HAL_MODEL_MESSAGE
#define TESEO_HAL_MODEL_MESSAGE

#include <tuple>
#include <utility>
#include <type_traits>

#include <teseo/utils/ByteVector.h>
#include <teseo/model/TalkerId.h>

namespace stm {
namespace model {

namespace message_utils {

template<typename T>
constexpr std::size_t byte_count()
{
	return std::tuple_size<T>::value;
}

template<typename T, typename... TRest>
constexpr typename std::enable_if<
	(sizeof...(TRest) > std::integral_constant<std::size_t, 0>::value),
	std::size_t>::type
		byte_count()
{
	return std::tuple_size<T>::value + byte_count<TRest...>();
}

template<std::size_t SentenceIdSize, model::TalkerId talkerId, typename... TParams>
constexpr std::size_t message_byte_count()
{
	return model::TalkerIdByteCount(talkerId) +
	       SentenceIdSize              +
	       sizeof...(TParams)          +
	       byte_count<TParams...>()    +
	       4;
}

template<std::size_t N, std::size_t... I>
constexpr ByteArray<N> bytes_to_array(uint8_t (&a)[N], std::index_sequence<I...>)
{
	return { {a[I]...} };
}

} // namespace message_utils

template<
	TalkerId talkerId,
	std::size_t TalkerIdSize,
	std::size_t SentenceIdSize,
	std::size_t ParameterCount,
	std::size_t TotalByteCount,
	typename... TParams>
class Message {
public:
	using ParameterTuple = std::tuple<TParams...>;

	static constexpr std::size_t ParameterTupleSize = std::tuple_size<ParameterTuple>::value;

private:
	const ByteArray<SentenceIdSize> sentenceId;

	const ParameterTuple parameters;

	template<std::size_t I = 0>
	constexpr typename std::enable_if<I == ParameterTupleSize, std::size_t>::type
	copy_parameters_ba(uint8_t (&) [TotalByteCount], std::size_t offset, std::size_t) const
	{
		return offset;
	}

	template<std::size_t I = 0>
	constexpr typename std::enable_if<I < ParameterTupleSize, std::size_t>::type
	copy_parameters_ba(uint8_t (&ba) [TotalByteCount], std::size_t offset, std::size_t end) const
	{
		if(offset >= end)
			return end;

		auto const & pba = std::get<I>(parameters);
		
		ba[offset] = ',';
		offset++;

		for(std::size_t i = 0; i < pba.size() && offset != end; i++, offset++)
			ba[offset] = pba[i];

		return copy_parameters_ba<I + 1>(ba, offset, end);
	}

public:
	constexpr Message(
		const ByteArray<SentenceIdSize> & sentenceId,
		const ParameterTuple & params) :
			sentenceId(sentenceId),
			parameters(params)
	{ }

	constexpr const model::TalkerId getTalkerId() const
	{
		return talkerId;
	}

	constexpr const ByteArray<SentenceIdSize> & getSentenceId() const
	{
		return sentenceId;
	}

	constexpr std::size_t byteCount() const
	{
		return TotalByteCount;
	}

	template<std::size_t n>
	constexpr auto const & getParameter() const
	{
		return std::get<n>(parameters);
	}

	constexpr std::size_t parameterCount() const
	{
		return ParameterTupleSize;
	}

	constexpr ByteArray<TotalByteCount> getByteArray() const
	{
		uint8_t ba[TotalByteCount] = {0};
		std::size_t offset = 0;

		ba[offset] = '$';
		offset++;

		// Copy talker id
		if(talkerId == TalkerId::PSTM)
		{
			constexpr auto tidBa = ProprietaryTalkerIdToByteArray(talkerId);
			for(std::size_t i = 0; i < TalkerIdByteCount(talkerId); i++, offset++)
				ba[offset] = tidBa[i];
		}
		else
		{
			constexpr auto tidBa = StdTalkerIdToByteArray(talkerId);
			for(std::size_t i = 0; i < TalkerIdByteCount(talkerId); i++, offset++)
				ba[offset] = tidBa[i];
		}

		// Copy sentence id
		for(std::size_t i = 0; i < SentenceIdSize; i++, offset++)
			ba[offset] = sentenceId[i];

		offset = copy_parameters_ba(ba, offset, TotalByteCount);

		ba[offset] = '*';
		offset++;

		uint8_t crc = 0;
		for(std::size_t i = 1; ba[i] != '*'; i++)
			crc ^= ba[i];

		uint8_t crc1 = ((crc & static_cast<uint8_t>(0xF0)) >> 4);
		uint8_t crc2 = (crc & static_cast<uint8_t>(0x0F));

		ba[offset] = crc1 > 9 ? crc1 + 'A' : crc1 + '0'; offset++;
		ba[offset] = crc2 > 9 ? crc2 + 'A' : crc2 + '0';

		return message_utils::bytes_to_array(ba, std::make_index_sequence<TotalByteCount>{});
	}
};

template<
	model::TalkerId tid,
	std::size_t SentenceIdSize,
	typename... TParams>
constexpr auto make_message(const ByteArray<SentenceIdSize> & sid, TParams... params)
{
	return Message<
			tid,
			model::TalkerIdByteCount(tid),
			SentenceIdSize,
			sizeof...(TParams),
			message_utils::message_byte_count<SentenceIdSize, tid, TParams...>(),
			TParams...>(sid, std::make_tuple(params...));
}

enum class MessageId {
	GetVersions
};

} // namespace model
} // namespace stm

#endif // TESEO_HAL_MODEL_MESSAGE