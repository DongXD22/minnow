#include "wrapping_integers.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return Wrap32 { zero_point+n };
}
uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  uint64_t pad=1LL<<32;
  uint64_t rest=checkpoint&(pad-1);
  uint64_t level=checkpoint-rest;
  uint64_t ans=(raw_value_+pad-zero_point.raw_value_)&(pad-1);
  if(ans>=(pad>>1)+rest&&level+ans>pad) return level+ans-pad;
  else if(rest>=(pad>>1)+ans)return level+ans+pad;
  else return level+ans;
}