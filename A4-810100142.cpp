
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <iomanip>

#define short_dep_request "create_short_term_deposit"
#define long_dep_request "create_long_term_deposit"
#define gharz_dep_request "create_gharzolhasane_deposit"
#define pass_month "pass_time"
#define dep_inventory_report "inventory_report"
#define bank_inventory_report "calc_money_in_bank"
#define total_inventory_report "calc_all_money"
#define no_money "Not enough money"
#define ok "OK"
#define wrong_linked_dep "Invalid short-term deposit"

using namespace std;

class Gh_deposit {
public:
    int id;
    int bank_id;
    int owner_id;
    float first_investment;

private:

};

class Short_deposit {
public:
    int id;
    int profit_margin;
    int bank_id;
    int owner_id;
    float first_investment;
    int linked_long_dep_id;
    float inventory;
private:

};

class Long_deposit {
public:
    int id;
    int profit_margin;
    int bank_id;
    int owner_id;
    float first_investment;
    int linked_short_dep_id;
private:

};


class User {
public:
    int id;
    float wallet;

private:

};

class Bank {
public:
    int id;
    int profit_margin;
    float min_investment;
    int last_dep_id = 0;
    vector<Gh_deposit *> gh_deposits;
    vector<Short_deposit *> short_deposits;
    vector<Long_deposit *> long_deposits;

    bool can_make_short_deposit(float const &user_budget, float const &user_first_inv) {
        if (user_budget >= user_first_inv && user_first_inv >= min_investment)
            return true;
        else
            return false;
    }

    Short_deposit *select_short_dep(int const &sh_dep_id) {
        for (Short_deposit *dep: short_deposits) {
            if (dep->id == sh_dep_id) {
                return dep;
            }
        }
        return nullptr;
    }

    bool linked_short_dep_exists(int const &sh_dep_id) {
        for (int i = 0; i < short_deposits.size(); i++) {
            if (short_deposits[i]->id == sh_dep_id) {
                return true;
            } else
                continue;
        }
        return false;
    }

    bool enough_money_to_make_long_deposit(User *user, float const &user_first_inv, int const &linked_sh_dep,
                                           int const &years) {
        if (user->wallet >= user_first_inv && user_first_inv >= min_investment * years)
            return true;
        else
            return false;
    }

    bool linked_short_dep_is_true(User *user, int const &linked_short_dep_id) {
        if (linked_short_dep_exists(linked_short_dep_id)) {
            Short_deposit *linked_short_dep = select_short_dep(linked_short_dep_id);
            if (linked_short_dep == nullptr) {
                return false;
            }
            if (linked_short_dep->owner_id == user->id) {
                return true;
            } else
                return false;
        }
        return false;
    }

    void short_dep_creator(float const &first_inv, int const &owner) {
        Short_deposit *sh_dep = new Short_deposit;
        sh_dep->bank_id = id;
        sh_dep->first_investment = first_inv;
        sh_dep->linked_long_dep_id = 0;
        sh_dep->owner_id = owner;
        sh_dep->profit_margin = profit_margin;
        sh_dep->id = gh_deposits.size() + short_deposits.size() + long_deposits.size() + 1;
        last_dep_id = sh_dep->id;
        sh_dep->inventory = first_inv;
        short_deposits.push_back(sh_dep);

    }

    void gharz_dep_creator(float const &first_inv, int const &owner) {
        Gh_deposit *gh_dep = new Gh_deposit;
        gh_dep->bank_id = id;
        gh_dep->first_investment = first_inv;
        gh_dep->owner_id = owner;
        gh_dep->id = gh_deposits.size() + short_deposits.size() + long_deposits.size() + 1;
        last_dep_id = gh_dep->id;
        gh_deposits.push_back(gh_dep);
    }

    void
    long_dep_creator(float const &first_inv, int const &owner, int const &linked_short_dep_id, int const &years) {
        Long_deposit *long_dep = new Long_deposit;
        long_dep->bank_id = id;
        long_dep->first_investment = first_inv;
        long_dep->owner_id = owner;
        long_dep->profit_margin = profit_margin * years;
        long_dep->id = last_dep_id + 1;
        last_dep_id = +1;
        long_dep->linked_short_dep_id = linked_short_dep_id;
        long_deposits.push_back(long_dep);
    }

    void long_dep_profit(int const &linked_sh_dep_id, Long_deposit *const l_dep) {
        select_short_dep(linked_sh_dep_id)->inventory += (l_dep->profit_margin * l_dep->first_investment / 100);
    }

    float calc_inventory_in_bank(int const &user_id) {
        float inventory_in_bank = 0;
        for (int j = 0; j < short_deposits.size(); j++) {
            if (short_deposits[j]->owner_id == user_id) {
                inventory_in_bank += short_deposits[j]->inventory;
            } else
                continue;
        }

        if (long_deposits.size() != 0) {
            for (int k = 0; k < long_deposits.size(); k++) {
                if (long_deposits[k]->owner_id == user_id)
                    inventory_in_bank += long_deposits[k]->first_investment;
            }
        }
        if (gh_deposits.size() != 0) {
            for (int k = 0; k < gh_deposits.size(); k++) {
                if (gh_deposits[k]->owner_id == user_id)
                    inventory_in_bank += gh_deposits[k]->first_investment;
            }
        }
        return inventory_in_bank;

    }

private:

};

class Banking {
public:
    string banks_csv_name = "sample_banks.csv" ;
    string users_csv_name ="sample_users.csv";
    vector<User *> users;
    vector<Bank *> banks;

    void handle_command_line(int argc, char **argv) {
        for (int i = 1; i < argc; i = i + 2) {
            if (argv[i][1] == 'b') {
                banks_csv_name = argv[i + 1];
            } else if (argv[i][1] == 'u') {
                users_csv_name = argv[i + 1];
            }
        }
    }

    void get_banks_csv() {
        ifstream file(banks_csv_name);
        string line;
        int i = 0;
        getline(file, line);
        while (getline(file, line)) {
            stringstream ss(line);
            string id, profit_margin, min_investment;
            getline(ss, id, ',');
            getline(ss, profit_margin, ',');
            getline(ss, min_investment, ',');
            Bank *bank = new Bank(
                    {stoi(id), stoi(profit_margin), (float) stoi(min_investment)});
            banks.push_back(bank);
            i++;
        }
    }

    void get_users_csv() {
        ifstream file(users_csv_name);
        string line;
        int i = 0;
        getline(file, line);
        while (getline(file, line)) {
            stringstream ss(line);
            string id, wallet;
            getline(ss, id, ',');
            getline(ss, wallet, ',');
            User *user = new User({stoi(id), (float) stoi(wallet)});
            users.push_back(user);
            i++;
        }
    }


    void handle_short_dep_request() {
        int user_id, bank_id;
        float first_inv;
        cin >> user_id >> bank_id >> first_inv;
        Bank *bank = select_bank(bank_id);
        User *user = select_user(user_id);
        if(user == nullptr || bank == nullptr){
            return;
        }
        if (bank->can_make_short_deposit(user->wallet, first_inv)) {
            bank->short_dep_creator(first_inv, user_id);
            user->wallet = user->wallet - first_inv;
            cout << bank->last_dep_id << endl;
        } else {
            cout << no_money << endl;
        }
    }

    void handle_gharz_dep_request() {
        int user_id, bank_id;
        float first_inv;
        cin >> user_id >> bank_id >> first_inv;
        Bank *bank = select_bank(bank_id);
        if(bank == nullptr){
            return;
        }
        bank->gharz_dep_creator(first_inv, user_id);
        cout << ok << endl;

    }

    void handle_long_dep_request() {
        int user_id, bank_id, linked_short_dep_id, years;
        float first_inv;
        cin >> user_id >> bank_id >> linked_short_dep_id >> years >> first_inv;
        Bank *bank = select_bank(bank_id);
        User *user = select_user(user_id);
        if(user == nullptr || bank == nullptr){
            return;
        }
        if (bank->linked_short_dep_is_true(user, linked_short_dep_id)) {
            if (bank->enough_money_to_make_long_deposit(user, first_inv, linked_short_dep_id, years)) {
                bank->long_dep_creator(first_inv, user_id, linked_short_dep_id, years);
                user->wallet = user->wallet - first_inv;
                cout << ok << endl;
            } else {
                cout << no_money << endl;

                return;
            }
        } else {
            cout << wrong_linked_dep << endl;
        }

    }


    void handle_pass_month_request() {
        int months;
        cin >> months;
        for (int i = 0; i < months; i++)
            give_one_month_profit();
        cout << ok << endl;
    }

    void handle_dep_inventory_report_request() {
        int user_id, bank_id, short_dep_id;
        cin >> user_id >> bank_id >> short_dep_id;
        Bank *bank = select_bank(bank_id);
        Short_deposit *short_dep = bank->select_short_dep(short_dep_id);
        if (short_dep == nullptr) {
            cout << wrong_linked_dep << endl;
            return;
        }
        if (short_dep->owner_id == user_id) {
            float dep_inventory = short_dep->inventory;
            cout << fixed << setprecision(2);
            cout << dep_inventory << endl;
        } else {
            cout << wrong_linked_dep << endl;
        }
    }

    void handle_bank_inventory_report_request() {
        int user_id, bank_id;
        cin >> user_id >> bank_id;
        Bank *bank = select_bank(bank_id);if(bank == nullptr){
            return;
        }
        float inventory_in_bank = bank->calc_inventory_in_bank(user_id);
        cout << fixed << setprecision(2);
        cout << inventory_in_bank << endl;
    }

    void handle_total_inventory_report_request() {
        int user_id;
        cin >> user_id;
        float total_inventory = 0;
        for (int i = 0; i < banks.size(); i++) {
            total_inventory += banks[i]->calc_inventory_in_bank(user_id);
        }
        cout << fixed << setprecision(2);
        cout << total_inventory << endl;
    }

    void handle_admin_requests() {
        string user_input;
        while (true) {
            cin >> user_input;
            if (user_input == short_dep_request) {
                handle_short_dep_request();
                continue;
            } else if (user_input == long_dep_request) {
                handle_long_dep_request();
                continue;
            } else if (user_input == gharz_dep_request) {
                handle_gharz_dep_request();
                continue;
            } else if (user_input == pass_month) {
                handle_pass_month_request();
                continue;
            } else if (user_input == dep_inventory_report) {
                handle_dep_inventory_report_request();
                continue;
            } else if (user_input == bank_inventory_report) {
                handle_bank_inventory_report_request();
                continue;
            } else if (user_input == total_inventory_report) {
                handle_total_inventory_report_request();
                continue;
            }

        }
    }


private:
    void give_one_month_profit() {
        for (int i = 0; i < banks.size(); i++) {
            for (int j = 0; j < banks[i]->short_deposits.size(); j++) {
                banks[i]->short_deposits[j]->inventory += (banks[i]->short_deposits[j]->profit_margin *
                                                           banks[i]->short_deposits[j]->first_investment / 100);
            }
            if (banks[i]->long_deposits.size() != 0) {
                for (int k = 0; k < banks[i]->long_deposits.size(); k++) {
                    banks[i]->long_dep_profit(banks[i]->long_deposits[k]->linked_short_dep_id,
                                              banks[i]->long_deposits[k]);
                }
            }
        }
    }

    Bank *select_bank(int const &bank_id) {
        for (Bank *bank: banks) {
            if (bank->id == bank_id)
                return bank;
        }
        return nullptr;
    }

    User *select_user(int const &user_id) {
        for (User *user: users) {
            if (user->id == user_id)
                return user;
        }
        return nullptr;
    }
};


int main(int argc, char **argv) {
    Banking BANKING;
    BANKING.handle_command_line(argc, argv);
    BANKING.get_users_csv();
    BANKING.get_banks_csv();
    BANKING.handle_admin_requests();
    return 0;
}
