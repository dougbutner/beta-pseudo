#include <eosio/eosio.hpp>
#include "songs.hpp"

using namespace std;
using namespace eosio;

class [[eosio::contract]] ups public eosio::contract {
private:
  
  int32_t timeunit;
  
  // Artist Types: { 1: solo, 2: group}
  
  std::string AUTH_ACCOUNTS[] const = { "currentxchng", "cxc", "cron.cxc", "pay.cxc", "ups.cxc", "helpups.cxc" }; // CHECK these are right
  
  enum up_type: uint8_t {
    SOL = 1,
    BLUX = 2,
    BIG = 3
  };
  
  enum method_sent: uint8_t {
    DAPP = 1,
    DAPPMOBILE = 2,
    DAPPFREE = 3,
    DAPPMOBILEFREE = 4,
    CONTRACT = 9
  };
  
  struct [[eosio::table]] ups {
    name account;
    int8_t account_type; // is it a group or a solo artist
    uint32_t songid;
    int8_t ups_type;
    uint32_t ups_count;
    enum method_sent;
    uint32_t tuid;
  };
  
  struct [[eosio::table]] totals {
    uint32_t song;
    enum ups_type;
    uint32_t ups_count;
    time_point_sec updated;
  };
  
  struct [[eosio::table]] listeners {
    name account;
    time_point_sec first_vote;
    time_point_sec last_vote;
    uint32_t total_sol_ups;
    uint32_t total_sol_ups;
    uint32_t total_big_ups;
  };
  
  struct [[eosio::table]] ious {
    name sending_account;
    name receiving_account;
    name receiving_account_type;
    uint32_t ups_count;
    enum ups_type;
    time_point_sec initiated;
  };
  
  struct [[eosio::table]] songs {
    uint32_t songid;
    name recipient;
    vector<string> song_info;
  };
  
  struct [[eosio::table]] artists {
    name account;
    string googleid; // CHECK this needs to be NON-plaintext (hash based on account + timestamp salt )
    string artist_name;
    vector<string> artist_info;
  };
  
  struct [[eosio::table]] artistgroups {
    string groupname;
    name internal_name; // CHECK is there benefit to a name (uint64_t) vs a simple uint32_t? 
    vector<string> artists;
    vector<int8_t> weights;
    vector<string> artist_info; // CHECK this will work, can pass empty strings, or is there an <auto> type, key value pairs
    int8_t pay_position;  
  };
  
  struct [[eosio::table]] internallog {
    time_point_sec last_pay; 
    bool remaining; // Did we reach the end of who is owed to pay? 
  };
  
  void updateup(uint32_t ups_count, int8_t ups_type, int8_t method_sent, name account); 
  void logup(uint32_t ups_count, enum ups_type, int8_t method_sent, name account); 
  void removeups(name user); 
  void updatetotal(uint32_t ups_count, enum ups_type, int8_t method_sent, name account); 
  void updateiou(name sender, name receiver, uint32_t amount, bool subtract); 
  void removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
  void updatelisten(uint32_t ups_count, enum ups_type, int8_t method_sent, name account);
  void removelisten(name account);
  
  // --- Only AUTH_ACCOUNTS can update Googleid (Salted hash) --- \\
  void updateartist(name account, vector<string> artist_info, string artist_name, string googleid); 

  
public:
  
  void payup(void); // Default call by AUTH_ACCOUNTS
  void payup(name account); // User's call to pay themselves
  void updateartist(name account, vector<string> artist_info, string artist_name);
  void updategroup(name internal_name, string group_name, vector<string> artists, vector<int8_t> weights, vector<string> group_info);
  void updatesong(uint32_t songid, song& song); //CHECK I'm not sure how to put in song custom struct
};
