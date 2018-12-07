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

   class [[eosio::contract("pdjtoken")]] token : public contract {
      public:
         using contract::contract;

         [[eosio::action]]
         void create( name   issuer,
                      asset  maximum_supply);

         [[eosio::action]]
         void issue( name to, asset quantity, string memo );

         [[eosio::action]]
         void transfer( name    from,
                        name    to,
                        asset   quantity,
                        string  memo );




      private:

         inline asset get_supply( name token_contract_account, symbol_code sym_code )const;

         inline asset get_balance( name token_contract_account, name owner, symbol_code sym_code )const;

      private:

         struct [[eosio::table]] account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
         };

         struct [[eosio::table]] currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
         };

         typedef eosio::multi_index< "accounts"_n, account > accounts;
         typedef eosio::multi_index< "stat"_n, currency_stats > stats;

         void sub_balance( name owner, asset value );
         void add_balance( name owner, asset value, name ram_payer );
   };

asset token::get_supply( name token_contract_account, symbol_code sym_code )const
{
   stats statstable( token_contract_account, sym_code.raw() );
   const auto& st = statstable.get( sym_code.raw() );
   return st.supply;
}

asset token::get_balance( name token_contract_account, name owner, symbol_code sym_code )const
{
   accounts accountstable( token_contract_account, owner.value );
   const auto& ac = accountstable.get( sym_code.raw() );
   return ac.balance;
}


class [[eosio::contract]] pdjtask : public token {
   public:
      /*
      pdjtask( name self ):  pdjtoken(self){
            
      }*/

      //创建任务
      [[eosio::action]]
      void createtk( name creator, name worker, asset taskBonus, string memo );
      //提交任务
      [[eosio::action]]
      void commit( uint64_t taskID, name worker, string memo );
      //验收任务
      [[eosio::action]]
      void confirm( uint64_t taskID, name creator, uint8_t ok = 1 );

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
};

} /// namespace eosio
