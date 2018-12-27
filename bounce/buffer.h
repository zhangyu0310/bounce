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
	const size_t kInt8Size = 1;
	const size_t kInt16Size = 2;
	const size_t kInt32Size = 4;
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
	Buffer& operator=(const Buffer& buffer) {
		if (&buffer == this) {
			return *this;
		}
		init_size_ = buffer.init_size_;
		buffer_.clear();
		for (auto it : buffer.buffer_) {
			buffer_.push_back(it);
		}
		read_index_ = buffer.read_index_;
		write_index_ = buffer.write_index_;
		return *this;
	}

	size_t readableBytes() { return write_index_ - read_index_; }
	size_t writeableBytes() { return buffer_.size() - write_index_; }

	// Peek
	const char* peek() { return buffer_.data() + read_index_; }
	int8_t peekInt8() {
		if (read_index_ + kInt8Size > write_index_) {
			return 0;
		}
		const void* ptr = peek();
		return *static_cast<const int8_t*>(ptr);
	}
	int16_t peekInt16() {
		if (read_index_ + kInt16Size > write_index_) {
			return 0;
		}
		const void* ptr = peek();
		return *static_cast<const int16_t*>(ptr);
	}
	int32_t peekInt32() {
		if (read_index_ + kInt32Size > write_index_) {
			return 0;
		}
		const void* ptr = peek();
		return *static_cast<const int32_t*>(ptr);
	}

	// Input
	void append(const std::string& src) {
		append(src.c_str(), src.size());
	}
	void append(const char* src, size_t len);
	ssize_t readFd(int fd, int* errorno);
	void writeInt8(int8_t num) {
		void* num_ptr = &num;
		append(static_cast<char*>(num_ptr), kInt8Size);
	}
	void writeInt16(int16_t num) {
		void* num_ptr = &num;
		append(static_cast<char*>(num_ptr), kInt16Size);
	}
	void writeInt32(int32_t num) {
		void* num_ptr = &num;
		append(static_cast<char*>(num_ptr), kInt32Size);
	}

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
		if (kInt8Size > readableBytes()) {
			return 0;
		}
		read_index_ += kInt8Size;
		return peekInt8();
	}
	int16_t readInt16() {
		if (kInt16Size > readableBytes()) {
			return 0;
		}
		read_index_ += kInt16Size;
		return peekInt16();
	}
	int32_t readInt32() {
		if (kInt32Size > readableBytes()) {
			return 0;
		}
		read_index_ += kInt16Size;
		return peekInt32();
	}

private:
	void makeSpace(size_t len);

	ssize_t init_size_;
	std::vector<char> buffer_;
	SizeType read_index_;  // read from this index
	SizeType write_index_; // the next insert index
};

}

#endif // !BOUNCE_BUFFER_H