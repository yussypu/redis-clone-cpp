#pragma once
#include "store.hpp"

void runServer(Store& store);
void clientHandler(int clientSocket, Store& store);
