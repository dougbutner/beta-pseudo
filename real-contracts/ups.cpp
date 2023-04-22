#include "ups.hpp"
#include "helpups.cpp"

//WARN updateartist updategroup updatesong may have incorrect parameters

/*/ TODO
ACTION payup(name upsender); // User's call to pay themselves
ACTION updateartist(name artistacc, vector<string> artistinfo, string artistalias);
ACTION updategroup(name intgroupname, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> groupinfo);
ACTION updatesong(uint32_t songid, vector<string>);
ACTION removesong(uint32_t songid)
ACTION deepremvsong(uint32_t songid)
/*/

// --- Recieve SOL sent to contract + make ups --- \\
//NOTE UPs memo is either an integer of the songid, or the songid with " BIG appended" or any other string
[[eosio::on_notify("sol.cxc::transfer")]] void ups::sol_catch( const name from, const name to, const asset quantity, const string memo )
{  
  // --- Check that we're the intended recipient --- \\ 
  if (to != _self) return; 
  
  uint32_t songid_upped;
  require_auth(get_self());

  // --- Token-symbol + Memo = Songid Check --- \\
  check(quantity.symbol == symbol("SOL", 0), "Accepting SOL and BLUX only");  
  songid_upped = stoi(memo); // Set memo (songid) to <int> 
  
  // --- Instantiate Table --- \\
  _songs(_self, _self.value);
  
  // --- Check for song in table --- \\ 
  auto song_iter = _songs.require_find( songid_upped, string( "Song ID " + to_string(songid_upped) + " was not found." ) );

  // --- Set up Variables --- \\
  uint32_t quantity = uint32_t(quantity);
  
  // --- Check if BIG --- \\
  //EXPLAIN - Passing a character at the end of the memo (instead of just songid) triggers BIG check (app does "2947 BIG")
  auto ups_type;
  if(!isdigit( memo.back() ) ){
    ups_type = BIGSOL;
  } else {
    ups_type = SOL;
  }
  
  // --- Pass on to updateup() --- \\
  updateup(quantity.amount, ups_type, upsender, songid_upped, 0); // 1=SOL Ups (uint32_t quantity, uint8_t upstype, uint32_t songid, name upsender)
  
}//END listen->SOL ups 


// --- Receive BLUX sent to contract + make ups --- \\
[[eosio::on_notify("bluxbluxblux::transfer")]] void ups::blux_catch( const name from, const name to, const asset quantity, const string memo )
{  
  // --- Check that we're the intended recipient --- \\ 
  if (to != _self) return; 
  
  uint32_t songid_upped;
  require_auth(get_self());

  // --- Token-symbol + Memo = Songid Check --- \\
  check(quantity.symbol == symbol("BLUX", 0), "Accepting SOL and BLUX only");  
  songid_upped = stoi(memo); // Set memo (songid) to <int> 
  
  // --- Instantiate Table --- \\
  _songs(_self, _self.value);
  
  // --- Check for song in table --- \\ 
  auto song_iter = _songs.require_find( songid_upped, string( "Song ID " + to_string(songid_upped) + " was not found." ) );

  // --- Set up Variables --- \\
  uint32_t quantity = uint32_t(quantity);

  // --- Pass on to updateup() --- \\
  updateup(quantity.amount, 2, upsender, songid_upped, 0); // 2=BLUX Ups (uint32_t quantity, uint8_t upstype, uint32_t songid, name upsender)
  
}//END listen->BLUX ups 


// --- Send all owed payments listed in |ious| ROUTES to payupsender --- \\
ACTION ups::payup(void) {
  payupsender("cxc"_n); // Dummy value for dispatcher
}//END payup(void)

// --- Send owed payments listed in |ious| for one account ROUTES to payupsender  --- \\
ACTION ups::payup(name upsender) {
  payupsender(upsender);
}//END payup(void)

// --- Register artist, or change artist information --- \\
ACTION ups::updateartist(name artistacc, vector<string> artistinfo, string artistalias) {
  // IF (exists |artists => account|)
  
  // Check if authorized to update
  // --- DISPATCH to upsert_artist function 
  // UPDATE |artists => artistinfo|
  
  
  // Check if there is a change to 
  // else 
  // INSERT record into |artists| return;
}

// --- Register artist group, or change group information --- \\
ACTION ups::updategroup(name intgroupname, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> groupinfo) {
  //TODO Move AUTH check from internal upsert_groups function to here
  
  // --- Call the upsert helper function with payposition flag 9999 --- \\
  upsert_groups(intgroupname, group_alias, artists, weights, groupinfo, 9999);
}

/*/ SONG 
uint32_t songid;
name artistacc; 
uint8_t artisttype;
song song;

struct song {
  string title;
  vector<double> geoloc; //CHECK this should accomidate changes, be optional of possible, but that would defeat the geo point
  uint8_t genre;
  uint8_t mood;
  uint8_t format;
  string atomictempid;
};
/*/

// --- WARN NEEDS REVIEW Update song info or receiving account --- \\
//WARN CHECK TODO currrently only works with songs with NFTs
ACTION ups::updatesong( string title, vector<double> geoloc, uint8_t genre, uint8_t mood, uint8_t format, uint64_t atomictempid, name artistacc, name adderacc, uint32_t songid) { 
  require_auth(adderacc); 
  
  // --- Validate GMF Information --- \\
  check(genre <= G_LEN, string("Genre code isn't recognized"));
  check(mood <= M_LEN, string("Mood code isn't recognized"));
  check(format <= F_LEN, string("Format code isn't recognized"));
  
  // --- Validate Artist is signing --- \\
  
  //struct car c = {.year=1923, .make="Nash", .model="48 Sports Touring Car"};
  
  struct song song = {.title = title; .geoloc=geoloc; .genre=genre; .mood=mood; .format=format; .atomictempid=atomictempid;};
    // TODO if (group) CHECK (sender is in |artistgroups|)
    // TODO How are we allowing anyone to update? Tracking last updater? Checking the record of updates?
  upsert_song(song song, name artistacc, name adderacc, uint8_t artisttype, uint32_t songid, bool deleteme);
}


// --- Remove the song from earning potential --- \\
ACTION ups::removesong(uint32_t songid) {
 // --- Only We, Artist, and Uploader can remove the song --- \\
 bool authorized = false;
 if (has_auth(adderacc) || has_auth(artistacc) || has_auth("cxc"_n) || has_auth(eosio.code){
   authorized = true;
 }
 
 check(authorized, "Only artist and uploader can update this contract.");
 
 remove_song(songid);
}//END removesong()

// --- Remove all record of song in RAM --- \\
ACTION ups::deepremvsong(uint32_t songid) {
  // --- Only We + Artist can deep-remove the song --- \\
  bool authorized = false;
  if (has_auth(artistacc) || has_auth("cxc"_n) || has_auth(eosio.code){
    authorized = true;
  }
  
  check(authorized, "Only the artist can remove records of this song");
  
  // === Remove From Totals + Upslog tables === \\
  
  // --- Find the song in the _upslog table, erase, verify --- \\ by_songid bysongid
  auto upslog_iterator = _upslog.get_index<"bysongid"_n>();
  uint8_t stepper = 0;
  //del uint8_t max_steps = 33; //TODO make this a singleton to be able to update it OR just make this a const? which is worse
  if(upslog_iterator != _upslog.end()){// Records Exist
    for(auto loop_itr = upslog_iterator.find(songid), loop_itr != upslog_iterator.end(), loop_itr++){
      _upslog.erase(loop_itr);
      stepper++;
      if (stepper > max_steps){ // Protect against long calls
        break;
      }
    }
  }//END if(upslog_iterator != .end)
  
  //if(){} If End Reached, continue, else exit function (to avoid time limit)
  
  
  
  
  // --- Find the song in the _totals table, erase, verify --- \\
  auto totals_iterator = _totals.require_find(songid, string("Song not found"));
  _totals.erase(totals_iterator);   
  check(_totals.find(songid) == _totals.end(), "There was a problem erasing the totals");
  

  
  // --- Forward call to remover --- \\
  remove_song(songid);

}//END deepremovesong()
