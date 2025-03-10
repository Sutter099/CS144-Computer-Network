#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) {
    _buffer.resize(capacity, 0);
    _capacity = capacity;
}

size_t ByteStream::write(const string &data) {
    size_t remain_len = remaining_capacity();
    size_t data_len = data.size();

    // 1. check full
    if (!remain_len)
        return 0;
    if (_empty && data_len > 0)
        _empty = false;

    // 2. write available, update flag and info
    size_t written = std::min(remain_len, data_len);
    for (size_t i = 0; i < written; ++i) {
        _buffer[_w_offset % _capacity] = data[i];
        _w_offset++;
    }

    _w_offset %= _capacity;
    _w_count += written;

    if (written == remain_len)
        _full = true;

    return written;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t avail_len = buffer_size();
    size_t out_len = std::min(avail_len, len);
    size_t read_offset = _r_offset;
    std::string output;

    output.resize(out_len);

    // cannot fill with index? although reserved, it is not initialized, size == 0
    for (size_t i = 0; i < out_len; ++i) {
        output[i] = _buffer[read_offset % _capacity];
        // output.push_back(_buffer[read_offset % _capacity]);
        read_offset++;
    }

    return output;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t avail_len = buffer_size();
    size_t out_len = std::min(avail_len, len);

    if (len > 0 && _full)
        _full = false;
    if (out_len == avail_len)
        _empty = true;

    _r_offset += out_len;
    _r_offset %= _capacity;
    _r_count += out_len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std::string output = peek_output(len);

    pop_output(len);

    return output;
}

void ByteStream::end_input() {
    _ended = true;
}

bool ByteStream::input_ended() const {
    return _ended;
}

size_t ByteStream::buffer_size() const {
    if (_full)
        return _capacity;

    return (_capacity + _w_offset - _r_offset) % _capacity;
}

bool ByteStream::buffer_empty() const {
    return _empty;
}

bool ByteStream::eof() const {
    return input_ended() && (_r_offset == _w_offset);
}

size_t ByteStream::bytes_written() const {
    return _w_count;
}

size_t ByteStream::bytes_read() const {
    return _r_count;
}

size_t ByteStream::remaining_capacity() const {
    if (_empty)
        return _capacity;

    return (_capacity + _r_offset - _w_offset) % _capacity;
}
