#pragma once

#include "database.h"
#include <iostream>
#include <fstream>

class exporter
{
    public: 
    exporter() {}; 
    ~exporter() {}; 
    void export_data(database* pData, std::string file_path);
};
