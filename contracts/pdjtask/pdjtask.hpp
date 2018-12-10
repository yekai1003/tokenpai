/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

   using std::string;

   
class [[eosio::contract]] pdjtask : public contract {
   public:
      pdjtask(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

      //创建任务
      [[eosio::action]]
      void createtk( name creator, name worker, asset taskBonus, string memo );
      //提交任务
      [[eosio::action]]
      void commit( uint64_t taskID, string memo );
      //验收任务
      [[eosio::action]]
      void confirm( uint64_t taskID, uint8_t ok = 1 );

   private:

      struct [[eosio::table]] task {
         uint64_t    taskID;
         name        creator; 
         name        worker;
         asset       bonus;
         uint8_t     status = 0;
         string      remark;
         string      comment;

         uint64_t primary_key()const { return taskID; }
      };

      typedef eosio::multi_index< "tasks"_n, task > tasks;
   private:

      /*
      void transfer( name    from,
                        name    to,
                        asset   quantity,
                        string  memo );


      */

   void task_commit(name from, name to, asset bonus, string memo) {
    
    action act = action(
      permission_level{from,"active"_n},
      "pdjtoken"_n,
      "transfer"_n,
      std::make_tuple(from, to, bonus, memo)
    );

    act.send();
  }
};

} /// namespace eosio
