#include "endianess.h"
#include <byteswap.h>

namespace endian
{

	bool
	is_big_endian()
	{
		union {
			uint32_t i;
			char c[4];
		} big_int = {0x01020304};

		return big_int.c[0] == 1; 
	}

	// ---------------------------------------

	template<>
	uint8_t
	native_to_big(
		uint8_t val)
	{
		return val;
	}

	template<>
	int8_t
	native_to_big(
		int8_t val)
	{
		return val;
	}

	template<>
	uint16_t
	native_to_big(
		uint16_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_16(val);
	}

	template<>
	int16_t
	native_to_big(
		int16_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_16(val);
	}

	template<>
	uint32_t
	native_to_big(
		uint32_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_32(val);
	}

	template<>
	int32_t
	native_to_big(
		int32_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_32(val);
	}

	template<>
	uint64_t
	native_to_big(
		uint64_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_64(val);
	}

	template<>
	int64_t
	native_to_big(
		int64_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_64(val);
	}

	// ---------------------------------------

	template<>
	uint8_t
	big_to_native(
		uint8_t val)
	{
		return val;
	}

	template<>
	int8_t
	big_to_native(
		int8_t val)
	{
		return val;
	}

	template<>
	uint16_t
	big_to_native(
		uint16_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_16(val);
	}

	template<>
	int16_t
	big_to_native(
		int16_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_16(val);
	}

	template<>
	uint32_t
	big_to_native(
		uint32_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_32(val);
	}

	template<>
	int32_t
	big_to_native(
		int32_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_32(val);
	}

	template<>
	uint64_t
	big_to_native(
		uint64_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_64(val);
	}

	template<>
	int64_t
	big_to_native(
		int64_t val)
	{
		if (is_big_endian())
		{
			return val;
		}
		return bswap_64(val);
	}

	// ---------------------------------------

	template<>
	uint8_t
	native_to_little(
		uint8_t val)
	{
		return val;
	}

	template<>
	int8_t
	native_to_little(
		int8_t val)
	{
		return val;
	}

	template<>
	uint16_t
	native_to_little(
		uint16_t val)
	{
		if (is_big_endian())
		{
			return bswap_16(val);
		}
		return val;
	}

	template<>
	int16_t
	native_to_little(
		int16_t val)
	{
		if (is_big_endian())
		{
			return bswap_16(val);
		}
		return val;
	}

	template<>
	uint32_t
	native_to_little(
		uint32_t val)
	{
		if (is_big_endian())
		{
			return bswap_32(val);
		}
		return val;
	}

	template<>
	int32_t
	native_to_little(
		int32_t val)
	{
		if (is_big_endian())
		{
			return bswap_32(val);
		}
		return val;
	}

	template<>
	uint64_t
	native_to_little(
		uint64_t val)
	{
		if (is_big_endian())
		{
			return bswap_64(val);
		}
		return val;
	}

	template<>
	int64_t
	native_to_little(
		int64_t val)
	{
		if (is_big_endian())
		{
			return bswap_64(val);
		}
		return val;
	}

	// ---------------------------------------

	template<>
	uint8_t
	little_to_native(
		uint8_t val)
	{
		return val;
	}

	template<>
	int8_t
	little_to_native(
		int8_t val)
	{
		return val;
	}

	template<>
	uint16_t
	little_to_native(
		uint16_t val)
	{
		if (is_big_endian())
		{
			return bswap_16(val);
		}
		return val;
	}

	template<>
	int16_t
	little_to_native(
		int16_t val)
	{
		if (is_big_endian())
		{
			return bswap_16(val);
		}
		return val;
	}

	template<>
	uint32_t
	little_to_native(
		uint32_t val)
	{
		if (is_big_endian())
		{
			return bswap_32(val);
		}
		return val;
	}

	template<>
	int32_t
	little_to_native(
		int32_t val)
	{
		if (is_big_endian())
		{
			return bswap_32(val);
		}
		return val;
	}

	template<>
	uint64_t
	little_to_native(
		uint64_t val)
	{
		if (is_big_endian())
		{
			return bswap_64(val);
		}
		return val;
	}

	template<>
	int64_t
	little_to_native(
		int64_t val)
	{
		if (is_big_endian())
		{
			return bswap_64(val);
		}
		return val;
	}

}