//
// Created by Dylan Fong on 2015-07-06.
//

#include "Binder.h"


int Binder::is_initialized() {
    return initialized;
}

void Binder::init() {

    print_status();
}

void Binder::print_status() {
    string binderAddr;
    string binderPort;

    cout << "BINDER_ADDR " <<  binderAddr << endl;
    cout << "BINDER_PORT " << binderPort << endl;

}