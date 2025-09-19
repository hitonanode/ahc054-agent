#include <algorithm>
#include <array>
#include <iostream>
#include <queue>
#include <vector>

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

        auto fill_direction = [&](int dir, int start_step, bool allow_extend) -> bool {
            for (int step = start_step;; ++step) {
                int target_i = pi + di[dir] * step;
                int target_j = pj + dj[dir] * step;
                if (!inside(target_i, target_j)) {
                    break;
                }
                if (is_tree[target_i][target_j]) {
                    break;
                }
                if (attempt_place(target_i, target_j)) {
                    return true;
                }
                if (!allow_extend) {
                    break;
                }
            }
            return false;
        };

        auto place_near_clear = [&](int dir) -> bool {
            for (int step = 1; step <= 3; ++step) {
                int target_i = pi + di[dir] * step;
                int target_j = pj + dj[dir] * step;
                if (!inside(target_i, target_j)) {
                    continue;
                }
                if (is_tree[target_i][target_j]) {
                    break;
                }
                std::vector<std::pair<int, bool>> neighbors;
                neighbors.reserve(8);
                for (int ddi = -1; ddi <= 1; ++ddi) {
                    for (int ddj = -1; ddj <= 1; ++ddj) {
                        if (ddi == 0 && ddj == 0) {
                            continue;
                        }
                        int ni2 = target_i + ddi;
                        int nj2 = target_j + ddj;
                        int dist = std::abs(ni2 - pi) + std::abs(nj2 - pj);
                        bool blocked = true;
                        if (inside(ni2, nj2)) {
                            blocked = is_tree[ni2][nj2];
                        }
                        neighbors.emplace_back(dist, blocked);
                    }
                }
                std::sort(neighbors.begin(), neighbors.end(), [](const auto &a, const auto &b) {
                    return a.first < b.first;
                });
                bool ok = true;
                int checked = 0;
                for (const auto &nb : neighbors) {
                    if (checked >= 3) {
                        break;
                    }
                    if (nb.second) {
                        ok = false;
                        break;
                    }
                    ++checked;
                }
                if (!ok) {
                    continue;
                }
                if (attempt_place(target_i, target_j)) {
                    return true;
                }
            }
            return false;
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

        // 条件3: 未探索隣接マスの個数に応じて遮断
        std::array<bool, 4> neighbor_unexplored{};
        const std::array<int, 4> opposite{1, 0, 3, 2};
        int unexplored_count = 0;
        for (int dir = 0; dir < 4; ++dir) {
            int ni = pi + di[dir];
            int nj = pj + dj[dir];
            if (!inside(ni, nj)) {
                continue;
            }
            if (is_tree[ni][nj]) {
                continue;
            }
            if (!confirmed[ni][nj]) {
                neighbor_unexplored[dir] = true;
                ++unexplored_count;
            }
        }

        if (unexplored_count == 3) {
            std::array<bool, 4> handled{};
            for (int dir = 0; dir < 4; ++dir) {
                if (!neighbor_unexplored[dir] || handled[dir]) {
                    continue;
                }
                int opp = opposite[dir];
                if (neighbor_unexplored[opp] && !handled[opp]) {
                    bool placed_dir = place_near_clear(dir);
                    if (!placed_dir) {
                        fill_direction(dir, 2, true);
                    }
                    handled[dir] = true;
                    bool placed_opp = place_near_clear(opp);
                    if (!placed_opp) {
                        fill_direction(opp, 2, true);
                    }
                    handled[opp] = true;
                    break;
                }
            }
            for (int dir = 0; dir < 4; ++dir) {
                if (neighbor_unexplored[dir] && !handled[dir]) {
                    fill_direction(dir, 1, false);
                }
            }
        } else if (unexplored_count == 2) {
            for (int dir = 0; dir < 4; ++dir) {
                if (!neighbor_unexplored[dir]) {
                    continue;
                }
                bool placed = place_near_clear(dir);
                if (!placed) {
                    fill_direction(dir, 2, true);
                }
            }
        } else if (unexplored_count == 1) {
            for (int dir = 0; dir < 4; ++dir) {
                if (!neighbor_unexplored[dir]) {
                    continue;
                }
                bool placed = place_near_clear(dir);
                if (!placed) {
                    fill_direction(dir, 3, true);
                }
            }
        }

        // 条件4: 隣接マスが確認済みで遠方に未確認が残る方向
        for (int dir = 0; dir < 4; ++dir) {
            int ni = pi + di[dir];
            int nj = pj + dj[dir];
            if (!inside(ni, nj)) {
                continue;
            }
            if (!confirmed[ni][nj]) {
                continue;
            }
            if (is_tree[ni][nj]) {
                continue;
            }
            bool has_future_unconfirmed = false;
            for (int step = 2;; ++step) {
                int ti2 = pi + di[dir] * step;
                int tj2 = pj + dj[dir] * step;
                if (!inside(ti2, tj2)) {
                    break;
                }
                if (is_tree[ti2][tj2]) {
                    break;
                }
                if (!confirmed[ti2][tj2]) {
                    has_future_unconfirmed = true;
                    break;
                }
            }
            if (!has_future_unconfirmed) {
                continue;
            }
            fill_direction(dir, 2, true);
        }

        std::cout << placements.size();
        for (const auto &pos : placements) {
            std::cout << ' ' << pos.i << ' ' << pos.j;
        }
        std::cout << std::endl;
    }

    return 0;
}
