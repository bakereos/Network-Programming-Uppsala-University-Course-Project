// byte_stream.hpp

#include <cassert>
#include <cstring>      // std::memcpy
#include <type_traits>  // std::is_fundamental

#include "Shared.h"

struct byte_stream {
  byte_stream() = default;

  int m_size{0};
  char m_buffer[1024] = {};
};

struct byte_stream_writer {
  byte_stream_writer(byte_stream& stream)
      : m_stream(stream),
        m_cursor(stream.m_buffer + stream.m_size),
        m_end(stream.m_buffer + sizeof(stream.m_buffer)) {}

  template <typename T>
  bool serialize(T value) {
    static_assert(std::is_fundamental_v<T>,
                  "T needs to be a fundamental datatype!");
    if ((m_cursor + sizeof(T)) >= m_end) {
      return false;
    }

    std::memcpy(m_cursor, &value, sizeof(T));
    m_cursor += sizeof(T);
    m_stream.m_size = int32_t(m_cursor - m_stream.m_buffer);
    return true;
  }

  bool serialize_vector_2(raylibOverlay::Vector2 vector2) {
    if (!serialize(vector2.x)) return false;
    if (!serialize(vector2.y)) return false;

    return true;
  }

  bool serialize_color(raylibOverlay::Color color) {
    if (!serialize(color.r)) return false;
    if (!serialize(color.g)) return false;
    if (!serialize(color.b)) return false;
    if (!serialize(color.a)) return false;

    return true;
  }

  template <typename T>
  bool serialize_enum(T value) {
    static_assert(std::is_enum_v<T>, "T is not a enum");

    int convertedValue = (int)value;

    if (!serialize(convertedValue)) return false;

    return true;
  }

  byte_stream& m_stream;
  char* m_cursor{nullptr};
  char* m_end{nullptr};
};

struct byte_stream_reader {
  byte_stream_reader(byte_stream& stream)
      : m_stream(stream),
        m_cursor(stream.m_buffer),
        m_end(stream.m_buffer + stream.m_size) {}

  template <typename T>
  bool serialize(T& value) {
    static_assert(std::is_fundamental_v<T>,
                  "T needs to be a fundamental datatype!");
    if ((m_cursor + sizeof(T)) > m_end) {
      return false;
    }

    std::memcpy(&value, m_cursor, sizeof(T));
    m_cursor += sizeof(T);
    return true;
  }

  bool serialize_vector_2(raylibOverlay::Vector2& vector2) {
    if (!serialize(vector2.x)) return false;
    if (!serialize(vector2.y)) return false;

    return true;
  }

  bool serialize_color(raylibOverlay::Color& color) {
    if (!serialize(color.r)) return false;
    if (!serialize(color.g)) return false;
    if (!serialize(color.b)) return false;
    if (!serialize(color.a)) return false;

    return true;
  }

  template <typename T>
  bool serialize_enum(T& value) {
    static_assert(std::is_enum_v<T>, "T is not a enum");

    int recvValue;

    if (!serialize(recvValue)) return false;

    value = (T)recvValue;

    return true;
  }

  byte_stream& m_stream;
  char* m_cursor{nullptr};
  char* m_end{nullptr};
};