#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT aabadge : public contract {
  public:
    using contract::contract;

    ACTION initcoll(name org, name collection_name);

    [[eosio::on_notify(OPEN_PROFILE_BADGE_SUBSCRIPTION_NOTIFICATION)]] void notifyinit( 
      name sink;
      name org, 
      uint64_t badge_id, 
      name contract,
      name badge,
      vector<ipfs_hash> ipfs_hashes,
      vector<string> details,
      uint32_t rarity_counts);

    [[eosio::on_notify(ATOMIC_ASSETS_CREATE_TEMPLATE_NOTIFICATION)]] void updatebadge(
      int32_t template_id,
      name authorized_creator,
      name collection_name,
      name schema_name,
      bool transferable,
      bool burnable,
      uint32_t max_supply,
      ATTRIBUTE_MAP immutable_data); 

    [[eosio::on_notify(ATOMIC_ASSETS_CREATE_ASSET_NOTIFICATION)]] void notifyachiev(
      name org, 
      name contract, 
      name badge, 
      uint64_t badge_id, 
      name account, 
      uint8_t count, 
      vector<name> notify_accounts);

  private:
    TABLE aatemplate {
      uint64_t badge_id;
      uint32_t  template_id;
      auto primary_key() const { return badge_id; }
    };
    typedef multi_index<name("aatemplate"), aatemplate> aatemplate_table;

    TABLE aacollection {
      name org;
      name collection;
      auto primary_key() const {return org.value;}
      uint64_t collection_key() const {return collection.value;}
    };
    typedef multi_index<name("aacollection"), aacollection,
    indexed_by<name("colkey"), const_mem_fun<aacollection, uint64_t, &aacollection::collection_key>>
    > aacollection_table;
};
