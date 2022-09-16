#include <orgsettings.hpp>

ACTION orgsettings::recognize (name org, name trusted_contract) {
  require_auth (org);
  authorized_contracts_table _authorized_contracts( _self, org.value );
  auto itr = _authorized_contracts.find(trusted_contract.value);
  check(itr == _authorized_contracts.end(), "<trusted_contract> already authorized to issues badges");
  _authorized_contracts.emplace(get_self(), [&](auto& row){
    row.trusted_contract = trusted_contract;
  });
}

ACTION orgsettings::initbadge() {
  // check authorization
  // local create
  // notify sinks
  // notify org specified 
}

ACTION orgsettings::achievement() {
  // check authorization
  // notify cumulative
  // notify sinks
  // notify org specified
}

