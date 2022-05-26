
#include "eosio/eosio.hpp"
#include <eosio/asset.hpp>
#include <vector> //CHECK is needed? 

using namespace std;
using namespace eosio;
using std::string;


class [[eosio::contract]] ups : public contract {
  
  using contract::contract;
public: 

#include "songs.hpp"

struct song {
  string title;
  vector<string> links;
  vector<double> geoloc; // CHECK this is a real 
  uint8_t genre;
  uint8_t mood;
  uint8_t format;
  string atomictempid;
};

// --- Bring in song, genres, moods, formats --- \\ 
uint32_t timeunit;

// Artist Types: { 1: solo, 2: group}

std::string AUTH_ACCOUNTS[6] = { "currentxchng", "cxc", "cron.cxc", "pay.cxc", "ups.cxc", "helpups.cxc" }; // CHECK these are right

enum up_type: uint8_t {
  SOL = 1,
  BLUX = 2,
  BIG = 3
};

private:  
  TABLE upslog {
    uint64_t upid;
    uint32_t songid;
    uint8_t ups_type;
    uint32_t ups_count;
    uint32_t tuid;
  
    uint64_t primary_key() const { return upid; }
    uint64_t by_songid() const { return (uint64_t) songid; }
    uint64_t by_ups_type() const { return (uint64_t) ups_type; }
    uint64_t by_ups_count() const { return (uint64_t) ups_count; }
    uint64_t by_tuid() const { return (uint64_t)tuid; }
  };
  
  using upslog_table = multi_index<name("upslog"), upslog,
    eosio::indexed_by<"bysongid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_songid>>,
    eosio::indexed_by<"byupstype"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_ups_type>>,
    eosio::indexed_by<"byupscount"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_ups_count>>,
    eosio::indexed_by<"bytuid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_tuid>>
  >;
  
  TABLE totals {
    uint32_t songid;
    uint8_t ups_type;
    uint32_t ups_count;
    uint32_t updated;
    
    uint64_t primary_key() const { return songid; }
  };
  
  using totals_table = multi_index<name("totals"), totals>;
  
  // --- Connects cXc.world's DB to chain with songid, maintains NFT list for other dapps --- \\
  TABLE songs { //CHECK need to store the type of account here?
    uint32_t songid;
    name recipient;
    uint64_t template_id; // CHECK WARN Considering options and waiting on Emanate
    
    uint64_t primary_key() const { return (uint64_t) songid; }
    // Waiting on Emanate to see if we'll use something to connect them
  };
  
  using songs_table = multi_index<name("songs"), songs >;
  
  // --- Activity stats for Listeners (For future awards) --- \\
  TABLE listeners {
    name up_sender;
    uint32_t first_vote;
    uint32_t last_vote;
    uint32_t total_sol_ups;
    uint32_t total_blu_ups;
    uint32_t total_big_ups;
    
    uint64_t primary_key() const { return up_sender.value; }
  };
    using listeners_table = multi_index<name("listeners"), listeners>;
  
  // --- Store record of who to pay --- \\ 
  TABLE ious {
    uint64_t iouid;
    name sending_account;
    name receiving_account;
    uint8_t receiving_account_type;
    uint32_t ups_count;
    uint8_t ups_type;
    uint32_t initiated;
    
    uint64_t primary_key() const { return iouid; }
    uint64_t by_receiving_account() const { return receiving_account.value; }
    uint64_t by_receiving_account_type() const { return (uint64_t) receiving_account_type; }
    uint64_t by_ups_count() const { return (uint64_t)ups_count; }
    uint64_t by_initiated() const { return (uint64_t) initiated; }
  };
  
  using ious_table = multi_index<name("ious"), ious,
    eosio::indexed_by<"byrecacc"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_receiving_account>>,
    eosio::indexed_by<"byrecacctype"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_receiving_account_type>>,
    eosio::indexed_by<"byupscount"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_ups_count>>,
    eosio::indexed_by<"byinitiated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_initiated>>
  >;
  

  
  // --- Keep record of Artists (recipient in _songs) to pay --- \\
  TABLE artists {
    name artist_account;
    string artist_alias;
    vector<string> artist_info;
    
    uint64_t primary_key() const { return artist_account.value; }
  };
  using artists_table = multi_index<name("artists"), artists>;
  
  TABLE artistgroups {
    string groupname;
    name internal_name; // abcdef.cxc // CHECK is there benefit to a name (uint64_t) vs a simple uint32_t? 
    vector<name> artists;
    vector<string> artist_aliass;
    vector<int8_t> weights;
    vector<string> artist_info; // CHECK this will work, can pass empty strings, or is there an <auto> type, key value pairs
    uint8_t pay_position;  
    
    uint64_t primary_key() const { return internal_name.value; }
  };
  
    using groups_table = multi_index<name("artistgroups"), artistgroups>;
  
  
  
  TABLE internallog {
    uint32_t last_pay;
    uint32_t last_full_pay; 
    bool remaining; // Did we reach the end of who is owed to pay? 
    
    uint64_t primary_key() const { return (uint64_t) last_pay; } //CHECK if this is singleton
  };
  
  void updateup(uint32_t ups_count, uint8_t ups_type, name up_sender, uint32_t songid); 
  void logup(uint32_t ups_count, uint8_t ups_type, name up_sender, uint32_t songid); 
  void removeups(name user); 
  void updatetotal(uint32_t ups_count, uint8_t ups_type, name up_sender, uint32_t songid); 
  void updateiou(uint32_t ups_count, uint8_t ups_type, uint8_t method_sent, name up_sender, bool subtract); 
  void removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
  void updatelisten(uint32_t ups_count, uint8_t ups_type, name up_sender);
  void removelisten(name up_sender);
  void removesong(uint32_t songid); // Removes all IOUs for song + song record (minimal)
  void deepremvsong(uint32_t songid); // Removes all records of Ups for this sond
  
  // --- All info in *_info Artist info has removed Google rec for security --- \\ 
  void updateartist(name artist_wax, vector<string> artist_info, string artist_alias); 
  void updateartistgroup(name internal_name,  vector<string> group_info, string group_alias, vector<string> artists, vector<int8_t> weights);
  
  
  // --- Declare the _tables for later use --- \\ 
  ious_table _ious;
  upslog_table _upslog;
  songs_table _songs;
  listeners_table _listeners;
  artists_table _artists;
  groups_table _groups;
  totals_table _totals;
  
  
  

  
public:
  
  [[eosio::on_notify("sol.cxc::transfer")]] // CHECK REQUIRES correct contract for SOL/BLUX Listens for any token transfer
  void sol_catch( const name from, const name to, const asset quantity, const std::string memo );
  
  ACTION payup(void); // Default call
  
  ACTION payup(name up_sender); // User's call to pay themselves
  
  ACTION updateartist(name up_sender, vector<string> artist_info, string artist_alias);
  
  ACTION updategroup(name internal_name, string group_alias, vector<string> artists, vector<int8_t> weights, vector<string> group_info);
  
  ACTION updatesong(uint32_t songid, song dasong); //CHECK I'm not sure how to put in song custom struct
};
