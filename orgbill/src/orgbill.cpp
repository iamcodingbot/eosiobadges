#include <orgbill.hpp>

// todo 
// 1) check for system currency in buycredits notified action.
// 2) buyram action from system currency.
// 3) put profiles contract in a variable.
// 4) error messages replace with value.
// 5) org's account to notify when credits run out.

ACTION orgbill::addsettings (name key, uint32_t value) {
  require_auth(get_self());
  settings_table _settings(get_self(), get_self().value);
  auto itr = _settings.find(key.value);
  if(itr == _settings.end()) {
    _settings.emplace(get_self(), [&](auto& row) {
      row.key = key;
      row.value = value;
    });
  } else {
    _settings.modify(itr, get_self(), [&](auto& row) {
      row.value = value;
    });
  }
}

void orgbill::buycredits(name from, name to, asset quantity, string memo) {
  if (to != get_self() || from == get_self()){
    eosio::print("Ignoring token transfer of contract to itself and from itself.");
    return;
  }
  credits_table _credits(get_self(), get_self().value);
  auto itr = _credits.find(name(memo).value);
  uint32_t credits_bought = token_to_credits (quantity.amount);

  if(itr == _credits.end()) {
    check(credits_bought <= 10000, "can not buy more than 10000 credits");
    _credits.emplace(get_self(), [&](auto& row) {
      row.org = name(memo);
      row.total_credits = credits_bought;
      row.used_credits = 1;
    });
  } else {
    _credits.modify(itr, get_self(), [&](auto& row) {
      check(row.total_credits + credits_bought <= 10000, "can not hold more than 10000 credits");
      row.total_credits = row.total_credits + credits_bought;
    });
  }
}

ACTION orgbill::usecredit(name billed_org, uint64_t bytes_consumed) {
  require_auth(name(PROFILE_CONTRACT_NAME));
  credits_table _credits(get_self(), get_self().value);
  auto itr = _credits.find(billed_org.value);
  uint8_t credits_needed = bytes_to_credits(bytes_consumed);
  check(itr != _credits.end(), "<org> never registered for credits");
  check(itr->total_credits - itr->used_credits - credits_needed >= 0, "Credits exhausted for <org>");

  // todo use %age or something else as threshold
  if(itr->total_credits - itr->used_credits - credits_needed < 100) {
    action {
      permission_level{get_self(), name("active")},
      get_self(),
      name("notify"),
      notify_args {
        .org = billed_org,
        .total_credits = itr->total_credits,
        .used_credits = itr->used_credits + credits_needed}
    }.send();
  } 
  _credits.modify(itr, get_self(), [&](auto& row) {
      row.used_credits = row.used_credits + credits_needed;
  });
}



ACTION orgbill::notify (name org, uint32_t total_credits, uint32_t used_credits) {
  require_auth(get_self());
  require_recipient(org);
}


