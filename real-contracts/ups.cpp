#include "ups.hpp"
#include "helpups.cpp"

//WARN updateartist updategroup updatesong may have incorrect parameters

/*/
ACTION payup(void); // Default call 
ACTION payup(name upsender); // User's call to pay themselves
ACTION updateartist(name artistacc, vector<string> artistinfo, string artistalias);
ACTION updategroup(name intgroupname, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> groupinfo);
ACTION updatesong(uint32_t songid, vector<string>);
ACTION removesong(uint32_t songid)
ACTION deepremvsong(uint32_t songid)
/*/

// --- Receive tokens sent to contract + make ups --- \\
[[eosio::on_notify("sol.cxc::transfer")]] void ups::sol_catch( const name from, const name to, const asset quantity, const string memo )
{  
  uint32_t songid_upped;
  require_auth(get_self());
  
  // --- Check that we're the intended recipient --- \\ //CHECK Is this really needed
  if (to != _self) return; // internal function no need to check()

  // --- Token-symbol + Memo = Songid Check --- \\
  check(quantity.symbol == symbol("SOL", 0), "Accepting SOL and BLUX only");  
  songid_upped = stoi(memo); // Set memo (songid) to <int> 
  
  // --- Instantiate Table --- \\
  _songs(_self, _self.value);
  
  // --- Check for song in table --- \\ 
  auto song_iter = _songs.require_find( songid_upped, string( "Song " + to_string(songid_upped) + " was not found." ).c_str() );

  // --- Set up Variables --- \\
  uint32_t quantity = uint32_t(quantity);

  // --- Pass on to updateup() --- \\
  ups::updateup(quantity, 1, song_iter, upsender); // 1=SOL Ups (uint32_t quantity, uint8_t upstype, uint32_t songid, name upsender)
  
}//END listen->SOL ups 

//NOTE need separate LISTENER for Bluups

// --- Send all owed payments listed in |ious|  --- \\
ACTION ups::payup(void) {
  // check the time to ensure it's been 5 minutes
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  _internallog(get_self(), get_self().value);
  check(nftToTokenTable.get().primary_key() < (time_of_up + 3), "Please wait 5 seconds between each payup. ");//CHECK this syntax is correct
  
  // READ the |ious.cxc => ious| table for account
  _ious(get_self(), upsender.value);//CHECK scope
  //auto ious_iterator = _ious.find(iouid); 
  auto ious_iterator = _ious.get_index("initiated"_n)
  for ( auto itr = ious_iterator.rbegin(); itr >= ious_iterator.rbegin() - 12; itr++ ) {//CHECK (optimize/test) Goes 12 rows deep to avoid failed TX 
   /*/ itr->secondary
     uint64_t ious_iterator->iouid;
     name ious_iterator->upsender;
     name ious_iterator->upcatcher;
     uint8_t ious_iterator->artisttype;
     uint32_t ious_iterator->upscount // Should be either BIGSOL or sol up or both
     uint8_t ious_iterator->upstype
     uint32_t ious_iterator->initiated;
     uint32_t ious_iterator->updated; 
   /*/
   
   // --- Check Time for min 5 minutes since last payment --- \\
   
   
   
   if (ious_iterator->upstype ==  BIGSOL){
     // --- How many Big Ups --- \\
     auto big_ups_count = floor(ious_iterator->upscount / 64);
     // --- Mint NFTs for Big Ups --- \\
   }
 }//END for (12)


  
  
  //auto& user_iterator = _upslog.find(username.value); // WARN jumped to other thing, this is not good
  //auto& ur_ious = _ious.get(username.value, "User doesn't exist");
  // Make [] with each record of owed to depth of 12 rows, starting with oldest
  // if (account is group)
    // READ |artistgroups => payposition|
    // Compile [] of the members owed by counting off each position and weight until all reward is given out
  // TRANSFER the total sum owed to account[s]
  // UPDATE / DELETE |ups.cxc => ious| table to reflect changes 


}//END payup(void)

// --- Send owed payments listed in |ious| for one account --- \\
ACTION ups::payup(name upsender) {
// Same as above but with account
  //require_auth(username);

}//END payup(void)

// --- Register artist, or change artist information --- \\
ACTION ups::updateartist(name artistacc, vector<string> artistinfo, string artistalias) {
  // IF (exists |artists => account|)
  // UPDATE |artists => artistinfo|
  // Check if there is a change to 
  // else 
  // INSERT record into |artists| return;
}

// --- Register artist group, or change group information --- \\
ACTION ups::updategroup(name intgroupname,  vector<string> groupinfo, string group_alias, vector<string> artists, vector<int8_t> weights) {
  // CHECK (artists.length = weights.length OR 0 weights && 0 members) // 0 = no update, both or none

  // IF (exists |artistgroup => intgroupname|) // Check member list 
    // CHECK (exists |artistgroup => artists => account|)
    // UPDATE |artistgroups => artistinfo|)
  // else 
  // INSERT |artistgroups|
}


// --- WARN NEEDS REVIEW Update song info or receiving account --- \\
ACTION ups::updatesong(uint32_t songid, name artistacc) { 
  // NOTE : Music 
  // NOTE: Remove the '-' from genres coming from cXc.world, use enum
  // CHECK (sender = artist || sender = AUTH_ACCOUNT)
    // if (group) CHECK (sender is in |artistgroups|)
  // IF (exists |so => account|)
  // UPDATE |songs = songid|)
  // else 
  // INSERT |songs|
}


// --- Remove the song from earning potential --- \\
ACTION ups::removesong(uint32_t songid) {

}

// --- Remove all record of song in RAM --- \\
ACTION ups::deepremvsong(uint32_t songid) {

}
