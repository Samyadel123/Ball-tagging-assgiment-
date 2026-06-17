#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <random>
#include <cmath>
#include <chrono>
#include <regex>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) return -1;

    char buffer[8192] = {0};
    int valread = read(sock, buffer, 8192);
    std::string init_str(buffer);

    // Robust extraction using Regex
    std::regex num_re("\"num_balls\": ([0-9]+)");
    std::regex dur_re("\"duration\": ([0-9]+)");
    std::regex pos_re("\\[([0-9.]+), ([0-9.]+)\\]");

    std::smatch m;
    int num_balls = 0;
    if (std::regex_search(init_str, m, num_re)) num_balls = std::stoi(m[1]);
    int duration = 10;
    if (std::regex_search(init_str, m, dur_re)) duration = std::stoi(m[1]);

    std::vector<std::pair<double, double>> positions;
    auto pos_begin = std::sregex_iterator(init_str.begin(), init_str.end(), pos_re);
    auto pos_end = std::sregex_iterator();
    for (std::sregex_iterator i = pos_begin; i != pos_end; ++i) {
        positions.push_back({std::stod((*i)[1]), std::stod((*i)[2])});
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 100);
    double curr_x = dis(gen), curr_y = dis(gen);

    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() < duration) {
        curr_x = std::max(0.0, std::min(100.0, curr_x + (dis(gen)/10.0 - 5.0)));
        curr_y = std::max(0.0, std::min(100.0, curr_y + (dis(gen)/10.0 - 5.0)));

        for (int i = 0; i < num_balls; ++i) {
            if (std::sqrt(std::pow(curr_x - positions[i].first, 2) + std::pow(curr_y - positions[i].second, 2)) < 5.0) {
                std::string msg = "TAG " + std::to_string(i) + " 2\n";
                send(sock, msg.c_str(), msg.length(), 0);
                char resp[256] = {0};
                read(sock, resp, 256);
            }
        }
        usleep(100000);
    }
    close(sock);
    return 0;
}
