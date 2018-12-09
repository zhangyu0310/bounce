/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Socket.
* This is an internal header file, users should not include this.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#ifndef BOUNCE_BUFFER_H
#define BOUNCE_BUFFER_H

#include <string>
#include <vector>

namespace bounce {

class Buffer {
	typedef std::vector<char>::size_type SizeType;
	const size_t BUFFER_SIZE = 1024;  // TODO: Read Configuration to set BUFFER_SIZE.
public:
	Buffer() : 
		buffer_(BUFFER_SIZE),
		read_index_(0),
		write_index_(0) 
	{}

	size_t readableBytes() { return write_index_ - read_index_; }
	size_t writeableBytes() { return buffer_.size() - write_index_; }

	// Peek
	const char* peek() { return buffer_.data() + read_index_; }
	int8_t peekInt8();
	int16_t peekInt16();
	int32_t peekInt32();
	int64_t peekInt64();

	// Input
	void append(std::string src) {
		append(src.c_str(), src.size());
	}
	void append(const char* src, size_t len);
	ssize_t readFd(int fd, int* errorno);
	void writeInt8();
	void writeInt16();
	void writeInt32();
	void writeInt64();

	// Output
	std::string readAllAsString() {
		return readAsString(readableBytes());
	}
	std::string readAsString(size_t size);
	// used by send message.
	void retrieve(size_t len) {
		if (len < readableBytes()) {
			read_index_ += len;
		} else {
			// retrieve all, the buffer can be reset.
			read_index_ = write_index_ = 0;
		}
	}
	int8_t readInt8();
	int16_t readInt16();
	int32_t readInt32();
	int64_t readInt64();

private:
	void makeSpace(size_t len);

	std::vector<char> buffer_;
	SizeType read_index_;
	SizeType write_index_;
};

}

#endif // !BOUNCE_BUFFER_H