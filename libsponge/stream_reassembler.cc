#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

uint64_t StreamReassembler::ContinuousEnd() {
    uint64_t idx_stop = _idx_unread + _capacity;

    for (size_t i = 0; i < _capacity; ++i) {
        if (_bitmap[(i + _idx_unread) % _capacity] == false) {
            idx_stop = i + _idx_unread;
            break;
        }
    }

    return idx_stop;
}

size_t StreamReassembler::CountData() {
    size_t loaded = 0;

    for (size_t i = 0; i < _capacity; ++i)
        loaded += _bitmap[i];

    return loaded;
}

StreamReassembler::StreamReassembler(const size_t capacity)
    : _bitmap(capacity),
      _buffer(capacity),
      _output(capacity),
      _idx_unaccept(capacity),
      _idx_unread(0),
      _idx_unasm(0),
      _num_unasm(0),
      _capacity(capacity),
      _eof(false) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    size_t total, avail, loop_len0, loop_len1, start, end, written;
    std::string output, output1, output2;
    uint64_t idx_stop;

    if (eof)
        _eof = true;

    // 2. fill buffer and bitmap
    start   = std::max(_idx_unread, index);
    end     = std::min(_idx_unaccept, index + data.size());
    for (size_t i = start; i < end; ++i) {
        _buffer[i % _capacity] = data[i - index];
        _bitmap[i % _capacity] = true;
    }

    // 3. check if we can merge and send to _output
    idx_stop    = ContinuousEnd();
    total       = CountData();
    avail       = idx_stop - _idx_unread;
    loop_len0   = std::min(avail, _capacity - _idx_unread % _capacity);
    loop_len1   = avail - loop_len0;

    output1 = std::string{&_buffer[_idx_unread % _capacity], loop_len0};
    output2 = std::string{&_buffer[0], loop_len1};
    output = output1 + output2;

    for (size_t i = _idx_unread; i < idx_stop; ++i)
        _bitmap[i % _capacity] = false;

    written = _output.write(output);
    _num_unasm = total - written;
    _idx_unaccept += written;
    _idx_unread += written;

    if (_num_unasm == 0 && _eof)
        _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const {
    return _num_unasm;
}

bool StreamReassembler::empty() const {
    return _num_unasm == 0;
}
