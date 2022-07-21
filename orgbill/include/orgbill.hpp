#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

CONTRACT orgbill : public contract {
  public:
    using contract::contract;
    struct notify_orgs {
      name org;
      uint32_t total_credits;
      uint32_t used_credits;
    };

    [[eosio::on_notify("eosio.token::transfer")]] void buycredits(name from, name to, asset quantity, string memo);
    ACTION usecredit(name billed_org, uint8_t used_credits);
    ACTION notify (name org, uint32_t total_credits, uint32_t used_credits);
  private:
    TABLE credits {
      name    org;
      uint32_t  total_credits;
      uint32_t used_credits;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("credits"), credits> credits_table;
};