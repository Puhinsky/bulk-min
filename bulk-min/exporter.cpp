#include "exporter.h"

void exporter::export_data(database* pData, std::string file_path)
{
    if (pData == NULL) 
    {
        log::error(EXPORTER, "pData has not been passed to exporter!");
        return;
    }
    
    std::ofstream outputFile;
    outputFile.open(file_path);
    if (!outputFile.is_open())
    {
        log::error(EXPORTER, "counldn't open: " + file_path);
    }

    // HEADER
    outputFile << "GRID\n" << std::to_string((int)pData->m_header.length) << " " << std::to_string((int)pData->m_header.space_segments) << " " << std::to_string((int)pData->m_header.time_segments) << "\n/\n";
    
    // TUBE
    outputFile << "TUBE\n";
    for (int i = 1; i <= pData->m_header.space_segments; i++) 
        outputFile << std::to_string(pData->m_conductivities[i]) << " ";
    outputFile << "/\n";

    // TEMPERATURE
    outputFile << "TEMP\n";
    for (int i = 0; i < pData->m_header.time_segments; i++)
    {
        outputFile << std::to_string((double) pData->m_time_scale[i]) << " ";
        for (int j = 0; j < pData->m_header.space_segments; j++)
            outputFile << std::to_string((double) pData->m_temperatures[(i * pData->m_header.space_segments) + j]) << " ";
    }
    outputFile << "/\n"; 
    outputFile.close();

    log::success(EXPORTER, "successfuly export");
};