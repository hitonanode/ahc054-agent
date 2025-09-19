#include <iostream>
#include <vector>
#include <queue>
#include <array>

struct Position {
    int i;
    int j;
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N;
    int ti;
    int tj;
    if (!(std::cin >> N >> ti >> tj)) {
        return 0;
    }

    std::vector<std::string> initial(N);
    for (int i = 0; i < N; ++i) {
        std::cin >> initial[i];
    }

    std::vector<std::vector<bool>> is_tree(N, std::vector<bool>(N, false));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (initial[i][j] == 'T') {
                is_tree[i][j] = true;
            }
        }
    }

    std::vector<std::vector<bool>> confirmed(N, std::vector<bool>(N, false));

    const Position entrance{0, N / 2};
    const std::array<int, 4> di{-1, 1, 0, 0};
    const std::array<int, 4> dj{0, 0, -1, 1};

    auto inside = [&](int i, int j) -> bool {
        return 0 <= i && i < N && 0 <= j && j < N;
    };

    auto has_path = [&]() -> bool {
        if (!inside(entrance.i, entrance.j) || !inside(ti, tj)) {
            return false;
        }
        if (is_tree[entrance.i][entrance.j] || is_tree[ti][tj]) {
            return false;
        }
        std::vector<std::vector<bool>> visited(N, std::vector<bool>(N, false));
        std::queue<Position> q;
        q.push(entrance);
        visited[entrance.i][entrance.j] = true;
        while (!q.empty()) {
            Position cur = q.front();
            q.pop();
            if (cur.i == ti && cur.j == tj) {
                return true;
            }
            for (int dir = 0; dir < 4; ++dir) {
                int ni = cur.i + di[dir];
                int nj = cur.j + dj[dir];
                if (!inside(ni, nj)) {
                    continue;
                }
                if (visited[ni][nj]) {
                    continue;
                }
                if (is_tree[ni][nj]) {
                    continue;
                }
                visited[ni][nj] = true;
                q.push(Position{ni, nj});
            }
        }
        return false;
    };

    while (true) {
        int pi;
        int pj;
        if (!(std::cin >> pi >> pj)) {
            break;
        }
        int n;
        std::cin >> n;
        for (int k = 0; k < n; ++k) {
            int x;
            int y;
            std::cin >> x >> y;
            if (inside(x, y)) {
                confirmed[x][y] = true;
            }
        }

        if (pi == ti && pj == tj) {
            break;
        }

        std::vector<Position> placements;

        auto attempt_place = [&](int x, int y) -> bool {
            if (!inside(x, y)) {
                return false;
            }
            if (is_tree[x][y]) {
                return false;
            }
            if (confirmed[x][y]) {
                return false;
            }
            if (x == pi && y == pj) {
                return false;
            }
            if (x == ti && y == tj) {
                return false;
            }
            is_tree[x][y] = true;
            if (!has_path()) {
                is_tree[x][y] = false;
                return false;
            }
            placements.push_back(Position{x, y});
            return true;
        };

        // 条件1: 次の移動でAAに隣接し得る場合を遮断
        for (int dir = 0; dir < 4; ++dir) {
            int ni = pi + di[dir];
            int nj = pj + dj[dir];
            if (!inside(ni, nj)) {
                continue;
            }
            if (is_tree[ni][nj]) {
                continue;
            }
            int dist = std::abs(ni - ti) + std::abs(nj - tj);
            if (dist == 1) {
                attempt_place(ni, nj);
            }
        }

        // 条件2: 今ターンでAAが確認される場合の遮断
        for (int dir = 0; dir < 4; ++dir) {
            int ni = pi;
            int nj = pj;
            bool detect_target = false;
            while (true) {
                ni += di[dir];
                nj += dj[dir];
                if (!inside(ni, nj)) {
                    break;
                }
                if (is_tree[ni][nj]) {
                    break;
                }
                if (ni == ti && nj == tj) {
                    detect_target = true;
                    break;
                }
            }
            if (!detect_target) {
                continue;
            }
            ni = pi + di[dir];
            nj = pj + dj[dir];
            while (ni != ti || nj != tj) {
                if (attempt_place(ni, nj)) {
                    break;
                }
                ni += di[dir];
                nj += dj[dir];
            }
        }

        // 条件3: 新たな確認済み空きマスが増える方向を遮断
        for (int dir = 0; dir < 4; ++dir) {
            int ni = pi;
            int nj = pj;
            bool will_increase = false;
            int max_place_step = 0;
            for (int step = 1;; ++step) {
                ni += di[dir];
                nj += dj[dir];
                if (!inside(ni, nj)) {
                    break;
                }
                if (is_tree[ni][nj]) {
                    break;
                }
                max_place_step = step;
                if (!confirmed[ni][nj]) {
                    will_increase = true;
                }
            }
            if (!will_increase) {
                continue;
            }

            auto try_step = [&](int step) -> bool {
                if (step <= 0 || step > max_place_step) {
                    return false;
                }
                int target_i = pi + di[dir] * step;
                int target_j = pj + dj[dir] * step;
                return attempt_place(target_i, target_j);
            };

            if (try_step(2)) {
                continue;
            }
            if (try_step(1)) {
                continue;
            }
            for (int step = 3; step <= max_place_step; ++step) {
                if (try_step(step)) {
                    break;
                }
            }
        }

        std::cout << placements.size();
        for (const auto &pos : placements) {
            std::cout << ' ' << pos.i << ' ' << pos.j;
        }
        std::cout << std::endl;
    }

    return 0;
}
