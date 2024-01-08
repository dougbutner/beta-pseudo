#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include "atomicassets-interface.hpp"


using namespace eosio;

CONTRACT atomicgeo : public contract {
public:
    using contract::contract;

    ACTION registernft(const uint64_t atomicassets_id, const double geo_location, const std::string iso_country);

    [[eosio::on_notify("bluxbluxblux::transfer")]] 
    void on_notify(const name& from, const name& to, const asset& quantity, const std::string& memo);
  
    ACTION gettopnfts(const std::string iso_country, const uint64_t limit);

private:
    TABLE nft_registration {
        uint64_t atomicassets_id;
        double geo_location;
        uint64_t bluups;
        std::string iso_country;
        
        uint64_t primary_key() const { return atomicassets_id; }
        uint64_t by_bluups() const { return bluups; }
    };

    typedef multi_index<name("nfts"), nft_registration,
        indexed_by<name("bybluups"), const_mem_fun<nft_registration, uint64_t, &nft_registration::by_bluups>>> nfts_table;
};
