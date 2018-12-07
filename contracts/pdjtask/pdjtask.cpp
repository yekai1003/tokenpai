/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "pdjtask.hpp"

namespace eosio {

void token::create( name   issuer,
                    asset  maximum_supply )
{
    require_auth( _self );

    auto sym = maximum_supply.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( maximum_supply.is_valid(), "invalid supply");
    eosio_assert( maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable( _self, sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    eosio_assert( existing == statstable.end(), "token with symbol already exists" );

    statstable.emplace( _self, [&]( auto& s ) {
       s.supply.symbol = maximum_supply.symbol;
       s.max_supply    = maximum_supply;
       s.issuer        = issuer;
    });
}


void token::issue( name to, asset quantity, string memo )
{
    auto sym = quantity.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    stats statstable( _self, sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    eosio_assert( existing != statstable.end(), "token with symbol does not exist, create token before issue" );
    const auto& st = *existing;

    require_auth( st.issuer );
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must issue positive quantity" );

    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    statstable.modify( st, same_payer, [&]( auto& s ) {
       s.supply += quantity;
    });

    add_balance( st.issuer, quantity, st.issuer );

    if( to != st.issuer ) {
      SEND_INLINE_ACTION( *this, transfer, { {st.issuer, "active"_n} },
                          { st.issuer, to, quantity, memo }
      );
    }
}


void token::transfer( name    from,
                      name    to,
                      asset   quantity,
                      string  memo )
{
    eosio_assert( from != to, "cannot transfer to self" );
    require_auth( from );
    eosio_assert( is_account( to ), "to account does not exist");
    auto sym = quantity.symbol.code();
    stats statstable( _self, sym.raw() );
    const auto& st = statstable.get( sym.raw() );

    require_recipient( from );
    require_recipient( to );

    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    auto payer = has_auth( to ) ? to : from;

    sub_balance( from, quantity );
    add_balance( to, quantity, payer );
}

void token::sub_balance( name owner, asset value ) {
   accounts from_acnts( _self, owner.value );

   const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found" );
   eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );

   from_acnts.modify( from, owner, [&]( auto& a ) {
         a.balance -= value;
      });
}

void token::add_balance( name owner, asset value, name ram_payer )
{
    accounts to_acnts( _self, owner.value );
    auto to = to_acnts.find( value.symbol.code().raw() );
    if( to == to_acnts.end() ) {
       to_acnts.emplace( ram_payer, [&]( auto& a ){
         a.balance = value;
       });
    } else {
       to_acnts.modify( to, same_payer, [&]( auto& a ) {
         a.balance += value;
       });
    }
}




//pdjtask
void pdjtask::createtk( name creator, name worker, asset taskBonus, string memo )
{    
    require_auth(creator);
    require_auth(worker);

    auto sym = taskBonus.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );

    tasks tk( _self, sym.code().raw() );

    tk.emplace( creator, [&](auto &t){
        t.creator = creator;
        t.worker  = worker;
        t.taskID  = tk.available_primary_key();
        t.bonus   = taskBonus;
        t.remark  = memo;
    });
}

void pdjtask::commit( uint64_t taskID, name worker, string memo )
{
    //提交任务者必须与任务分配者是一个人
    print("hi,",name{_self});
    require_auth(worker);
    tasks tk( _self, worker.value );
    auto tkobj = tk.find(taskID);
    eosio_assert( tkobj != tk.end(), "taskid not exists" );
    eosio_assert( tkobj->worker == worker, "worker not same" );
    tk.modify(tkobj,worker,[&](auto &t){
        t.status = 1;
        t.comment = memo;
    });

}

void pdjtask::confirm( uint64_t taskID, name creator, uint8_t ok )
{
    require_auth(creator);
    tasks tk( _self, creator.value );
    auto tkobj = tk.find(taskID);
    eosio_assert( tkobj != tk.end(), "taskid not exists" );
    uint8_t status = 2;
    if ( !ok ) {
        // re do 
        status = 0;
    }
    tk.modify(tkobj,creator,[&](auto &t){
        t.status = status;
        t.comment = "well done!";
    });
    
    if ( ok ){
        //发币刺激
        transfer( creator, tkobj->worker, tkobj->bonus, "very good!" );
    }
    
}

} /// namespace eosio

EOSIO_DISPATCH( eosio::pdjtask, (createtk)(commit)(confirm)(create)(issue)(transfer) )
//EOSIO_DISPATCH( eosio::token, (create)(issue)(transfer))
