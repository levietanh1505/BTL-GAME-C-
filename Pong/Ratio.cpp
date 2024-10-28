#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>

// Class xử lý tỉ số trong game
class Ratio {
private:
    std::string fileRatio = "resources/ratioGame.txt";
    std::deque<std::pair<int, int>> listRatio;
public:
    std::string getRatioGame() {
        std::string res = "";
        std::ifstream file(fileRatio);

        int x, y;
        while (file >> x >> y) {
            listRatio.push_back({ x, y });
        }
        file.close();

        for (const auto& i : listRatio) {
            res += std::to_string(i.first) + " - " + std::to_string(i.second) + "\n";
        }

        return (res == "") ? "NO HISTORY" : res;
    }

    void writeRatioToFile(int A, int B) {
        listRatio.clear();
        std::ifstream file(fileRatio);
        int x, y;
        while (file >> x >> y) {
            listRatio.push_front({ x, y });
        }
        file.close();

        // Giữ tối đa 3 lịch sử gần nhất
        while (listRatio.size() >= 3) listRatio.pop_back();

        // Thêm tỷ số mới vào đầu danh sách
        listRatio.push_front({ A, B });

        // Ghi lại tất cả dữ liệu trong deque vào file
        std::ofstream file2(fileRatio);
        for (const auto& i : listRatio) {
            file2 << i.first << " " << i.second << "\n";
        }
        file2.close();
    }
};