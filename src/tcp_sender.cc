#include "tcp_sender.hh"
#include "tcp_config.hh"

using namespace std;

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // Your code here.
  return msgs_unchecked_size_;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  // Your code here.
  return retrans_timers_;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  // Your code here.
  uint16_t curr_window_size=max((uint16_t)1,window_size_);
  while(curr_window_size>msgs_unchecked_size_){
    TCPSenderMessage msg={
      Wrap32::wrap(next_abs_seqno_,isn_),
      !syn_sent_,
      {},
      false,
      input_.reader().has_error(),
    };
    syn_sent_=true;

    if(msg.RST){
      msg.SYN=false;
      transmit(msg);
      return;
    }

    size_t payload_length=min(curr_window_size-msg.sequence_length()-msgs_unchecked_size_,TCPConfig::MAX_PAYLOAD_SIZE);

    read(reader(),payload_length,msg.payload);

    if(reader().is_finished()
      &&curr_window_size>msgs_unchecked_size_+msg.sequence_length()
      &&!fin_sent_
    ){
      msg.FIN=true;
      fin_sent_=true;
    }

    if(msg.sequence_length()==0) break;
    
    if(!timer_.started()) timer_.start(initial_RTO_ms_);

    transmit(msg);

    msgs_unchecked_.push(msg);
    msgs_unchecked_size_+=msg.sequence_length();
    next_abs_seqno_+=msg.sequence_length();

    if(msg.FIN) break;
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  // Your code here.
  return {
    Wrap32::wrap(next_abs_seqno_,isn_),
    false,
    "",
    false,
    input_.reader().has_error()
  };
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // Your code here.
  if(msg.RST){
    input_.set_error();
    return;
  }

  window_size_=msg.window_size;
  
  if (!msg.ackno.has_value()) {
    return;
  }

  auto left_no=(msg.ackno.value()).unwrap(isn_,next_abs_seqno_);

  if(left_no>next_abs_seqno_){
    return;
  }

  if(left_no>last_ackno_){
    last_ackno_ = left_no;
    while(!msgs_unchecked_.empty()) {
      const auto &curr_msg=msgs_unchecked_.front();
      uint64_t msg_left_no=curr_msg.seqno.unwrap(isn_,next_abs_seqno_);
      uint64_t msg_right_no=msg_left_no+curr_msg.sequence_length();

      if(left_no<msg_right_no) break;
      
      msgs_unchecked_size_-=curr_msg.sequence_length();
      msgs_unchecked_.pop();
    }

    RTO_=initial_RTO_ms_;
    retrans_timers_=0;
    if(msgs_unchecked_.empty()) {
      timer_.close();
    }
    else{
      timer_.start(RTO_);
    }
  }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  // Your code here.
  timer_.time_pass(ms_since_last_tick);
  if(timer_.ring()){
    const auto &curr_msg=msgs_unchecked_.front();
    transmit(curr_msg);
    if(window_size_){
      retrans_timers_++;
      RTO_*=2;
    }
    timer_.start(RTO_);
  }
}
