/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of Buffer.
* It's a public class, users can use this class.
*
* Distributed under the MIT License (http://opensource.org/licenses/MIT)
*/

#include <bounce/buffer.h>

#include <algorithm>

#include <sys/uio.h>

ssize_t bounce::BUFFER_INIT_SIZE = 1024;

void bounce::Buffer::append(const char* src, size_t len) {
	if (writeableBytes() < len) {
		makeSpace(len);
	}
	std::copy(src, src + len, &buffer_[write_index_]);
	write_index_ += len;
}

// Write space is not enough.
// If read_index_ is more than half of BUFFER_SIZE, forward data, else resize buffer. 
void bounce::Buffer::makeSpace(size_t len) {
	if (read_index_ > static_cast<SizeType>(init_size_ / 2)) {
		std::copy(&buffer_[read_index_], &buffer_[write_index_], &buffer_[0]);
		write_index_ -= read_index_;
		read_index_ = 0;
	}
	if (writeableBytes() < len) {
		buffer_.resize(buffer_.size() + len);
	}
}

ssize_t bounce::Buffer::readFd(int fd, int* errorno) {
	const size_t STACK_BUFFER_SIZE = 65536;
	struct iovec iov[2];
	char buf[STACK_BUFFER_SIZE];
	iov[0].iov_base = &buffer_[write_index_];
	iov[0].iov_len = writeableBytes();
	iov[1].iov_base = buf;
	iov[1].iov_len = STACK_BUFFER_SIZE;
	// readv will fill up buffer_ first.
	// If buffer_ is not enough, it will fill buf(on stack).
	ssize_t ret = ::readv(fd, iov, 2);

	if (ret < 0) {
		*errorno = errno;
	} else if (static_cast<ssize_t >(writeableBytes()) > ret) {
		// buffer_ is not filled up.
		write_index_ += ret;
	} else { // buffer_ is been filled up. move buf into buffer_.
		size_t write_able = writeableBytes();
		write_index_ = buffer_.size();
		append(buf, ret - write_able);
	}
	return ret;
}

std::string bounce::Buffer::readAsString(size_t size) {
	std::string str;
	size_t read_bytes = size;
	if (size > readableBytes()) {
		read_bytes = readableBytes();
	}
	str.append(&buffer_[read_index_], read_bytes);
	if (read_bytes == readableBytes()) {
		// read all, there is nothing in buffer. So reset the buffer.
		read_index_ = write_index_ = 0;
	} else {
		read_index_ += read_bytes;
	}
	return str;
}