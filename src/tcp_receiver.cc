#include "tcp_receiver.hh"
using namespace std;
void TCPReceiver::receive( TCPSenderMessage message )
{
// Your code here.
  if(message.RST){
    RST=true;
    reassembler_.reader().set_error_r();
    return;
  }
  if(message.SYN){
    zero_point_=message.seqno;
    started_=true;
  }
  if(!started_) return;
  reassembler_.insert(
    message.seqno.unwrap(zero_point_,reassembler_.next_expected_idx())
    -!message.SYN,
    message.payload,
    message.FIN
  );
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  bool rst_flag=RST||reassembler_.reader().has_error();
  if(!started_)return {{},
    window_size(),
    rst_flag
  };
  return {
    zero_point_+reassembler_.next_expected_idx()
    +1+reassembler_.writer().is_closed(),
    window_size(),
    rst_flag
  };
}

uint16_t TCPReceiver::window_size() const
{
  return reassembler_.writer().available_capacity()>UINT16_MAX?
  UINT16_MAX:reassembler_.writer().available_capacity();
}