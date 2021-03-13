#ifndef ENDIANESS_H_
#define ENDIANESS_H_

#include <stdint.h>

namespace endian
{
	bool
	is_big_endian();
	
	template<typename T>
	T
	native_to_big(
		T val);
	
	template<typename T>
	T
	big_to_native(
		T val);

	template<typename T>
	T
	native_to_little(
		T val);

	template<typename T>
	T
	little_to_native(
		T val);
}

#endif