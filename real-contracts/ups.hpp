#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

#include "songs.hpp"


#include <vector>



using namespace std;
using namespace eosio;
using std::string;


class [[eosio::contract]] ups : public eosio::contract {
  
  using eosio::contract::contract;
private:
  
  // --- Bring in song, genres, moods, formats --- \\
  
  uint32_t timeunit;
  
  // Artist Types: { 1: solo, 2: group}
  
  std::string AUTH_ACCOUNTS[6] = { "currentxchng", "cxc", "cron.cxc", "pay.cxc", "ups.cxc", "helpups.cxc" }; // CHECK these are right
  
  enum up_type: uint8_t {
    SOL = 1,
    BLUX = 2,
    BIG = 3
  };
  
  
  struct [[eosio::table]] upslog {
    uint64_t upid;
    uint64_t songid;
    uint8_t ups_type;
    uint32_t ups_count;
    uint32_t tuid;
  
    uint64_t primary_key() const { return upid; }
    uint64_t by_songid() const { return songid; }
    uint64_t by_ups_type() const { return (uint64_t)ups_type; }
    uint64_t by_ups_count() const { return (uint64_t)ups_count; }
    uint64_t by_tuid() const { return (uint64_t)tuid; }
  };
  
  typedef multi_index<name("upslog"), upslog,
    eosio::indexed_by<"bysongid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_songid>>,
    eosio::indexed_by<"byupstype"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_ups_type>>,
    eosio::indexed_by<"byupscount"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_ups_count>>,
    eosio::indexed_by<"bytuid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_tuid>>
  > upslog_table;
  
  struct [[eosio::table]] totals {
    uint32_t song;
    uint8_t ups_type;
    uint32_t ups_count;
    uint32_t updated;
    
    uint64_t primary_key() const { return song; }
  };
  
  struct [[eosio::table]] listeners {
    name account;
    uint32_t first_vote;
    uint32_t last_vote;
    uint32_t total_sol_ups;
    uint32_t total_blu_ups;
    uint32_t total_big_ups;
    
    uint64_t primary_key() const { return account.value; }
  };
  
  struct [[eosio::table]] ious {
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
  
  struct [[eosio::table]] songs {
    uint32_t songid;
    name recipient;
    uint64_t template_id; // CHECK WARN Considering options and waiting on Emanate
    
    uint64_t primary_key() const { return songid; }
    // Waiting on Emanate to see if we'll use something to connect them
  };
  
  typedef multi_index<name("songs"), songs> songs_table;
  
  struct [[eosio::table]] artists {
    name account;
    string googleid; // CHECK this needs to be NON-plaintext (hash based on account + timestamp salt )
    string artist_name;
    vector<string> artist_info;
    
    uint64_t primary_key() const { return account.value; }
  };
  
  struct [[eosio::table]] artistgroups {
    string groupname;
    name internal_name; // CHECK is there benefit to a name (uint64_t) vs a simple uint32_t? 
    vector<name> artists;
    vector<string> artist_names;
    vector<int8_t> weights;
    vector<string> artist_info; // CHECK this will work, can pass empty strings, or is there an <auto> type, key value pairs
    uint8_t pay_position;  
    
    uint64_t primary_key() const { return internal_name.value; }
  };
  
  struct [[eosio::table]] internallog {
    uint32_t last_pay;
    uint32_t last_full_pay; 
    bool remaining; // Did we reach the end of who is owed to pay? 
    
    uint64_t primary_key() const { return (uint64_t) last_pay; } //CHECK if this is valid +  
  };
  
  void updateup(uint32_t ups_count, uint8_t ups_type, name account, uint64_t songid); 
  void logup(uint32_t ups_count, uint8_t ups_type, name account, uint64_t songid); 
  void removeups(name user); 
  void updatetotal(uint32_t ups_count, uint8_t ups_type, name account); 
  void updateiou(name sender, name receiver, uint32_t amount, bool subtract); 
  void removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
  void updatelisten(uint32_t ups_count, uint8_t ups_type, name account);
  void removelisten(name account);
  void removesong(uint64_t songid); // Removes all IOUs for song + song record (minimal)
  void deepremvsong(uint64_t songid); // Removes all records of Ups for this sond
  
  // --- Only AUTH_ACCOUNTS can update Googleid (Salted hash) --- \\ 
  void updateartist(name account, vector<string> artist_info, string artist_name, string googleid); //CHECK changing
  
  ious_table _ious;
  upslog_table _upslog;
  songs_table _songs;
  
  

  
public:
  
  [[eosio::on_notify("*::transfer")]] // Listens for any token transfer
  void on_transfer( const name from, const name to, const asset quantity, const std::string memo );
  
  [[eosio::action]]
  void payup(void); // Default call by AUTH_ACCOUNTS
  
  [[eosio::action]]
  void payup(name account); // User's call to pay themselves
  
  [[eosio::action]]
  void updateartist(name account, vector<string> artist_info, string artist_name);
  
  [[eosio::action]]
  void updategroup(name internal_name, string group_name, vector<string> artists, vector<int8_t> weights, vector<string> group_info);
  
  [[eosio::action]]
  void updatesong(uint32_t songid, song dasong); //CHECK I'm not sure how to put in song custom struct
};
