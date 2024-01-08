#include "atomicgeo.hpp"

ACTION atomicgeo::registernft(const uint64_t atomicassets_id, const double geo_location, const std::string iso_country) {
    require_auth(get_self());

    nfts_table nfts(get_self(), get_self().value);
    auto nft_itr = nfts.find(atomicassets_id);

    check(nft_itr == nfts.end(), "NFT with this ID already exists.");

    nfts.emplace(get_self(), [&](auto& row) {
        row.atomicassets_id = atomicassets_id;
        row.geo_location = geo_location;
        row.bluups = 0;
        row.iso_country = iso_country;
    });
}

void atomicgeo::on_notify(const name& from, const name& to, const asset& quantity, const std::string& memo) {
    if (to != get_self() || quantity.symbol != symbol("BLUX", 0)) return;

    // SUPER INSECURE 

    uint64_t nft_id = std::stoull(memo);
    nfts_table nfts(get_self(), get_self().value);
    auto nft_itr = nfts.find(nft_id);

    check(nft_itr != nfts.end(), "NFT not registered.");

    nfts.modify(nft_itr, same_payer, [&](auto& row) {
        row.bluups++;
    });
}

ACTION atomicgeo::gettopnfts(const std::string iso_country, const uint64_t limit) {
    require_auth(get_self());

    nfts_table nfts(get_self(), get_self().value);

    std::vector<nft_registration> top_nfts;
    uint64_t count = 0;

    if (iso_country.empty()) {
        // Get top NFTs globally
        auto bluups_index = nfts.get_index<name("bybluups")>();
        for (auto itr = bluups_index.rbegin(); itr != bluups_index.rend() && count < limit; ++itr, ++count) {
            top_nfts.push_back(*itr);
        }
    } else {
        // Get top NFTs for a specific country
        for (auto itr = nfts.begin(); itr != nfts.end() && count < limit; ++itr) {
            if (itr->iso_country == iso_country) {
                top_nfts.push_back(*itr);
                ++count;
            }
        }
        // Sort the vector based on 'bluups'
        std::sort(top_nfts.begin(), top_nfts.end(), [](const nft_registration& a, const nft_registration& b) {
            return a.bluups > b.bluups;
        });
    }
    for (const auto& nft : top_nfts) {
        print(nft.atomicassets_id, ", ", nft.geo_location, ", ", nft.bluups, ", ", nft.iso_country, "\n");
    }
}
