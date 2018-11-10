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

#include <vector>

namespace bounce {

class Buffer {
public:
	Buffer();
	~Buffer();

private:
	std::vector<char> buffer_;
};

}

#endif // !BOUNCE_BUFFER_H