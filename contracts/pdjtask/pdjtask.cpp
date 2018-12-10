/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "pdjtask.hpp"

namespace eosio {


//pdjtask
void pdjtask::createtk( name creator, name worker, asset taskBonus, string memo )
{    
    require_auth(creator);
    //require_auth(worker);

    auto sym = taskBonus.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );

    tasks tk( _code, _code.value );

    tk.emplace( creator, [&](auto &t){
        t.creator = creator;
        t.worker  = worker;
        t.taskID  = tk.available_primary_key();
        t.bonus   = taskBonus;
        t.remark  = memo;
    });
}

void pdjtask::commit( uint64_t taskID, string memo )
{
    //提交任务者必须与任务分配者是一个人
    print("hi,",name{_self});
    //require_auth(worker);
    tasks tk( _code, _code.value );
    auto tkobj = tk.find(taskID);
    eosio_assert( tkobj != tk.end(), "taskid not exists" );
    require_auth(tkobj->worker );
    //eosio_assert( tkobj->worker == worker, "worker not same" );
    tk.modify(tkobj, tkobj->worker ,[&](auto &t){
        t.status = 1;
        t.comment = memo;
    });

}

void pdjtask::confirm( uint64_t taskID, uint8_t ok )
{
    //require_auth(creator);
    tasks tk( _code, _code.value );
    auto tkobj = tk.find(taskID);
    eosio_assert( tkobj != tk.end(), "taskid not exists" );
    uint8_t status = 2;
    print("confirm---",name{tkobj->creator});
    //require_auth(tkobj->creator);
    if ( !ok ) {
        // re do 
        status = 0;
    }
    tk.modify(tkobj, tkobj->creator, [&](auto &t){
        t.status = status;
        t.comment = "well done!";
    });
    
    if ( ok ){
        //发币刺激
        //transfer( creator, tkobj->worker, tkobj->bonus, "very good!" );
        //task_commit(tkobj->creator, tkobj->worker, tkobj->bonus, "very good!");
    }
    
}

} /// namespace eosio

EOSIO_DISPATCH( eosio::pdjtask, (createtk)(commit)(confirm) )
//EOSIO_DISPATCH( eosio::token, (create)(issue)(transfer))
