#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity),
                                                              _stream(capacity), _cur_index(0), 
                                                              _unassembled_bytes(0), _eof_index(numeric_limits<uint64_t>::max()){}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // start position of substring (avoid overlap)
    auto start = max(index, _cur_index);
    // end position of substring
    auto end = min(index + data.size(), min(_cur_index + _capacity - _output.buffer_size(), _eof_index));
    if (eof) {
        if (_eof_index == numeric_limits<uint64_t>::max()) _eof_index = index + data.size();
        else if (_eof_index != index + data.size())
            throw runtime_error("StreamReassembler::push_substring: Inconsistent EOF indexes!");
    }
    // skip the overlapped part, write substring into cache
    for (size_t i = start, j = start - index; i < end; i++, j++) {
        auto &t = _stream[i % _capacity];
        if (t.second == true) {
            if (t.first != data[j])
                throw runtime_error("StreamReassembler::push_substring: Inconsistent substring!");
        } else {
            t = make_pair(data[j], true);
            ++_unassembled_bytes;
        }
    }
    string str;
    // write the contiguous substrings from cache into the output stream in order
    while (_cur_index < _eof_index && _stream[_cur_index % _capacity].second == true) {
        str.push_back(_stream[_cur_index % _capacity].first);
        _stream[_cur_index % _capacity] = {0, false};
        --_unassembled_bytes, ++_cur_index;
    }
    _output.write(str);
    if (_cur_index == _eof_index) _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_bytes; }

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }