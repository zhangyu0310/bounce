/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the Socket.
* This is an internal header file, users should not include this.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
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
	int8_t peekInt8() {
		if (read_index_ + 1 > write_index_) {
			return 0;
		}
		const void* ptr = peek();
		return *static_cast<const int8_t*>(ptr);
	}
	int16_t peekInt16() {
		if (read_index_ + 2 > write_index_) {
			return 0;
		}
		const void* ptr = peek();
		return *static_cast<const int16_t*>(ptr);
	}
	int32_t peekInt32() {
		if (read_index_ + 4 > write_index_) {
			return 0;
		}
		const void* ptr = peek();
		return *static_cast<const int32_t*>(ptr);
	}
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
	// unsafe
	int8_t readInt8() {
		if (1 > readableBytes()) {
			return 0;
		}
		read_index_ += 1;
		return peekInt8();
	}
	int16_t readInt16() {
		if (2 > readableBytes()) {
			return 0;
		}
		read_index_ += 2;
		return peekInt16();
	}
	int32_t readInt32() {
		if (4 > readableBytes()) {
			return 0;
		}
		read_index_ += 4;
		return peekInt32();
	}
	//readInt64();

private:
	void makeSpace(size_t len);

	ssize_t init_size_;
	std::vector<char> buffer_;
	SizeType read_index_;  // read from this index
	SizeType write_index_; // the next insert index
};

}

#endif // !BOUNCE_BUFFER_H