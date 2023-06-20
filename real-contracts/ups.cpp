#include "ups.hpp"
#include "helpups.cpp"

//WARN updateartist updategroup updatesong may have incorrect parameters

// --- Recieve SOL sent to contract + make ups --- //
//NOTE UPs memo is either an integer of the songid, or the songid with " BIG appended" or any other string
[[eosio::on_notify("sol.cxc::transfer")]] void ups::sol_catch( const name from, const name to, const asset quantity, const string memo )
{  
  // --- Check that we're the intended recipient --- // 
  if (to != _self) return; 
  
  uint32_t songid_upped;
  require_auth(get_self());

  // --- Token-symbol + Memo = Songid Check --- //
  check(quantity.symbol == symbol("SOL", 0), "Accepting SOL and BLUX only");  
  songid_upped = stoi(memo); // Set memo (songid) to <int> 
  
  // --- Instantiate Table --- //
  _songs(_self, _self.value);
  
  // --- Check for song in table --- // 
  auto song_iter = _songs.require_find( songid_upped, string( "Song ID " + to_string(songid_upped) + " was not found." ) );

  // --- Set up Variables --- //
  //uint32_t quantity = uint32_t(quantity);
  uint32_t quantity_uint = static_cast<uint32_t>(quantity.amount); 
  
  // --- Check if BIG --- //
  //EXPLAIN - Passing a character at the end of the memo (instead of just songid) triggers BIG check (app does "2947 BIG")
  auto ups_type;
  if(!isdigit( memo.back() ) ){
    ups_type = BIGSOL;
  } else {
    ups_type = SOL;
  }
  
  // --- Pass on to updateup() --- //
  updateup(quantity_uint, 1, from, songid_upped, 0); // 1=SOL Ups (uint32_t quantity, uint8_t upstype, uint32_t songid, name upsender)
  
}//END listen->SOL ups 


// --- Receive BLUX sent to contract + make ups --- //
[[eosio::on_notify("bluxbluxblux::transfer")]] void ups::blux_catch( const name from, const name to, const asset quantity, const string memo )
{  
  // --- Check that we're the intended recipient --- // 
  if (to != _self) return; 
  
  uint32_t songid_upped;
  require_auth(get_self());

  // --- Token-symbol + Memo = Songid Check --- //
  check(quantity.symbol == symbol("BLUX", 0), "Accepting SOL and BLUX only");  
  songid_upped = stoi(memo); // Set memo (songid) to <int> 
  
  // --- Instantiate Table --- //
  _songs(_self, _self.value);
  
  // --- Check for song in table --- // 
  auto song_iter = _songs.require_find( songid_upped, string( "Song ID " + to_string(songid_upped) + " was not found." ) );

  // --- Set up Variables --- //
  //uint32_t quantity = uint32_t(quantity);
  uint32_t quantity_uint = static_cast<uint32_t>(quantity.amount); //ChatGPT said this was better


  // --- Pass on to updateup() --- //
  updateup(quantity_uint, 2, from, songid_upped, 0); // 2=BLUX Ups (uint32_t quantity, uint8_t upstype, uint32_t songid, name upsender)
  
}//END listen->BLUX ups 


// --- Send all owed payments listed in |ious| ROUTES to payupsender --- //
ACTION ups::payup(void) {
  check(_internallog.get().primary_key() < (time_of_up + 4), "Someone just called the action, try again in a few seconds.");

  payupsender("cxc"_n); // Dummy value for dispatcher
}//END payup(void)

// --- Send owed payments listed in |ious| for one account ROUTES to payupsender  --- //
ACTION ups::payup(name upsender) {
  payupsender(upsender);
}//END payup(void)

// --- Register artist, or change artist information --- //
ACTION ups::updateartist(name artistacc, vector<string> artistinfo, string artistalias) {
  auto artist_itr = _artists.find(artistacc.value);
  if (artist_itr == _artists.end()) { 
    // If artist does not exist, insert a new record
    _artists.emplace(get_self(), [&](auto& row){
      row.artistacc = artistacc;
      row.artistalias = artistalias;
      row.artistinfo = artistinfo;
    });
  } else {
    // Check if caller has authority
    check(has_auth(artistacc), "Only the artists account can update their information");
    // If artist already exists, update the record
    _artists.modify(artist_itr, get_self(), [&](auto& row){
      row.artistalias = artistalias;
      row.artistinfo = artistinfo;
    });    
  }

}

// --- Register artist group, or change group information --- //
ACTION ups::updategroup(name intgroupname, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> groupinfo) {
    // Check if at least one artist in the group is authorized
  bool authorized = false;
  
  for (auto const& artist : artists) {
    name artist_name(artist);
    if (has_auth(artist_name)) {
      authorized = true;
      break;
    }
  }
  
  check(authorized, "At least one artist from the group must authorize the update");
  
  // --- Call the upsert helper function with payposition flag 9999 --- //
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
  uint64_t atomictempid;
};
/*/

// --- WARN NEEDS REVIEW Update song info or receiving account --- //
//WARN CHECK TODO currrently only works with songs with NFTs
ACTION ups::updatesong( string title, vector<double> geoloc, uint8_t genre, uint8_t mood, uint8_t format, uint64_t atomictempid, name artistacc, name adderacc, uint32_t songid) { 
  require_auth(adderacc); 
  
  // --- Validate GMF Information --- //
  check(genre <= G_LEN, string("Genre code isn't recognized"));
  check(mood <= M_LEN, string("Mood code isn't recognized"));
  check(format <= F_LEN, string("Format code isn't recognized"));
  
  // --- Validate Artist is signing --- //
  
  //struct car c = {.year=1923, .make="Nash", .model="48 Sports Touring Car"};
  //struct song s = {.title = title, .geoloc=geoloc, .genre=genre, .mood=mood, .format=format, .atomictempid=atomictempid};
  song s{.title = title, .geoloc = geoloc, .genre = genre, .mood = mood, .format = format, .atomictempid = atomictempid};
  
    
    // TODO if (group) CHECK (sender is in |artistgroups|)
    // TODO How are we allowing anyone to update? Tracking last updater? Checking the record of updates?
  upsert_song(s, artistacc, adderacc, artisttype, songid, false);
}

// --- Remove the song from current earners  --- //
ACTION ups::removesong(uint32_t songid) {
 // --- Only We, Artist, and Uploader can remove the song --- //
  auto song_itr = _songs.require_find(songid, "Song does not exist. Good job, Ninja person.");
  check(has_auth(song_itr->adderacc) || has_auth(song_itr->artistacc) || has_auth("cxc"_n) || has_auth(_self), "Unauthorized: Only the uploader, artist, cxc, or the contract itself can remove this song");
   _songs.erase(song_itr);
    check(_songs.find(songid) == _upslog.end(), "There was a problem erasing the records from the upslog table.");
}//END removesong()

// --- Remove all record of song in RAM --- //
ACTION ups::deepremvsong(uint32_t songid) {
  auto song_itr = _songs.require_find(songid, "Song does not exist. Good job, Ninja person.");
  check(has_auth(song_itr->adderacc) || has_auth(song_itr->artistacc) || has_auth("cxc"_n) || has_auth(_self), "Unauthorized: Only the uploader, artist, cxc, or the contract itself can remove this song");

  // Erase from _upslog
  auto upslog_itr = _upslog.find(songid);
  while (upslog_itr != _upslog.end() && upslog_itr->songid == songid) {
    upslog_itr = _upslog.erase(upslog_itr);
  }
  check(_upslog.find(songid) == _upslog.end(), "There was a problem erasing the records from the upslog table.");

  // Erase from _totals
  auto totals_itr = _totals.find(songid);
  if (totals_itr != _totals.end()) {
    _totals.erase(totals_itr);
  }
  check(_totals.find(songid) == _totals.end(), "There was a problem erasing the up totals from the totals table.");

    // Erase from _songs, no ckeck
  _songs.erase(song_itr);

}//END deepremvsong()

