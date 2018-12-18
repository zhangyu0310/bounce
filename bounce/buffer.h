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

extern ssize_t BUFFER_INIT_SIZE;

class Buffer {
	typedef std::vector<char>::size_type SizeType;
public:
	Buffer() :
	    init_size_(BUFFER_INIT_SIZE),
		buffer_(static_cast<size_t >(init_size_)),
		read_index_(0),
		write_index_(0) 
	{}
	~Buffer() = default;
	Buffer(const Buffer& buf) :
		init_size_(buf.init_size_),
		read_index_(buf.read_index_),
		write_index_(buf.write_index_) {
		for (auto it : buf.buffer_) {
			buffer_.push_back(it);
		}
	}
	Buffer(Buffer&& buf) noexcept :
		init_size_(buf.init_size_),
		buffer_(std::move(buf.buffer_)),
		read_index_(buf.read_index_),
		write_index_(buf.write_index_) {
	}
	// TODO: copy operator?

	size_t readableBytes() { return write_index_ - read_index_; }
	size_t writeableBytes() { return buffer_.size() - write_index_; }

	// Peek
	const char* peek() { return buffer_.data() + read_index_; }
	// TODO: some buffer function
	//int8_t peekInt8();
	//int16_t peekInt16();
	//int32_t peekInt32();
	//int64_t peekInt64();

	// Input
	void append(const std::string& src) {
		append(src.c_str(), src.size());
	}
	void append(const char* src, size_t len);
	ssize_t readFd(int fd, int* errorno);
	// TODO: some buffer function
	//void writeInt8();
	//void writeInt16();
	//void writeInt32();
	//void writeInt64();

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
	// TODO: some buffer function
	//int8_t readInt8();
	//int16_t readInt16();
	//int32_t readInt32();
	//readInt64();

private:
	void makeSpace(size_t len);

	ssize_t init_size_;
	std::vector<char> buffer_;
	SizeType read_index_;
	SizeType write_index_;
};

}

#endif // !BOUNCE_BUFFER_H