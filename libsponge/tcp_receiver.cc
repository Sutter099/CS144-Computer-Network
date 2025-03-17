#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    TCPHeader header = seg.header();

    if (!_syn_received && !header.syn)
        return;

    if (header.syn) {
        _isn = header.seqno;
        _syn_received = true;
        _last_abs_seq = 0;
        // std::cerr << "[debug] SYN received\n";
    }

    uint64_t stream_index = unwrap(header.seqno, _isn, _last_abs_seq);
    if (!header.syn) {
        stream_index -= _syn_received;  // SYN 只占 1 字节
    }

    if (header.fin)
        _fin_received = true;

    _reassembler.push_substring(seg.payload().copy(), stream_index, _syn_received && _fin_received);
    _last_abs_seq = stream_out().bytes_written() + _syn_received + _fin_received;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_syn_received)
        return std::nullopt;

    return wrap(_last_abs_seq - (_reassembler.unassembled_bytes() && _fin_received), _isn);
}

size_t TCPReceiver::window_size() const {
    return stream_out().remaining_capacity();
}
