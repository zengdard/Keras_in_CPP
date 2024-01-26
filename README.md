# README

## Description

This C++ code is designed to make predictions using a Tensorflow Keras model. The main functionality of the code involves converting packet data from pcap format to CSV, processing the CSV data, and making predictions using a pre-trained neural network model.

## Dependencies

Before running the code, make sure you have the following dependencies installed:

- [TensorFlow C++ API](https://www.tensorflow.org/install/lang_c)
- [frugally-deep] - [frugally-deep GitHub Repository](https://github.com/Dobiasd/frugally-deep/)
- [C++17](https://en.cppreference.com/w/cpp/17)

## Usage

1. **Packet Conversion and CSV Processing:**

    ```cpp
    pcapToCsv();
    ```

    This function converts a pcap file (`dump.pcap`) to CSV format using a shell script (`convert_pcap_csv.sh`). It then processes the CSV data by removing unnecessary columns.

2. **Scientific Notation Processing:**

    ```cpp
    process_csv_scientific();
    ```

    This function processes CSV data, specifically handling scientific notation in the CSV cells. It converts scientific notation to its corresponding decimal form.

3. **CSV Data Reading and Model Prediction:**

    ```cpp
    read_and_process_csv();
    ```

    This function reads the processed CSV data from `finaldump.csv`, loads a pre-trained neural network model (`model.json`), and makes predictions on each line of the CSV data. The predicted results are mapped to human-readable labels using a predefined status map.

## Additional Notes

- **Status Map:**

    The predictions are mapped to human-readable labels using the `statusMap` defined in the code.

- **Invalid Data Handling:**

    If invalid or non-numeric values are encountered in the CSV data during processing, a warning is printed, and the corresponding value is set to 0.0f.

- **Output:**

    The predicted results are printed to the console and stored in the `sharedVector` vector.

## Running the Code

Compile and run the code using a C++ compiler with C++17 support. Make sure to link against the TensorFlow C++ library and include the necessary dependencies.

```bash
g++ -std=c++17 -o your_executable_name your_source_code.cpp -ltensorflow
./your_executable_name
```

Note: Ensure that the paths to the model file and other resources are correctly specified in the code.

## License

This code is provided under the [MIT License](LICENSE). Feel free to use, modify, and distribute it according to the terms of the license.
