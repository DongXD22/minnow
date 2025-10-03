#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
	uint64_t win_start=next_expected_idx;
	uint64_t win_end=win_start+output_.writer().available_capacity();
	uint64_t cur_start=first_index;
	uint64_t cur_end=cur_start+data.length();

	if(is_last_substring){
		eof_idx=cur_end;
	}
	if(cur_start>win_end){
		return;
	}

	uint64_t start_idx=max(cur_start,win_start);
	uint64_t end_idx=min(cur_end,win_end);
	if (end_idx <= start_idx) {
		if (is_last_substring) {
			output_.writer().close();
		}
		return;
	}
	uint64_t len=end_idx-start_idx;
	data=data.substr(start_idx-cur_start,len);
	buf_.insert({start_idx,end_idx,data});

	Interval last=*buf_.begin();
	auto it=buf_.begin();
	it++;
	vector<Interval> managed;
	while(it!=buf_.end()){
		if(it->start<=last.end){
			if(last.end<it->end){
				last.data+=it->data.substr(last.end-it->start);	
				last.end=it->end;	
			}
		}
		else{
			managed.push_back(last);
			last=*it;
		}
		it++;
	}
	
	managed.push_back(last);
	buf_.clear();
	for(auto x:managed){
		buf_.insert(x);
	}

	it=buf_.begin();
	while(it->start==next_expected_idx){
		output_.writer().push(it->data);
		next_expected_idx=it->end;
		it=buf_.erase(it);
	}
	if(next_expected_idx==eof_idx){
		output_.writer().close();
	}

}

uint64_t Reassembler::bytes_pending() const
{
	uint64_t ans=0;
	for(const auto& x:buf_){
		ans+=x.end-x.start;
	}
  	return ans;
}
