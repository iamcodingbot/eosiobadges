#include <orgbill.hpp>

void orgbill::buycredits(name from, name to, asset quantity, string memo) {
  credits_table _credits(get_self(), get_self().value);
  auto itr = _credits.find(name(memo).value);
  if(itr == _credits.end()) {
    _credits.emplace(get_self(), [&](auto& row) {
      row.org = name(memo);
      row.total_credits = quantity.amount;
      row.used_credits = 1;
    });
  } else {
    _credits.modify(itr, get_self(), [&](auto& row) {
      row.total_credits = row.total_credits + quantity.amount;
    });
  }
}

ACTION orgbill::usecredit(name billed_org, uint8_t credits_needed) {
  require_auth(name("profiles"));
  credits_table _credits(get_self(), get_self().value);
  auto itr = _credits.find(billed_org.value);

  check(itr != _credits.end(), "<org> never registered for credits");
  check(itr->total_credits - itr->used_credits - credits_needed >= 0, "Credits exhausted for <org>");

  if(itr->total_credits - itr->used_credits - credits_needed < 100) {
    notify(billed_org, itr->total_credits, itr->used_credits + credits_needed);
  } 
  _credits.modify(itr, get_self(), [&](auto& row) {
      row.used_credits = row.used_credits + credits_needed;
  });
}

ACTION orgbill::notify (name org, uint32_t total_credits, uint32_t used_credits) {
  require_auth(get_self());
  require_recipient(org);
}
