#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity )             // 初始化 capacity_
  , error_( false )                   // 初始化 error_
  , is_closed_( false )               // 初始化 is_closed_
  , buffer_()                         // 初始化 buffer_ 为空字符串 (调用默认构造函数)
  , bytes_pushed_count_( 0 )          // 初始化 bytes_pushed_count_
  , bytes_popped_count_( 0 )          // 初始化 bytes_popped_count_
{} 

bool Writer::is_closed() const
{
  // Your code here.
  return is_closed_;
}

void Writer::push( string data )
{
  // Your code here.
  if(is_closed()||has_error()){
    return ;
  }

  uint64_t bytes_to_push=min(data.length(),available_capacity());
  if(bytes_to_push==0){
    return;
  }

  buffer_.append(data,0,bytes_to_push);
  bytes_pushed_count_+=bytes_to_push;
  return;
}

void Writer::close()
{
  // Your code here.
  is_closed_=true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_-buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_count_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return buffer_.size()==0&&is_closed_;
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_popped_count_;
}

string_view Reader::peek() const
{
  // Your code here.
  if(has_error()){
    return {};
  }
  if(buffer_.empty()){
    return string_view();
  }
  return {string_view(buffer_.data(),buffer_.size())};
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if(has_error()){
    return;
  }
  uint64_t len_to_pop=min(buffer_.size(),len);
  buffer_.erase(buffer_.begin(),buffer_.begin()+len_to_pop);
  bytes_popped_count_+=len_to_pop;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffer_.size();
}
