/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is some function implementation of EventLoop.
* It's a public class, users can use this class.
*
* Use of this source code is governed by the GNU Lesser General Public.
*/

#include <bounce/event_loop.h>

void bounce::EventLoop::loop() {
	looping_ = true;
	while (!stop_) {
		poller_->poll();
	}
}