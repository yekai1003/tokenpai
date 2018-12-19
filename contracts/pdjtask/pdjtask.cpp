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
    uint8_t status;
    
    if(worker.length()){
        eosio_assert(is_account(worker),"没有这个账户");
    	status = 1;
    }
    else{
    	status = 0;
    }
    auto sym = taskBonus.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );

    tasks tk( _code, _code.value );

    tk.emplace( creator, [&](auto &t){
        t.creator = creator;
        t.worker  = worker;
        t.taskID  = tk.available_primary_key();
        t.bonus   = taskBonus;
        t.remark  = memo;
        t.status  = status;
    });
}

void pdjtask::receivetk( uint64_t taskID, name worker)
{
	print("hi,",name{_self});
	print("hi,",name{_code});
	tasks tk( _code, _code.value );
	require_auth(worker);

	auto tkobj = tk.find(taskID);
	eosio_assert( tkobj != tk.end(), "taskid not exists" );
	eosio_assert(tkobj->status == 0, "任务已被他人领取");

   	tk.modify(tkobj, worker ,[&](auto &t){
    	t.status = 1;
        t.worker = worker;
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
        t.status = 2;
        t.comment = memo;
    });

}

void pdjtask::confirm( uint64_t taskID, uint8_t ok )
{
    //require_auth(creator);
    tasks tk( _code, _code.value );
    auto tkobj = tk.find(taskID);
    eosio_assert( tkobj != tk.end(), "taskid not exists" );
    uint8_t status = 3;
    print("confirm---",name{tkobj->creator});
    //require_auth(tkobj->creator);
    if ( !ok ) {
        // re do 
        status = 1;
    }
    tk.modify(tkobj, tkobj->creator, [&](auto &t){
        t.status = status;
        t.comment = "well done!";
    });
    
    if ( ok ){
        //发币刺激
        //transfer( creator, tkobj->worker, tkobj->bonus, "very good!" );
        task_commit(tkobj->creator, tkobj->worker, tkobj->bonus, "very good!");
    }
    
}

} /// namespace eosio

EOSIO_DISPATCH( eosio::pdjtask, (createtk)(receivetk)(commit)(confirm) )
//EOSIO_DISPATCH( eosio::token, (create)(issue)(transfer))
