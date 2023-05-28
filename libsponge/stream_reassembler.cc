#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), _unassembles(capacity, '\0'), _unassemble_mask(capacity, false) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // DUMMY_CODE(data, index, eof);
    if (index > _next_index + _capacity) return;
    if (eof) _is_eof = true;
    if (_is_eof && empty() && data.empty()) {
        _output.end_input();
        return;
    }

    size_t end_index = data.size() + index;

    // not overlap
    if (index >= _next_index) {
        size_t astart = index - _next_index;
        size_t len = min(_output.remaining_capacity() - astart, data.size());
        if (len < data.size()) _is_eof = false;
        write_unassemble(data, 0, len, astart);
    }
    // overlap
    else if (end_index > _next_index) {
        size_t dstart = _next_index - index;
        size_t len = min(_output.remaining_capacity(), data.size() - dstart);
        if (len < data.size() - dstart) _is_eof = false;
        write_unassemble(data, dstart, len, 0);
    }
    assemble();
    if (_is_eof && empty()) _output.end_input();
}

void StreamReassembler::write_unassemble(const string &data, size_t dstart, size_t len, size_t astart) {
    for (size_t i = 0; i < len; ++i) {
        if (_unassemble_mask[i + astart]) continue;
        _unassembles[astart + i] = data[dstart + i];
        _unassemble_mask[astart + i] = true;
        _unassemble_bytes++;
    }
}

void StreamReassembler::assemble() {
    string s;
    while (_unassemble_mask.front()) {
        s.push_back(_unassembles.front());
        _unassembles.pop_front();
        _unassemble_mask.pop_front();
        _unassembles.push_back('\0');
        _unassemble_mask.push_back(false);
    }

    if (s.empty()) return;

    _output.write(s);
    _next_index += s.size();
    _unassemble_bytes -= s.size();
}

size_t StreamReassembler::unassembled_bytes() const { return _unassemble_bytes; }

bool StreamReassembler::empty() const { return _unassemble_bytes == 0; }
