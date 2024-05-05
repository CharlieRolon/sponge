#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    uint32_t ret = isn.raw_value() + static_cast<uint32_t>(n);
    return WrappingInt32{ret};
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint64_t lo32 = static_cast<uint64_t>(n.raw_value() - isn.raw_value());
    uint64_t hi32l = (checkpoint - (1UL<<31)) & 0xFFFFFFFF00000000;
    uint64_t hi32r = (checkpoint + (1UL<<31)) & 0xFFFFFFFF00000000;
    uint64_t ret1 = lo32 | hi32l, ret2 = lo32 | hi32r;
    // choose the result closest to the checkpoint
    if (max(ret1, checkpoint) - min(ret1, checkpoint) < max(ret2, checkpoint) - min(ret2, checkpoint))
        return ret1;
    return ret2;
}
