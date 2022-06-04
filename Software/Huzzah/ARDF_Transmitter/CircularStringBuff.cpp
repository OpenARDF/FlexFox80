/**********************************************************************************************
    Copyright © 2022 Digital Confections LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in the
    Software without restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the
    following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
    FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

**********************************************************************************************/
#include <Arduino.h>
#include "CircularStringBuff.h"

CircularStringBuff::CircularStringBuff(size_t size)
{
  buf_ = new String[size];
  max_size_ = size;
}

void CircularStringBuff::reset()
{
  head_ = tail_;
  full_ = false;
}

bool CircularStringBuff::empty() const
{
  /*if head and tail are equal, we are empty */
  return (!full_ && (head_ == tail_));
}

bool CircularStringBuff::full() const
{
  return (full_);
}

size_t CircularStringBuff::capacity() const
{
  return (max_size_);
}

size_t CircularStringBuff::size() const
{
  size_t size = max_size_;

  if (!full_)
  {
    if (head_ >= tail_)
    {
      size = head_ - tail_;
    }
    else
    {
      size = max_size_ + head_ - tail_;
    }
  }

  return (size);
}

void CircularStringBuff::put(String item)
{
  buf_[head_] = item;

  if (full_)
  {
    tail_ = (tail_ + 1) % max_size_;
  }

  head_ = (head_ + 1) % max_size_;

  full_ = head_ == tail_;
}

String CircularStringBuff::get()
{
  if (empty())
  {
    return ("");
  }

  /*Read data and advance the tail (we now have a free space) */
  String val = buf_[tail_];
  full_ = false;
  tail_ = (tail_ + 1) % max_size_;

  return (val);
}
