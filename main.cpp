#include <limits>
#include "../../include/packet/packetToCsv.h"


std::vector<std::string> sharedVector;

void pcapToCsv() {

#ifdef __linux__
    system("./../pcaptocsv/convert_pcap_csv.sh dump.pcap");
    process_csv();
    system("rm ./../pcaptocsv/csv/dump_ISCX.csv");
    process_csv_scientific();
    system("rm tmpdump.csv");
    read_and_process_csv();
    system("rm finaldump.csv");
#endif

}

void process_csv() {
    std::ifstream input_file("../pcaptocsv/csv/dump_ISCX.csv");
    std::ofstream output_file("tmpdump.csv");
    std::string line;

    // Skip the first line (header)
    std::getline(input_file, line);

    while (std::getline(input_file, line)) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;


        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        // Assuming the order of the columns is known and fixed
        for (int i = 0; i < tokens.size(); ++i) {
            if (i != 0 && i != 1 && i != 2 && i != 3 && i != 4 && i != 6 && i != tokens.size() - 1) {
                output_file << tokens[i];
                if (i < tokens.size() - 1) {
                    output_file << ",";
                }
            }
        }
        output_file << "\n";
    }

    input_file.close();
    output_file.close();
}

bool contains_exponent(const std::string& s) {
    return s.find('E') != std::string::npos || s.find('e') != std::string::npos;
}

std::string process_scientific_notation(const std::string& s) {
    std::stringstream ss(s);
    long double number;
    ss >> number; // Read the number in its complete precision

    std::ostringstream out;
    out << std::fixed << std::setprecision(std::numeric_limits<long double>::digits10) << number;

    std::string result = out.str();
    // Remove trailing zeros
    result.erase(result.find_last_not_of('0') + 1, std::string::npos);
    // Remove the decimal point if it is now at the end
    if (result.back() == '.') {
        result.pop_back();
    }
    return result;
}

void process_csv_scientific() {
    std::ifstream input_file("tmpdump.csv");
    std::ofstream output_file("finaldump.csv");
    std::string line;
    bool first_line = true;

    while (std::getline(input_file, line)) {
        if (first_line) {
            // Write the first line (header) as is
            output_file << line << std::endl;
            first_line = false;
        } else {
            std::istringstream iss(line);
            std::string token;
            bool first_token = true;
            while (std::getline(iss, token, ',')) {
                if (!first_token) {
                    output_file << ",";
                }
                if (contains_exponent(token)) {
                    output_file << process_scientific_notation(token);
                } else {
                    output_file << token;
                }
                first_token = false;
            }
            output_file << std::endl;
        }
    }

    input_file.close();
    output_file.close();
}

std::vector<float> process_line_to_vector(const std::string& line) {
    std::vector<float> result;
    std::stringstream ss(line);
    std::string cell;

    while (std::getline(ss, cell, ',')) {
        try {
            // Check if the cell can be converted to float
            if (cell.empty() || !std::all_of(cell.begin(), cell.end(), [](unsigned char c) { return std::isdigit(c) || c == '.'; })) {
                throw std::invalid_argument("Non-numeric value found");
            }

            float value = std::stof(cell);
            result.push_back(value);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Warning: Invalid argument encountered in CSV data: " << e.what() << " - Value: " << cell << std::endl;
            result.push_back(0.0f); // You can decide how to handle non-convertible values
        }
    }

    // Add any additional elements if needed
    result.push_back(0.0f);
    result.push_back(0.0f);
    return result;
}

void read_and_process_csv() {
    std::map<int, std::string> statusMap = {
        {0, "BENIGN"},
        {1, "Portscan"},
        {2, "DoS Hulk"},
        {3, "DDos"},
        {4, "Infiltration - Portscan"},
        {5, "DoS GoldenEye"},
        {6, "FTP-Patator"},
        {7, "DoS Slowloris"},
        {8, "SSH-Patator"},
        {9, "DoS Slowhttptest"},
        {10, "Botnet"},
        {11, "Brute Force"},
        {12, "Infiltration"},
        {13, "XSS"},
        {14, "SQL Injection"},
        {15, "Heartbleed"}
    };
    std::ifstream file("finaldump.csv");
    std::string line;

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file finaldump.csv" << std::endl;
        return;
    }

    // Skip the first line (header)
    std::getline(file, line);

    // Load the model
    const auto model = fdeep::load_model("../src/model/model.json");

    while (std::getline(file, line)) {
        std::vector<float> line_vector = process_line_to_vector(line);
        const auto result = model.predict({fdeep::tensor(fdeep::tensor_shape(79), line_vector)});

        // Process the model's result
        int maxPosition = findMaxPositionInTensors(result);
        auto it = statusMap.find(maxPosition);

        if (it != statusMap.end()) {
            sharedVector.push_back(it->second);
            std::cout << it->second << std::endl;
        } else {
            sharedVector.push_back("Unknown Status");
            std::cout << "Unknown Status" << std::endl;
        }
    }

    file.close();
}

int findMaxPositionInTensors(const fdeep::tensors& tensors) {
    int maxPosition = 0;
    float max = tensors[0].as_vector()->at(0);
    for (int i = 1; i < tensors[0].as_vector()->size(); ++i) {
        if (tensors[0].as_vector()->at(i) > max) {
            max = tensors[0].as_vector()->at(i);
            maxPosition = i;
        }
    }
    return maxPosition;
}
