#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

namespace Data {
    class Dataset {
    public:
        const static int NUMBER_OF_NUMBERS = 10;
        const static int IMAGE_SIZE = 28;
        std::vector<int> pixels;
        int label;
        Dataset(std::vector<int> m_pixels, int m_label) : pixels(m_pixels), label(m_label){};
        void print();
    };

    void Dataset::print() {
        std::cout << "Label: " << this->label;
        for(unsigned i = 0; i < this->pixels.size(); i++){
            if(i % IMAGE_SIZE == 0) std::cout << std::endl;
            char c = '.';
            if(this->pixels[i] > 100) c = '#';
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }
}

namespace ReadData {
    unsigned char readChar(std::ifstream &file) {
        // Wczytujemy z pliku 1 bajt.
        char c;
        file.read(&c, 1);

        return c;
    }

    int readInt(std::ifstream &file) {
        // Wczytujemy z pliku 4 bajty, i skladamy je w int'a.
        int result = 0;

        for (int byte = 0; byte < 4; byte++) {
            result = (result <<= 8) + readChar(file);
        }

        return result;
    }

    std::vector <std::vector<int>> read_mnist_images(std::string filename) {
        std::ifstream file(filename, std::ios::binary);

        // Pierwsza liczba w pliku to suma kontrolna.
        const int CHECKSUM_EXPECTED = 2051;

        int checksum = readInt(file);
        assert(checksum == CHECKSUM_EXPECTED);

        int number_of_images = readInt(file);
        int number_of_rows = readInt(file);
        int number_of_columns = readInt(file);
        int image_size = number_of_rows * number_of_columns;

        std::vector <std::vector<int>> images(number_of_images, std::vector <int> (image_size));
        for (auto &image : images) {
            for (int &pixel : image) {
                pixel = readChar(file);
            }
        }

        return images;
    }

    std::vector <int> read_mnist_labels(std::string filename) {
        std::ifstream file(filename, std::ios::binary);

        // Pierwsza liczba w pliku to suma kontrolna.
        const int CHECKSUM_EXPECTED = 2049;

        int checksum = readInt(file);
        std::cout << "Checksum: " << checksum << std::endl;
        assert(checksum == CHECKSUM_EXPECTED);

        int number_of_labels = readInt(file);

        std::vector <int> labels(number_of_labels);
        for (int &label : labels) {
            label = readChar(file);
        }

        return labels;
    }

    std::vector <Data::Dataset> read_data(std::string images_file_path, std::string labels_file_path){
        auto images = read_mnist_images(images_file_path);
        auto labels = read_mnist_labels(labels_file_path);

        std::vector <Data::Dataset> data_ready;
        for(unsigned i = 0; i < images.size(); i++){
            Data::Dataset data(images[i], labels[i]);
            data_ready.push_back(data);
        }

        return data_ready;
    }
}

namespace MachineLearning {
    int guess_the_number(std::vector <int> image, std::vector <std::vector <double>> values){
        std::vector <double> how_much_that_number(Data::Dataset::NUMBER_OF_NUMBERS, 0.0);

        for(int digit = 0; digit < Data::Dataset::NUMBER_OF_NUMBERS; digit++){
            for(unsigned i = 0; i < image.size(); i++){
                how_much_that_number[digit] += image[i] * values[digit][i];
            }
        }

        int most_suitable_digit = 0;
        for(int digit = 1; digit < Data::Dataset::NUMBER_OF_NUMBERS; digit++){
            if(how_much_that_number[digit] > how_much_that_number[most_suitable_digit]){
                most_suitable_digit = digit;
            }
        }

        return most_suitable_digit;
    }

    double test(std::vector <Data::Dataset> test_data, std::vector <std::vector <double>> values){
        int correct = 0;
        for(auto &data_point : test_data){
            int predict = guess_the_number(data_point.pixels, values);

            if(predict == data_point.label) ++correct;
        }
        return (double) correct / test_data.size();
    }

    void test_print(std::vector<Data::Dataset> test_data, std::vector <std::vector <double>> values){
        auto correctness = test(test_data, values);
        std::cout << 100.0 * correctness << "%" << std::endl;
    }

    double random_double(){
        const int M = 100;

        double value = (double) (rand() % (M + 1)) / M;

        return 2.0 * value - 1.0;
    }

    std::vector <std::vector <double>> train(std::vector <Data::Dataset> train_data, int steps){
        int image_size = train_data[0].pixels.size();

        std::vector <std::vector <double>> values(Data::Dataset::NUMBER_OF_NUMBERS, std::vector <double> (image_size, 0.0));

        std::string path = "path\\to\\your\\folder\\";
        std::fstream in;
        in.open(path + "values.txt");
        for(auto &value : values){
            for(auto &inner_value : value){
                in >> inner_value;
            }
        }

        std::cout << "Score before training: " << std::endl;
        test_print(train_data, values);

        double score = test(train_data, values);

        for(int step = 0; step < steps; step++){
            auto new_values = values;
            for(auto &digit_v : new_values){
                for(auto &value : digit_v){
                    value += random_double();
                }
            }

            double new_score = test(train_data, new_values);
            if(new_score > score){
                values = new_values;
                score = new_score;
                std::cout << "Step: " << step << ": " << new_score << std::endl;
            }
        }

        return values;
    }
}

int main() {

    std::string path = "path\\to\\your\\folder\\";
    auto data = ReadData::read_data(path + "train-images.idx3-ubyte", path + "train-labels.idx1-ubyte");
    data[0].print();
    data[1].print();

    auto values = MachineLearning::train(data, 20000);

    std::fstream out;
    out.open(path + "values.txt");
    for(auto &value : values){
        for(auto &inner_value : value){
            out << inner_value << " ";
        }
        out << std::endl;
    }

    return 0;
}
