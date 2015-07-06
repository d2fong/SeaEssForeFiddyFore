//
// Created by Dylan Fong on 2015-07-05.
//

#include <iostream>


#include "ClientTest.h"
#include "../Client.h"
#include <string>
#include <iostream>

using namespace std;

int main () {

    string test_name = "connect_to_binder_invalid_binder_credentials";
    Client c = Client();
    int connection = c.connect_to_binder("dsjfksdfkjs", "dsjfksdjfks");
    int expection = -1;

    bool result = (expection == connection);

    cout << test_name << " is " << result << endl;
    return 0;

}
