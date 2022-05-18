
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

private:
  
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
  
  typedef multi_index<name("upslog"), upslog,
    eosio::indexed_by<"bysongid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_songid>>,
    eosio::indexed_by<"byupstype"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_ups_type>>,
    eosio::indexed_by<"byupscount"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_ups_count>>,
    eosio::indexed_by<"bytuid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_tuid>>
  > upslog_table;
  
  TABLE totals {
    uint32_t songid;
    uint8_t ups_type;
    uint32_t ups_count;
    uint32_t updated;
    
    uint64_t primary_key() const { return songid; }
  };
  
  TABLE listeners {
    name up_sender;
    uint32_t first_vote;
    uint32_t last_vote;
    uint32_t total_sol_ups;
    uint32_t total_blu_ups;
    uint32_t total_big_ups;
    
    uint64_t primary_key() const { return up_sender.value; }
  };
  
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
  
  typedef multi_index<name("ious"), ious,
    eosio::indexed_by<"byrecacc"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_receiving_account>>,
    eosio::indexed_by<"byrecacctype"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_receiving_account_type>>,
    eosio::indexed_by<"byupscount"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_ups_count>>,
    eosio::indexed_by<"byinitiated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_initiated>>
  > ious_table;
  
  TABLE songs {
    uint32_t songid;
    name recipient;
    uint64_t template_id; // CHECK WARN Considering options and waiting on Emanate
    
    uint64_t primary_key() const { return songid; }
    // Waiting on Emanate to see if we'll use something to connect them
  };
  
  typedef multi_index<name("songs"), songs> songs_table;
  
  TABLE artists {
    name up_sender;
    string googleid; // CHECK this needs to be NON-plaintext (hash based on account + timestamp salt )
    string artist_name;
    vector<string> artist_info;
    
    uint64_t primary_key() const { return up_sender.value; }
  };
  
  TABLE artistgroups {
    string groupname;
    name internal_name; // CHECK is there benefit to a name (uint64_t) vs a simple uint32_t? 
    vector<name> artists;
    vector<string> artist_names;
    vector<int8_t> weights;
    vector<string> artist_info; // CHECK this will work, can pass empty strings, or is there an <auto> type, key value pairs
    uint8_t pay_position;  
    
    uint64_t primary_key() const { return internal_name.value; }
  };
  
  TABLE internallog {
    uint32_t last_pay;
    uint32_t last_full_pay; 
    bool remaining; // Did we reach the end of who is owed to pay? 
    
    uint64_t primary_key() const { return (uint64_t) last_pay; } //CHECK if this is valid +  
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
  
  
  // CHECK - Is this instantiation innefficient? Better in actual actions? 
  ious_table _ious;
  upslog_table _upslog;
  songs_table _songs;
  

  
public:
  
  [[eosio::on_notify("sol.cxc::transfer")]] // Listens for any token transfer
  void sol_catch( const name from, const name to, const asset quantity, const std::string memo );
  
  [[eosio::action]]
  void payup(void); // Default call
  
  [[eosio::action]]
  void payup(name up_sender); // User's call to pay themselves
  
  [[eosio::action]]
  void updateartist(name up_sender, vector<string> artist_info, string artist_name);
  
  [[eosio::action]]
  void updategroup(name internal_name, string group_name, vector<string> artists, vector<int8_t> weights, vector<string> group_info);
  
  [[eosio::action]]
  void updatesong(uint32_t songid, song dasong); //CHECK I'm not sure how to put in song custom struct
};
