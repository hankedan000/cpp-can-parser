#include "cpp-can-parser/CANDatabase.h"
#include "endianess.h"

using namespace CppCAN;

enum class Alignment
{
    size_inbetween_first_64_bit,
    signal_exceeds_64_bit_size_but_signal_fits_into_64_bit,
    signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit
};

CANSignal::Range CANSignal::Range::fromString(const std::string & minstr, const std::string & maxstr) {
  long min = std::stol(minstr);
  long max = std::stol(maxstr);

  return Range(min, max);
}

CANSignal::Range::Range(long m, long mm)
  : defined(true), min(m), max(mm) {

}

template <Alignment aAlignment, CANSignal::Endianness aByteOrder, CANSignal::Signedness aValueType, CANSignal::ExtendedValueType aExtendedValueType>
CANSignal::raw_t template_decode(const CANSignal* sig, const void* nbytes) noexcept
{
  uint64_t data;
  if constexpr (aAlignment == Alignment::signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit)
  {
    data = *reinterpret_cast<const uint64_t*>(&reinterpret_cast<const uint8_t*>(nbytes)[sig->_byte_pos]);
    uint64_t data1 = reinterpret_cast<const uint8_t*>(nbytes)[sig->_byte_pos + 8];
    if constexpr (aByteOrder == CANSignal::Endianness::BigEndian)
    {
      data = endian::native_to_big(data);
      data &= sig->_mask;
      data <<= sig->_fixed_start_bit_0;
      data1 >>= sig->_fixed_start_bit_1;
      data |= data1;
    }
    else
    {
      data = endian::native_to_little(data);
      data >>= sig->_fixed_start_bit_0;
      data1 &= sig->_mask;
      data1 <<= sig->_fixed_start_bit_1;
      data |= data1;
    }
    if constexpr (aExtendedValueType == CANSignal::ExtendedValueType::Float ||
        aExtendedValueType == CANSignal::ExtendedValueType::Double)
    {
      return data;
    }
    if constexpr (aValueType == CANSignal::Signedness::Signed)
    {
      if (data & sig->_mask_signed)
      {
        data |= sig->_mask_signed;
      }
    }
    return data;
  }
  else
  {
    if constexpr (aAlignment == Alignment::size_inbetween_first_64_bit)
    {
      data = *reinterpret_cast<const uint64_t*>(nbytes);
    }
    else
    {
      data = *reinterpret_cast<const uint64_t*>(&reinterpret_cast<const uint8_t*>(nbytes)[sig->_byte_pos]);
    }
    if constexpr (aByteOrder == CANSignal::Endianness::BigEndian)
    {
      data = endian::native_to_big(data);
    }
    else
    {
      data = endian::native_to_little(data);
    }
    if constexpr (aExtendedValueType == CANSignal::ExtendedValueType::Double)
    {
      return data;
    }
    data >>= sig->_fixed_start_bit_0;
  }
  data &= sig->_mask;
  if constexpr (aExtendedValueType == CANSignal::ExtendedValueType::Float)
  {
    return data;
  }
  if constexpr (aValueType == CANSignal::Signedness::Signed)
  {
    // bit extending
    // trust the compiler to optimize this
    if (data & sig->_mask_signed)
    {
      data |= sig->_mask_signed;
    }
  }
  return data;
}

constexpr uint64_t enum_mask(Alignment a, CANSignal::Endianness bo, CANSignal::Signedness vt, CANSignal::ExtendedValueType evt)
{
  uint64_t result = 0;
  switch (a)
  {
  case Alignment::size_inbetween_first_64_bit:                                    result |= 0b1; break;
  case Alignment::signal_exceeds_64_bit_size_but_signal_fits_into_64_bit:         result |= 0b10; break;
  case Alignment::signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit: result |= 0b100; break;
  }
  switch (bo)
  {
  case CANSignal::Endianness::LittleEndian:                                       result |= 0b1000; break;
  case CANSignal::Endianness::BigEndian:                                          result |= 0b10000; break;
  }
  switch (vt)
  {
  case CANSignal::Signedness::Signed:                                              result |= 0b100000; break;
  case CANSignal::Signedness::Unsigned:                                            result |= 0b1000000; break;
  }
  switch (evt)
  {
  case CANSignal::ExtendedValueType::Integer:                                     result |= 0b10000000; break;
  case CANSignal::ExtendedValueType::Float:                                       result |= 0b100000000; break;
  case CANSignal::ExtendedValueType::Double:                                      result |= 0b1000000000; break;
  }
  return result;
}

using decode_func_t = CANSignal::raw_t (*)(const CANSignal*, const void*) noexcept;
decode_func_t make_decode(Alignment a, CANSignal::Endianness bo, CANSignal::Signedness vt, CANSignal::ExtendedValueType evt)
{
  constexpr auto si64b            = Alignment::size_inbetween_first_64_bit;
  constexpr auto se64bsbsfi64b    = Alignment::signal_exceeds_64_bit_size_but_signal_fits_into_64_bit;
  constexpr auto se64bsasdnfi64b  = Alignment::signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit;
  constexpr auto le               = CANSignal::Endianness::LittleEndian;
  constexpr auto be               = CANSignal::Endianness::BigEndian;
  constexpr auto sig              = CANSignal::Signedness::Signed;
  constexpr auto usig             = CANSignal::Signedness::Unsigned;
  constexpr auto i                = CANSignal::ExtendedValueType::Integer;
  constexpr auto f                = CANSignal::ExtendedValueType::Float;
  constexpr auto d                = CANSignal::ExtendedValueType::Double;
  switch (enum_mask(a, bo, vt, evt))
  {
  case enum_mask(si64b, le, sig, i):            return template_decode<si64b, le, sig, i>;
  case enum_mask(si64b, le, sig, f):            return template_decode<si64b, le, sig, f>;
  case enum_mask(si64b, le, sig, d):            return template_decode<si64b, le, sig, d>;
  case enum_mask(si64b, le, usig, i):           return template_decode<si64b, le, usig, i>;
  case enum_mask(si64b, le, usig, f):           return template_decode<si64b, le, usig, f>;
  case enum_mask(si64b, le, usig, d):           return template_decode<si64b, le, usig, d>;
  case enum_mask(si64b, be, sig, i):            return template_decode<si64b, be, sig, i>;
  case enum_mask(si64b, be, sig, f):            return template_decode<si64b, be, sig, f>;
  case enum_mask(si64b, be, sig, d):            return template_decode<si64b, be, sig, d>;
  case enum_mask(si64b, be, usig, i):           return template_decode<si64b, be, usig, i>;
  case enum_mask(si64b, be, usig, f):           return template_decode<si64b, be, usig, f>;
  case enum_mask(si64b, be, usig, d):           return template_decode<si64b, be, usig, d>;
  case enum_mask(se64bsbsfi64b, le, sig, i):    return template_decode<se64bsbsfi64b, le, sig, i>;
  case enum_mask(se64bsbsfi64b, le, sig, f):    return template_decode<se64bsbsfi64b, le, sig, f>;
  case enum_mask(se64bsbsfi64b, le, sig, d):    return template_decode<se64bsbsfi64b, le, sig, d>;
  case enum_mask(se64bsbsfi64b, le, usig, i):   return template_decode<se64bsbsfi64b, le, usig, i>;
  case enum_mask(se64bsbsfi64b, le, usig, f):   return template_decode<se64bsbsfi64b, le, usig, f>;
  case enum_mask(se64bsbsfi64b, le, usig, d):   return template_decode<se64bsbsfi64b, le, usig, d>;
  case enum_mask(se64bsbsfi64b, be, sig, i):    return template_decode<se64bsbsfi64b, be, sig, i>;
  case enum_mask(se64bsbsfi64b, be, sig, f):    return template_decode<se64bsbsfi64b, be, sig, f>;
  case enum_mask(se64bsbsfi64b, be, sig, d):    return template_decode<se64bsbsfi64b, be, sig, d>;
  case enum_mask(se64bsbsfi64b, be, usig, i):   return template_decode<se64bsbsfi64b, be, usig, i>;
  case enum_mask(se64bsbsfi64b, be, usig, f):   return template_decode<se64bsbsfi64b, be, usig, f>;
  case enum_mask(se64bsbsfi64b, be, usig, d):   return template_decode<se64bsbsfi64b, be, usig, d>;
  case enum_mask(se64bsasdnfi64b, le, sig, i):  return template_decode<se64bsasdnfi64b, le, sig, i>;
  case enum_mask(se64bsasdnfi64b, le, sig, f):  return template_decode<se64bsasdnfi64b, le, sig, f>;
  case enum_mask(se64bsasdnfi64b, le, sig, d):  return template_decode<se64bsasdnfi64b, le, sig, d>;
  case enum_mask(se64bsasdnfi64b, le, usig, i): return template_decode<se64bsasdnfi64b, le, usig, i>;
  case enum_mask(se64bsasdnfi64b, le, usig, f): return template_decode<se64bsasdnfi64b, le, usig, f>;
  case enum_mask(se64bsasdnfi64b, le, usig, d): return template_decode<se64bsasdnfi64b, le, usig, d>;
  case enum_mask(se64bsasdnfi64b, be, sig, i):  return template_decode<se64bsasdnfi64b, be, sig, i>;
  case enum_mask(se64bsasdnfi64b, be, sig, f):  return template_decode<se64bsasdnfi64b, be, sig, f>;
  case enum_mask(se64bsasdnfi64b, be, sig, d):  return template_decode<se64bsasdnfi64b, be, sig, d>;
  case enum_mask(se64bsasdnfi64b, be, usig, i): return template_decode<se64bsasdnfi64b, be, usig, i>;
  case enum_mask(se64bsasdnfi64b, be, usig, f): return template_decode<se64bsasdnfi64b, be, usig, f>;
  case enum_mask(se64bsasdnfi64b, be, usig, d): return template_decode<se64bsasdnfi64b, be, usig, d>;
  }
  return nullptr;
}

template <class T>
double raw_to_phys(const CANSignal* sig, CANSignal::raw_t raw) noexcept
{
  double draw = double(*reinterpret_cast<T*>(&raw));
  return draw * sig->scale() + sig->offset();
}
template <class T>
CANSignal::raw_t phys_to_raw(const CANSignal* sig, double phys) noexcept
{
  T result = T((phys - sig->offset()) / sig->scale());
  return *reinterpret_cast<CANSignal::raw_t*>(&result);
}

CANSignal::CANSignal(const std::string & name, unsigned int start_bit, unsigned int length, double scale, double offset, Signedness signedness, Endianness endianness, Range range) :
  name_(name), start_bit_(start_bit), length_(length),
  scale_(scale), offset_(offset), signedness_(signedness), endianness_(endianness),
  range_(range) {
  // save some additional values to speed up decoding
  _mask =  (1ull << (length_ - 1ull) << 1ull) - 1;
  _mask_signed = ~((1ull << (length_ - 1ull)) - 1);

  _byte_pos = start_bit_ / 8;

  uint64_t nbytes;
  if (endianness_ == Endianness::LittleEndian)
  {
    nbytes = (start_bit_ % 8 + length_ + 7) / 8;
  }
  else
  {
    nbytes = (length_ + (7 - start_bit_ % 8) + 7) / 8;
  }
  Alignment alignment = Alignment::size_inbetween_first_64_bit;
  // check whether the data is in the first 8 bytes
  // so we can optimize out one memory access
  if (_byte_pos + nbytes <= 8)
  {
    alignment = Alignment::size_inbetween_first_64_bit;
    if (endianness_ == Endianness::LittleEndian)
    {
      _fixed_start_bit_0 = start_bit_;
    }
    else
    {
      _fixed_start_bit_0 = (8 * (7 - (start_bit_ / 8))) + (start_bit_ % 8) - (length_ - 1);
    }
  }
  // check whether we can align the data on 64 bit
  else if (_byte_pos  % 8 + nbytes <= 8)
  {
    alignment = Alignment::signal_exceeds_64_bit_size_but_signal_fits_into_64_bit;
    // align the byte pos on 64 bit
    _byte_pos -= _byte_pos % 8;
    _fixed_start_bit_0 = start_bit_ - _byte_pos * 8;
    if (endianness_ == Endianness::BigEndian)
    {
      _fixed_start_bit_0 = (8 * (7 - (_fixed_start_bit_0 / 8))) + (_fixed_start_bit_0 % 8) - (length_ - 1);
    }
  }
  // we aren't able to align the data on 64 bit, so check whether the data fits into on uint64_t
  else if (nbytes <= 8)
  {
    alignment = Alignment::signal_exceeds_64_bit_size_but_signal_fits_into_64_bit;
    _fixed_start_bit_0 = start_bit_ - _byte_pos * 8;
    if (endianness_ == Endianness::BigEndian)
    {
      _fixed_start_bit_0 = (8 * (7 - (_fixed_start_bit_0 / 8))) + (_fixed_start_bit_0 % 8) - (length_ - 1);
    }
  }
  // we aren't able to align the data on 64 bit, and we aren't able to fit the data into one uint64_t
  // so we have to compose the resulting value
  else
  {
    alignment = Alignment::signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit;
    if (endianness_ == Endianness::BigEndian)
    {
      uint64_t nbits_last_byte = (7 - start_bit_ % 8) + length_ - 64;
      _fixed_start_bit_0 = nbits_last_byte;
      _fixed_start_bit_1 = 8 - nbits_last_byte;
      _mask = (1ull << (start_bit_ % 8 + 57)) - 1;
    }
    else
    {
      _fixed_start_bit_0 = start_bit_ - _byte_pos * 8;
      _fixed_start_bit_1 = 64 - start_bit_ % 8;
      uint64_t nbits_last_byte = length_ + start_bit_ % 8 - 64;
      _mask = (1ull << nbits_last_byte) - 1ull;
    }
  }

  // FIXME for now, everything is interpreted as an Integer
  ExtendedValueType _extended_value_type = ExtendedValueType::Integer;
  
  _decode = ::make_decode(alignment, endianness_, signedness_, _extended_value_type);
  // _encode = ::encode;
  switch (_extended_value_type)
  {
  case CANSignal::ExtendedValueType::Integer:
    switch (signedness_)
    {
    case CANSignal::Signedness::Signed:
      _raw_to_phys = ::raw_to_phys<int64_t>;
      _phys_to_raw = ::phys_to_raw<int64_t>;
      break;
    case CANSignal::Signedness::Unsigned:
      _raw_to_phys = ::raw_to_phys<uint64_t>;
      _phys_to_raw = ::phys_to_raw<uint64_t>;
      break;
    }
    break;
  case CANSignal::ExtendedValueType::Float:
    _raw_to_phys = ::raw_to_phys<float>;
    _phys_to_raw = ::phys_to_raw<float>;
    break;
  case CANSignal::ExtendedValueType::Double:
    _raw_to_phys = ::raw_to_phys<double>;
    _phys_to_raw = ::phys_to_raw<double>;
    break;
  }
}

const std::string & CANSignal::name() const {
  return name_;
}

unsigned int CANSignal::start_bit() const {
  return start_bit_;
}

unsigned int CANSignal::length() const {
  return length_;
}

const std::string & CANSignal::comment() const {
  return comment_;
}

double CANSignal::scale() const {
  return scale_;
}

double CANSignal::offset() const {
  return offset_;
}

const CANSignal::Range & CANSignal::range() const {
  return range_;
}

CANSignal::Signedness CANSignal::signedness() const {
  return signedness_;
}

CANSignal::Endianness CANSignal::endianness() const {
  return endianness_;
}

const std::map<unsigned int, std::string>& CANSignal::choices() const {
  return choices_;
}

void CANSignal::setComment(const std::string & comment) {
  comment_ = comment;
}

void CANSignal::setChoices(const std::map<unsigned int, std::string>& choices) {
  choices_ = choices;
}
